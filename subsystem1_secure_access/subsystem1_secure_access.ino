/**
 * @file    subsystem1_secure_access.ino
 * @brief   N.O.V.A. Subsystem 1 — VLC-Based Two-Factor Access Control.
 * @author  Kiruthik R S
 * @date    2026-08-07
 * @version 1.0.0
 *
 * Arduino entry point for the ESP32-S3 secure access subsystem.
 * This file contains only setup() and loop() — all logic is delegated
 * to the authentication state machine FSM.
 *
 * Signal Path:
 *   BPW34 → LM358 (TIA) → RC (48 Hz) → ADC1_CH0 (GPIO 1)
 *   → 100 Hz timer ISR → float buffer[100]
 *   → Edge Impulse 16-pt FFT → 4-class NN → ML verdict
 *   → Goertzel (k=10,20,30) → deterministic verdict
 *   → Dual-verdict 2FA gate → relay → 12V solenoid
 *
 * Hardware:
 *   - ESP32-S3 DevKit
 *   - PN532 NFC Reader (I2C)
 *   - BPW34 Photodiode + LM358 TIA
 *   - 5V Relay Module → 12V Solenoid Lock
 *
 * @see     docs/ARCHITECTURE.md for system overview.
 * @see     auth/auth_state_machine.h for FSM documentation.
 */

#include "config.h"
#include "auth/auth_state_machine.h"

/**
 * @brief   Arduino setup entry point.
 *
 * Initializes all hardware modules via auth_init().
 * If any critical module fails, the system halts with an error message
 * rather than operating in a degraded state.
 */
void setup() {
    bool ok = auth_init();
    if (!ok) {
        // Critical initialization failure — do not proceed.
        // auth_init() has already printed the failure reason.
        while (true) {
            // Halt loop — flash built-in LED to indicate fault.
            // (ESP32-S3 DevKit LED is typically on GPIO 48 or 2.)
            delay(500);
        }
    }
}

/**
 * @brief   Arduino loop entry point.
 *
 * Calls the FSM tick function once per iteration.
 * Each tick advances the state machine by at most one transition.
 * The FSM is entirely non-blocking.
 */
void loop() {
    auth_tick();
}
