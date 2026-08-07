/**
 * @file    config.h
 * @brief   Subsystem 1 (Secure Access) — all configurable constants.
 * @author  Kiruthik R S
 * @date    2026-08-07
 * @version 1.0.0
 *
 * Every pin definition, timing constant, threshold, and tunable parameter
 * for the VLC-based two-factor access control subsystem is defined here.
 * No magic numbers shall exist outside this file.
 *
 * @note    Design Freeze Specification v1.0 is the source of truth for all
 *          values in this file. Any change requires an ADR.
 */

#ifndef NOVA_SS1_CONFIG_H
#define NOVA_SS1_CONFIG_H

#include <stdint.h>

// ============================================================================
// === Hardware Pins (ESP32-S3) ===============================================
// ============================================================================

/// @brief  ADC pin connected to LM358 TIA output (via RC anti-aliasing filter).
/// @note   Must be on ADC1 (unaffected by Wi-Fi). ADC1_CH0 on ESP32-S3.
constexpr int SENSOR_PIN = 1;

/// @brief  GPIO pin driving the relay module IN signal.
/// @note   Active HIGH asserts relay; relay NO contact closes the solenoid circuit.
constexpr int RELAY_PIN = 4;

/// @brief  I2C SDA pin for PN532 NFC reader.
/// @note   Custom I2C pins — Wire.begin(SDA_PIN, SCL_PIN) required.
constexpr int SDA_PIN = 8;

/// @brief  I2C SCL pin for PN532 NFC reader.
constexpr int SCL_PIN = 9;

// ============================================================================
// === ADC Configuration ======================================================
// ============================================================================

/// @brief  ADC resolution in bits. ESP32-S3 SAR ADC supports up to 12-bit.
constexpr int ADC_RESOLUTION_BITS = 12;

/// @brief  Maximum ADC raw value at full resolution.
constexpr int ADC_MAX_VALUE = (1 << ADC_RESOLUTION_BITS) - 1; // 4095

/// @brief  ADC saturation detection threshold (raw counts).
/// @note   If any sample exceeds this, the optical signal may be clipping.
///         Set to ~95% of ADC_MAX_VALUE.
constexpr int ADC_SATURATION_THRESHOLD = 3890;

// ============================================================================
// === Sampling Configuration =================================================
// ============================================================================

/// @brief  Sampling frequency in Hz for optical signal capture.
/// @note   Design Freeze §5.1: 100 Hz. Nyquist margin above 30 Hz.
constexpr float SAMPLING_FREQUENCY_HZ = 100.0f;

/// @brief  Number of ADC samples per optical capture window.
/// @note   Design Freeze §5.1: 100 samples = 1.000 second window.
///         Must match EI_CLASSIFIER_RAW_SAMPLE_COUNT in the EI library.
constexpr int SAMPLE_COUNT = 100;

/// @brief  Sampling interval in microseconds (1 / SAMPLING_FREQUENCY_HZ).
/// @note   10,000 µs = 10 ms = 100 Hz. Used by esp_timer ISR.
constexpr uint32_t SAMPLE_INTERVAL_US = 10000;

// ============================================================================
// === Goertzel DSP Configuration =============================================
// ============================================================================

/// @brief  Number of discrete frequencies to detect with Goertzel.
constexpr int GOERTZEL_NUM_FREQUENCIES = 3;

/// @brief  Target frequencies for Goertzel analysis (Hz).
/// @note   Design Freeze §5.3: At N=100 and fs=100 Hz, these map to exact
///         DFT bins k=10, k=20, k=30 — zero spectral leakage.
constexpr float GOERTZEL_TARGET_FREQ_1 = 10.0f;
constexpr float GOERTZEL_TARGET_FREQ_2 = 20.0f;
constexpr float GOERTZEL_TARGET_FREQ_3 = 30.0f;

/// @brief  Minimum Goertzel magnitude ratio (dominant / second-highest) to
///         consider a frequency detection valid.
/// @note   If the dominant frequency's magnitude is not at least this ratio
///         above the next-highest, the result is classified as ambiguous.
///         Must be calibrated on hardware. Initial value is conservative.
constexpr float GOERTZEL_MIN_DOMINANCE_RATIO = 2.0f;

// ============================================================================
// === ML / Edge Impulse Configuration ========================================
// ============================================================================

