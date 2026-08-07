/**
 * @file    test_ss1_hal.ino
 * @brief   Validation Test Harness — Subsystem 1 Hardware Abstraction Layer.
 * @author  Kiruthik R S
 * @date    2026-08-07
 * 
 * Interactive serial test harness to independently verify:
 * 1. NFC Card Reading & Role Resolution (HW-04, FW-02, FW-04)
 * 2. Timer-driven Optical Sampler & ADC Saturation (HW-02, FW-01)
 * 3. Relay Unlock & Auto-Relock Timing (HW-05)
 */

#include "../subsystem1_secure_access/config.h"
#include "../subsystem1_secure_access/hal/nfc_reader.h"
#include "../subsystem1_secure_access/hal/optical_sampler.h"
#include "../subsystem1_secure_access/hal/relay_lock.h"

void printMenu() {
    Serial.println("\n==========================================");
    Serial.println("   SS1 HAL VALIDATION TEST HARNESS");
    Serial.println("==========================================");
    Serial.println(" 1 - Test NFC Reader (Poll & Resolve)");
    Serial.println(" 2 - Test Optical Sampler (100 Hz Timer)");
    Serial.println(" 3 - Test Solenoid Relay (5s Timed Unlock)");
    Serial.println(" 4 - Run All HAL Diagnostics");
    Serial.println(" Enter option (1-4):");
}

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    unsigned long start = millis();
    while (!Serial && (millis() - start < SERIAL_INIT_DELAY_MS));

    Serial.println("\n[TEST_HAL] Initializing Subsystem 1 HAL Modules...");

    bool nfcOk = nfc_init();
    Serial.print("[TEST_HAL] NFC Init: "); Serial.println(nfcOk ? "PASS" : "FAIL");

    bool samplerOk = sampler_init();
    Serial.print("[TEST_HAL] Sampler Init: "); Serial.println(samplerOk ? "PASS" : "FAIL");

    relay_init();
    Serial.println("[TEST_HAL] Relay Init: PASS");

    printMenu();
}

void testNfc() {
    Serial.println("\n--- [TEST 1] NFC Card Reader ---");
    Serial.println("Please scan an NFC card within 5 seconds...");
    unsigned long startTime = millis();
    bool cardDetected = false;

    while (millis() - startTime < 5000) {
        NfcCard_t card;
        if (nfc_pollCard(&card)) {
            cardDetected = true;
            Serial.print("Card UID:");
            for (uint8_t i = 0; i < card.uid_length; i++) {
                Serial.print(" 0x");
                if (card.uid[i] <= 0x0F) Serial.print("0");
                Serial.print(card.uid[i], HEX);
            }
            Serial.println();

            NfcRole_t role = nfc_resolveRole(&card);
            switch (role) {
                case NFC_ROLE_ADMIN: Serial.println("Resolved Role: ADMIN (Expected: 30 Hz)"); break;
                case NFC_ROLE_STAFF: Serial.println("Resolved Role: STAFF (Expected: 20 Hz)"); break;
                case NFC_ROLE_GUEST: Serial.println("Resolved Role: GUEST (Expected: 10 Hz)"); break;
                default:             Serial.println("Resolved Role: UNKNOWN (Unregistered UID)"); break;
            }
            break;
        }
        delay(10);
    }
    if (!cardDetected) {
        Serial.println("RESULT: No card detected within 5 seconds.");
    }
}

void testOpticalSampler() {
    Serial.println("\n--- [TEST 2] Timer-Driven Optical Sampler ---");
    Serial.println("Starting 100-sample capture @ 100 Hz...");

    if (!sampler_startCapture()) {
        Serial.println("ERROR: Could not start capture!");
        return;
    }

    uint32_t waitStart = micros();
    while (!sampler_isComplete()) {
        // Yield/wait
    }
    uint32_t elapsedUs = sampler_getElapsedUs();
    const float* buf = sampler_getBuffer();
    bool saturated = sampler_wasSaturated();

    Serial.print("Capture Completed in: "); Serial.print(elapsedUs); Serial.println(" us");
    Serial.print("Expected Duration:   1000000 us (1.000 s)"); Serial.println();
    Serial.print("Timing Accuracy:     ");
    float errPct = ((float)elapsedUs - 1000000.0f) / 10000.0f;
    Serial.print(errPct, 2); Serial.println("%");

    Serial.print("ADC Saturation Detected: ");
    Serial.println(saturated ? "YES (Signal Clipping)" : "NO (Clean Range)");

    Serial.println("First 10 ADC Samples:");
    for (int i = 0; i < 10; i++) {
        Serial.print("  ["); Serial.print(i); Serial.print("] ");
        Serial.println(buf[i], 1);
    }
}

void testRelay() {
    Serial.println("\n--- [TEST 3] Relay Unlock & Auto-Relock ---");
    Serial.println("Unlocking solenoid...");
    relay_unlock();
    Serial.print("Relay status: "); Serial.println(relay_isUnlocked() ? "UNLOCKED" : "LOCKED");

    Serial.println("Waiting for auto-relock timer (5000 ms)...");
    unsigned long startMs = millis();
    while (relay_isUnlocked()) {
        relay_tick();
        delay(50);
    }
    unsigned long openDuration = millis() - startMs;
    Serial.print("Auto-relock completed in: "); Serial.print(openDuration); Serial.println(" ms");
    Serial.print("Relay status: "); Serial.println(relay_isUnlocked() ? "UNLOCKED" : "LOCKED");
}

void loop() {
    if (Serial.available() > 0) {
        char ch = Serial.read();
        if (ch == '1') { testNfc(); printMenu(); }
        else if (ch == '2') { testOpticalSampler(); printMenu(); }
        else if (ch == '3') { testRelay(); printMenu(); }
        else if (ch == '4') { testNfc(); testOpticalSampler(); testRelay(); printMenu(); }
    }
}
