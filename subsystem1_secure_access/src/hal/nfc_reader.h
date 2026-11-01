/**
 * @file    nfc_reader.h
 * @brief   PN532 NFC reader HAL — initialization, polling, and UID comparison.
 * @author  Kiruthik R S
 * @date    2026-08-07
 * @version 1.0.0
 *
 * Wraps the Adafruit PN532 library to provide a clean, hardware-agnostic API
 * for NFC card detection and role resolution. Hides I2C implementation details.
 *
 * @note    IRQ and RESET pins are not used (I2C polling mode confirmed working).
 * @warning Ensure Wire.begin(SDA_PIN, SCL_PIN) is called before nfc_init().
 */

#ifndef NOVA_NFC_READER_H
#define NOVA_NFC_READER_H

#include <stdint.h>
#include <stdbool.h>

/* === Public Type Definitions === */

/// @brief  Scanned NFC card data container.
typedef struct {
    uint8_t uid[7];       ///< Raw UID bytes (up to 7 for future MIFARE support)
    uint8_t uid_length;   ///< Actual UID length (4 for MIFARE Classic)
    bool    valid;        ///< true if a card was successfully read
} NfcCard_t;

/// @brief  User role resolved from NFC card UID.
typedef enum {
    NFC_ROLE_UNKNOWN = 0, ///< UID does not match any registered card
    NFC_ROLE_ADMIN,       ///< Admin role — expects 30 Hz optical key
    NFC_ROLE_STAFF,       ///< Staff role — expects 20 Hz optical key
    NFC_ROLE_GUEST,       ///< Guest role — expects 10 Hz optical key
    NFC_ROLE_COUNT        ///< Sentinel — number of valid roles
} NfcRole_t;

/* === Public API === */

/**
 * @brief   Initialize the PN532 NFC reader over I2C.
 * @return  true if initialization succeeded and firmware version was read.
 * @return  false if the PN532 was not detected on the I2C bus.
 * @note    Does NOT halt the system on failure. Caller decides recovery.
 */
bool nfc_init(void);

/**
 * @brief   Poll for an NFC card with NFC_POLL_TIMEOUT_MS timeout.
 * @param   card  Pointer to NfcCard_t struct to populate on success.
 * @return  true if a valid 4-byte UID card was detected.
 * @return  false if no card detected within timeout.
 */
bool nfc_pollCard(NfcCard_t* card);

/**
 * @brief   Resolve a scanned card UID to a user role.
 * @param   card  Pointer to a valid NfcCard_t from nfc_pollCard().
 * @return  NfcRole_t enum value; NFC_ROLE_UNKNOWN if UID is not registered.
 */
NfcRole_t nfc_resolveRole(const NfcCard_t* card);

/**
 * @brief   Return the ML classification label expected for a given role.
 * @param   role  A valid NfcRole_t value (not NFC_ROLE_UNKNOWN or COUNT).
 * @return  Pointer to a static string literal (e.g., "freq_30hz").
 *          Returns NULL for NFC_ROLE_UNKNOWN or NFC_ROLE_COUNT.
 */
const char* nfc_getExpectedLabel(NfcRole_t role);

/**
 * @brief   Return the expected optical frequency (Hz) for a given role.
 * @param   role  A valid NfcRole_t value.
 * @return  Frequency in Hz (e.g., 30.0f for ADMIN). 0.0f for unknown.
 */
float nfc_getExpectedFrequency(NfcRole_t role);

#endif /* NOVA_NFC_READER_H */
