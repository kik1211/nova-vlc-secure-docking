/**
 * @file    test_docking_stepper.ino
 * @brief   Validation Test Harness — Subsystem 2 Autonomous Optical Docking.
 * @author  Kiruthik R S
 * @date    2026-08-07
 * 
 * Interactive serial test harness for Subsystem 2:
 * 1. 28BYJ-48 Stepper Step Accuracy & Backlash Test
 * 2. Photodiode ADC Raw & Averaged Read Test
 * 3. Complete Two-Phase Alignment Scan Execution (Coarse + Fine)
 */

#include "../subsystem2_docking/config.h"
#include "../subsystem2_docking/hal/stepper_driver.h"
#include "../subsystem2_docking/hal/photodiode_adc.h"
#include "../subsystem2_docking/docking/alignment_controller.h"

void printMenu() {
    Serial.println("\n==========================================");
    Serial.println("   SS2 DOCKING VALIDATION TEST HARNESS");
    Serial.println("==========================================");
    Serial.println(" 1 - Test Stepper Rotation (Full Rev CW + CCW)");
    Serial.println(" 2 - Test Photodiode ADC Intensity Read");
    Serial.println(" 3 - Run Two-Phase Optical Alignment Scan");
    Serial.println(" Enter option (1-3):");
}

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    unsigned long start = millis();
    while (!Serial && (millis() - start < SERIAL_INIT_DELAY_MS));

    Serial.println("\n[TEST_DOCKING] Initializing Docking System...");
    alignment_init();
    printMenu();
}

void testStepper() {
    Serial.println("\n--- [TEST 1] Stepper Rotation (4096 Half-Steps CW) ---");
    stepper_resetPosition();

    uint32_t startMs = millis();
    for (int i = 0; i < STEPPER_STEPS_PER_REV; i++) {
        stepper_step(STEPPER_DIR_CW);
        delayMicroseconds(STEPPER_STEP_INTERVAL_US);
    }
    uint32_t cwDuration = millis() - startMs;
    Serial.print("4096 Steps CW Complete in: "); Serial.print(cwDuration); Serial.println(" ms");
    Serial.print("Recorded Position: "); Serial.println(stepper_getPosition());

    Serial.println("Reversing 4096 Half-Steps CCW...");
    startMs = millis();
    for (int i = 0; i < STEPPER_STEPS_PER_REV; i++) {
        stepper_step(STEPPER_DIR_CCW);
        delayMicroseconds(STEPPER_STEP_INTERVAL_US);
    }
    uint32_t ccwDuration = millis() - startMs;
    Serial.print("4096 Steps CCW Complete in: "); Serial.print(ccwDuration); Serial.println(" ms");
    Serial.print("Final Net Position: "); Serial.println(stepper_getPosition());
    Serial.print("Position Accuracy Check: ");
    Serial.println(stepper_getPosition() == 0 ? "PASS (Zero Drift)" : "FAIL (Drift Detected)");

    stepper_release();
}

void testPhotodiode() {
    Serial.println("\n--- [TEST 2] Photodiode ADC Reader ---");
    Serial.println("Reading ambient optical intensity (10 samples)...");
    for (int i = 0; i < 10; i++) {
        int raw = photodiode_readRaw();
        int avg = photodiode_readAveraged(16);
        Serial.print(" Sample ["); Serial.print(i); Serial.print("] Raw: ");
        Serial.print(raw); Serial.print(" | 16x Avg: "); Serial.println(avg);
        delay(200);
    }
}

void testFullScan() {
    Serial.println("\n--- [TEST 3] Full Two-Phase Alignment Scan ---");
    AlignResult_t res = alignment_runFullScan();

    Serial.println("\n------------------------------------------");
    Serial.println(" Alignment Scan Results:");
    Serial.print("  Status:        "); Serial.println(res.status == ALIGN_STATUS_ALIGNED ? "ALIGNED" : "FAILED");
    Serial.print("  Coarse Peak:   Index "); Serial.print(res.coarsePeakIndex);
    Serial.print(" (ADC "); Serial.print(res.coarsePeakValue); Serial.println(")");
    Serial.print("  Fine Peak:     ADC "); Serial.println(res.finePeakValue);
    Serial.print("  Final Pos:     "); Serial.print(res.finalPosition); Serial.println(" half-steps");
    Serial.print("  Final Angle:   "); Serial.print(res.finalAngleDeg, 2); Serial.println(" deg");
    Serial.println("------------------------------------------");
}

void loop() {
    if (Serial.available() > 0) {
        char ch = Serial.read();
        if (ch == '1') { testStepper(); printMenu(); }
        else if (ch == '2') { testPhotodiode(); printMenu(); }
        else if (ch == '3') { testFullScan(); printMenu(); }
    }
}
