/* =======================================================
   N.O.V.A. SECURE LOCK - FINAL MASTER INTEGRATION
   ======================================================= */

#include <NOVA_Secure_Lock_inferencing.h> // <-- CHANGE THIS TO YOUR EXACT AI LIBRARY NAME!
#include <Wire.h>
#include <Adafruit_PN532.h>

// --- HARDWARE PINS (ESP32-S3) ---
const int SENSOR_PIN = 1;  // Output from LM358 Pin 1 (Photodiode)
const int RELAY_PIN  = 4;  // Output to Door Lock Relay
const int SDA_PIN    = 8;  // ESP32-S3 I2C SDA
const int SCL_PIN    = 9;  // ESP32-S3 I2C SCL

// --- PN532 I2C SETUP ---
// Using (-1, -1) because diagnostic proved IRQ/RESET aren't needed for your I2C setup
Adafruit_PN532 nfc(-1, -1);

// --- KNOWN NFC UIDs (2-Factor Mapping) ---
byte adminUID[4] = {0xAE, 0x9A, 0xF9, 0x06}; // Expects 30Hz
byte staffUID[4] = {0x03, 0x5F, 0x1A, 0x32}; // Expects 20Hz
byte guestUID[4] = {0xDE, 0xB0, 0x29, 0x07}; // Expects 10Hz

// Array to hold exactly 1 second of light data (100 samples)
float features[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];

void setup() {
    Serial.begin(115200);
    delay(1000); // Give serial monitor time to connect
    
    // Initialize standard hardware
    pinMode(SENSOR_PIN, INPUT);
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW); // Default to LOCKED
    
    // Initialize I2C Bus with your specific pins
    Wire.begin(SDA_PIN, SCL_PIN);
    
    // Initialize PN532 NFC
    Serial.println("Initializing PN532 NFC Reader...");
    nfc.begin();

    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata) {
        Serial.println("ERROR: NFC Board not found! Check I2C pins and power.");
        while (1); // Halt system if NFC fails
    }
    
    // Configure board to read RFID tags
    nfc.SAMConfig();

    Serial.println("\n=========================================");
    Serial.println("   N.O.V.A. SECURE AI LOCK - ONLINE");
    Serial.println("=========================================");
    Serial.println("System Locked. Waiting for NFC Card...");
}

void loop() {
    // ---------------------------------------------------------
    // STEP 1: NFC SCANNING GATEWAY (Non-Blocking)
    // ---------------------------------------------------------
    uint8_t success;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
    uint8_t uidLength;

    // 50ms timeout ensures the ESP32 doesn't freeze while waiting for a card
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 50);

    // If no 4-byte card is found, exit the loop instantly and start over
    if (!success || uidLength != 4) {
        return; 
    }

    String userRole = "UNKNOWN";
    String expectedOpticalKey = "";

    // Check which card was tapped
    if (compareUID(uid, adminUID)) {
        userRole = "ADMIN";
        expectedOpticalKey = "freq_30hz";
    } 
    else if (compareUID(uid, staffUID)) {
        userRole = "STAFF";
        expectedOpticalKey = "freq_20hz";
    } 
    else if (compareUID(uid, guestUID)) {
        userRole = "GUEST";
        expectedOpticalKey = "freq_10hz";
    }

    // Unregistered card check
    if (userRole == "UNKNOWN") {
        Serial.println("\n==================================");
        Serial.print("[SECURITY] Unregistered UID: ");
        for (uint8_t i = 0; i < uidLength; i++) {
            Serial.print(" 0x");
            if (uid[i] <= 0x0F) Serial.print("0");
            Serial.print(uid[i], HEX);
        }
        Serial.println("\n>>> ACCESS DENIED <<<");
        Serial.println("==================================");
        delay(2000);
        return;
    }

    // Registered card found! Move to Step 2.
    Serial.println("\n==================================");
    Serial.print("[NFC] ");
    Serial.print(userRole);
    Serial.println(" Card Authorized.");
    Serial.print("[SYSTEM] Required Optical Key: ");
    Serial.println(expectedOpticalKey);
    Serial.println("[AI] Initiating Optical Scan... Point flashlight NOW.");
    Serial.println("==================================");
    
    delay(1500); // Give the user 1.5 seconds to aim their flashlight

    // ---------------------------------------------------------
    // STEP 2: CAPTURE THE LIGHT DATA (100Hz for 1 second)
    // ---------------------------------------------------------
    for (int i = 0; i < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE; i++) {
        features[i] = analogRead(SENSOR_PIN);
        delay(10); // Exactly 10ms delay = 100Hz sample rate
    }

    // ---------------------------------------------------------
    // STEP 3: RUN THE EDGE IMPULSE NEURAL NETWORK
    // ---------------------------------------------------------
    Serial.println("[AI] Analyzing optical signal...");
    signal_t signal;
    int err = numpy::signal_from_buffer(features, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
    
    ei_impulse_result_t result = { 0 };
    err = run_classifier(&signal, &result, false); 

    // Find the label with the highest confidence
    String ai_decision = "";
    float highest_confidence = 0.0;

    for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        if (result.classification[i].value > highest_confidence) {
            highest_confidence = result.classification[i].value;
            ai_decision = result.classification[i].label;
        }
    }

    Serial.print("[AI] Verdict: ");
    Serial.print(ai_decision);
    Serial.print(" (Confidence: ");
    Serial.print(highest_confidence * 100, 1);
    Serial.println("%)");

    // ---------------------------------------------------------
    // STEP 4: THE FINAL 2FA SECURITY CHECK
    // ---------------------------------------------------------
    // Requires both a matching frequency AND at least 85% confidence from the AI
    if (ai_decision == expectedOpticalKey && highest_confidence > 0.85) {
        Serial.println("\n>>> 2FA SUCCESS: UNLOCKING DOOR <<<");
        digitalWrite(RELAY_PIN, HIGH); // Turn Relay ON
        
        delay(5000); // Keep door open for 5 seconds
        
        digitalWrite(RELAY_PIN, LOW); // Turn Relay OFF
        Serial.println(">>> DOOR LOCKED. <<<");
    } else {
        Serial.println("\n>>> 2FA FAILED: INVALID OPTICAL KEY. ACCESS DENIED. <<<");
    }
    
    Serial.println("\nWaiting for next NFC Card...");
}

// --- HELPER FUNCTION: Compare two UIDs ---
bool compareUID(uint8_t* scannedUID, byte* knownUID) {
    for (byte i = 0; i < 4; i++) {
        if (scannedUID[i] != knownUID[i]) {
            return false;
        }
    }
    return true;
}