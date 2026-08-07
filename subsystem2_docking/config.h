/**
 * @file    config.h
 * @brief   Subsystem 2 (Docking) — all configurable constants.
 * @author  Kiruthik R S
 * @date    2026-08-07
 * @version 1.0.0
 *
 * Every pin definition, timing constant, threshold, and tunable parameter
 * for the autonomous optical docking subsystem is defined here.
 * No magic numbers shall exist outside this file.
 *
 * @note    Design Freeze Specification v1.0 is the source of truth for all
 *          values in this file. Any change requires an ADR.
 */

#ifndef NOVA_SS2_CONFIG_H
#define NOVA_SS2_CONFIG_H

#include <stdint.h>

// ============================================================================
// === Hardware Pins (ESP32) ==================================================
// ============================================================================
// NOTE: Stepper and photodiode pins are pending confirmation.
//       Design Freeze Open Questions 5 & 6 remain unanswered.
//       These pins MUST be updated to match actual wiring before flashing.

/// @brief  ULN2003 input pin 1 (stepper coil A).
/// @note   ESP32 DevKit-V1 GPIO 13 (Design Freeze §7.2 pinout).
constexpr int STEPPER_PIN_IN1 = 13;

/// @brief  ULN2003 input pin 2 (stepper coil B).
/// @note   ESP32 DevKit-V1 GPIO 12 (Design Freeze §7.2 pinout).
constexpr int STEPPER_PIN_IN2 = 12;

/// @brief  ULN2003 input pin 3 (stepper coil C).
/// @note   ESP32 DevKit-V1 GPIO 14 (Design Freeze §7.2 pinout).
constexpr int STEPPER_PIN_IN3 = 14;

/// @brief  ULN2003 input pin 4 (stepper coil D).
/// @note   ESP32 DevKit-V1 GPIO 27 (Design Freeze §7.2 pinout).
constexpr int STEPPER_PIN_IN4 = 27;

/// @brief  ADC pin for docking photodiode sensor.
/// @note   ESP32 DevKit-V1 GPIO 34 (ADC1_CH6, input-only pin).
constexpr int DOCKING_SENSOR_PIN = 34;

// ============================================================================
// === Stepper Motor Configuration (28BYJ-48) =================================
// ============================================================================

/// @brief  Number of half-steps per full revolution (28BYJ-48 with 1:64 gear).
/// @note   Design Freeze §7.2: 4096 half-steps per revolution.
constexpr int STEPPER_STEPS_PER_REV = 4096;

/// @brief  Angular resolution per half-step in degrees.
/// @note   360.0 / 4096 = 0.087890625 degrees.
constexpr float STEPPER_DEGREES_PER_STEP = 360.0f / STEPPER_STEPS_PER_REV;

/// @brief  Maximum step rate in steps per second.
/// @note   Design Freeze §7.2: 500 steps/s (conservative; avoids missed steps).
constexpr int STEPPER_MAX_STEP_RATE = 500;

/// @brief  Step interval in microseconds (1,000,000 / STEPPER_MAX_STEP_RATE).
constexpr uint32_t STEPPER_STEP_INTERVAL_US = 1000000UL / STEPPER_MAX_STEP_RATE;

/// @brief  Number of half-step phases in the drive sequence.
/// @note   Half-step mode uses an 8-step repeating sequence.
constexpr int STEPPER_PHASE_COUNT = 8;

/// @brief  Backlash compensation overshoot in half-steps.
/// @note   Design Freeze §7.5: 16 half-steps ≈ 1.4°. Must be empirically tuned.
constexpr int BACKLASH_COMPENSATION_STEPS = 16;

// ============================================================================
// === Docking Scan Configuration =============================================
// ============================================================================

/// @brief  Number of coarse scan measurement points across 360°.
/// @note   Design Freeze §7.4: 128 points, each 32 half-steps apart (2.8° spacing).
constexpr int COARSE_SCAN_POINTS = 128;

/// @brief  Number of half-steps between coarse scan measurement points.
constexpr int COARSE_STEPS_PER_POINT = STEPPER_STEPS_PER_REV / COARSE_SCAN_POINTS;

/// @brief  Number of ADC samples averaged at each coarse scan position.
/// @note   Design Freeze §7.4: 16 samples for noise rejection.
constexpr int COARSE_ADC_OVERSAMPLE = 16;

/// @brief  Number of coarse steps on each side of the coarse peak for fine sweep.
/// @note   Design Freeze §7.4: ±8 coarse steps = ±22.4°.
constexpr int FINE_SCAN_COARSE_HALF_WIDTH = 8;

/// @brief  Total fine scan measurement points.
/// @note   Each side: 8 coarse steps × 32 half-steps = 256 half-steps.
///         Total window: 512 half-steps at 1 half-step resolution.
constexpr int FINE_SCAN_POINTS = FINE_SCAN_COARSE_HALF_WIDTH * 2
                                 * COARSE_STEPS_PER_POINT;

/// @brief  Number of ADC samples averaged at each fine scan position.
/// @note   Design Freeze §7.4: 32 samples for improved SNR.
constexpr int FINE_ADC_OVERSAMPLE = 32;

/// @brief  Minimum ADC intensity threshold for beacon detection.
/// @note   If the maximum intensity during coarse sweep is below this value,
///         the system declares ALIGNMENT_FAILED. Must be calibrated on hardware.
constexpr int DOCKING_MIN_INTENSITY_THRESHOLD = 200;

/// @brief  Settle time (ms) between stepper stop and ADC read.
/// @note   Design Freeze §7.7: 5 ms for electrical transients to decay.
constexpr uint32_t STEPPER_SETTLE_TIME_MS = 5;

// ============================================================================
// === ADC Configuration ======================================================
// ============================================================================

/// @brief  ADC resolution in bits.
constexpr int ADC_RESOLUTION_BITS = 12;

/// @brief  Maximum ADC raw value.
constexpr int ADC_MAX_VALUE = (1 << ADC_RESOLUTION_BITS) - 1;

// ============================================================================
// === Serial / Debug Configuration ===========================================
// ============================================================================

/// @brief  UART baud rate for debug serial output.
constexpr uint32_t SERIAL_BAUD_RATE = 115200;

/// @brief  Delay (ms) after Serial.begin() to allow USB-serial to connect.
constexpr uint32_t SERIAL_INIT_DELAY_MS = 1000;

#endif /* NOVA_SS2_CONFIG_H */