/// @brief  Minimum classifier confidence (0.0–1.0) required for the ML
///         verdict to pass the 2FA gate.
/// @note   Design Freeze §6.4: 0.85 (85%). Must be empirically calibrated.
constexpr float ML_CONFIDENCE_THRESHOLD = 0.85f;

/// @brief  Classification label strings for frequency-to-role mapping.
/// @note   Must exactly match the Edge Impulse trained label strings.
constexpr const char* ML_LABEL_10HZ = "freq_10hz";
constexpr const char* ML_LABEL_20HZ = "freq_20hz";
constexpr const char* ML_LABEL_30HZ = "freq_30hz";
constexpr const char* ML_LABEL_NOISE = "noise";

// ============================================================================
// === Security / Authentication Configuration ================================
// ============================================================================

/// @brief  Maximum number of registered NFC roles.
constexpr int MAX_NFC_ROLES = 3;

/// @brief  NFC card UID length in bytes. MIFARE Classic uses 4-byte UIDs.
constexpr int NFC_UID_LENGTH = 4;

/// @brief  NFC polling timeout in milliseconds.
/// @note   Design Freeze §1.1 FR-SS1-01: 50 ms maximum per cycle.
constexpr uint16_t NFC_POLL_TIMEOUT_MS = 50;

/// @brief  Delay (ms) after unregistered NFC card before accepting next scan.
/// @note   Design Freeze §1.1 FR-SS1-08: configurable lockout delay.
constexpr uint32_t NFC_DENY_LOCKOUT_MS = 2000;

/// @brief  Preparation window (ms) between NFC success and optical capture.
/// @note   Gives user time to aim the modulated LED at the photodiode.
constexpr uint32_t OPTICAL_PREP_DELAY_MS = 1500;

// ============================================================================
// === Relay / Solenoid Configuration =========================================
// ============================================================================

/// @brief  Duration (ms) the relay remains asserted after successful 2FA.
/// @note   Design Freeze §1.1 FR-SS1-06: configurable hold-open duration.
constexpr uint32_t DOOR_OPEN_DURATION_MS = 5000;

/// @brief  Relay pin logic level to assert (unlock door).
constexpr int RELAY_ACTIVE_LEVEL = HIGH;

/// @brief  Relay pin logic level to deassert (lock door).
constexpr int RELAY_INACTIVE_LEVEL = LOW;

// ============================================================================
// === Serial / Debug Configuration ===========================================
// ============================================================================

/// @brief  UART baud rate for debug serial output.
constexpr uint32_t SERIAL_BAUD_RATE = 115200;

/// @brief  Delay (ms) after Serial.begin() to allow USB-serial to connect.
constexpr uint32_t SERIAL_INIT_DELAY_MS = 1000;

// ============================================================================
// === NFC Credential Store (Prototype Only) ==================================
// ============================================================================
// WARNING: In a production system, credentials MUST be stored in NVS
// (encrypted flash), not in compiled firmware. These plaintext UIDs are
// acceptable ONLY for the prototype demonstration.
//
// Design Freeze §8.4: compile-time constants for prototype.

/// @brief  Admin NFC card UID — expects 30 Hz optical key.
constexpr uint8_t ADMIN_UID[NFC_UID_LENGTH] = {0xAE, 0x9A, 0xF9, 0x06};

/// @brief  Staff NFC card UID — expects 20 Hz optical key.
constexpr uint8_t STAFF_UID[NFC_UID_LENGTH] = {0x03, 0x5F, 0x1A, 0x32};

/// @brief  Guest NFC card UID — expects 10 Hz optical key.
constexpr uint8_t GUEST_UID[NFC_UID_LENGTH] = {0xDE, 0xB0, 0x29, 0x07};

// ============================================================================
// === Role-to-Frequency Mapping ==============================================
// ============================================================================

/// @brief  Optical frequency (Hz) required for each role.
/// @note   Index 0 = ADMIN, Index 1 = STAFF, Index 2 = GUEST.
///         Must match GOERTZEL_TARGET_FREQ values.
constexpr float ROLE_FREQUENCIES[MAX_NFC_ROLES] = {30.0f, 20.0f, 10.0f};

/// @brief  ML classification label expected for each role.
/// @note   Index 0 = ADMIN, Index 1 = STAFF, Index 2 = GUEST.
constexpr const char* ROLE_EXPECTED_LABELS[MAX_NFC_ROLES] = {
    ML_LABEL_30HZ,
    ML_LABEL_20HZ,
    ML_LABEL_10HZ
};

#endif /* NOVA_SS1_CONFIG_H */
