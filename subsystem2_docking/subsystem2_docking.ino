/**
 * @file    subsystem2_docking.ino
 * @brief   N.O.V.A. Subsystem 2 — Autonomous Optical Docking.
 * @author  Kiruthik R S
 * @date    2026-08-07
 * @version 1.0.0
 *
 * Arduino entry point for the ESP32 docking subsystem.
 * On power-up, the system performs a single full alignment scan
 * and reports the result over UART.
 *
 * Hardware:
 *   - ESP32 DevKit
 *   - 28BYJ-48 Stepper Motor + ULN2003 Driver
 *   - BPW34 Photodiode
 *
 * @note    The current design runs a single scan on boot.
 *          Future versions may accept UART commands to trigger re-scans.
 *
 * @see     docs/DOCKING.md for algorithm documentation.
 * @see     docking/alignment_controller.h for scan API.
 */

#include "config.h"
#include "docking/alignment_controller.h"

/**
 * @brief   Arduino setup entry point.
 *
 * Initializes serial, alignment hardware, and runs a full scan.
 */
void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    unsigned long waitStart = millis();
    while (!Serial && (millis() - waitStart < SERIAL_INIT_DELAY_MS)) {
        // Wait for USB-serial
    }

    Serial.println();
    Serial.println("==========================================");
    Serial.println("   N.O.V.A. DOCKING SUBSYSTEM — v1.0.0");
    Serial.println("   Two-Phase Optical Alignment");
    Serial.println("==========================================");

    alignment_init();

    Serial.println("[MAIN] Starting alignment scan in 2 seconds...");
    delay(2000); // Allow user to observe startup message

    AlignResult_t result = alignment_runFullScan();

    Serial.println();
    Serial.println("==========================================");
    Serial.println("   SCAN COMPLETE — SUMMARY");
    Serial.println("==========================================");

    if (result.status == ALIGN_STATUS_ALIGNED) {
        Serial.print("  Status:       ALIGNED\n");
        Serial.print("  Final angle:  ");
        Serial.print(result.finalAngleDeg, 2);
        Serial.println(" degrees");
        Serial.print("  Coarse peak:  index ");
        Serial.print(result.coarsePeakIndex);
        Serial.print(" (ADC: ");
        Serial.print(result.coarsePeakValue);
        Serial.println(")");
        Serial.print("  Fine peak:    ADC ");
        Serial.println(result.finePeakValue);
    } else {
        Serial.println("  Status:       FAILED");
        Serial.println("  No beacon detected above threshold.");
    }

    Serial.println("==========================================");
    Serial.println("[MAIN] Scan complete. Motor released.");
}

/**
 * @brief   Arduino loop entry point.
 *
 * After the initial scan, the system idles. Future versions may
 * listen for UART commands to trigger re-scans.
 */
void loop() {
    // Idle — scan completes in setup().
    // Future: listen for UART re-scan commands.
}
