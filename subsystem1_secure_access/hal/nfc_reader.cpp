/**
 * @file    nfc_reader.cpp
 * @brief   PN532 NFC reader HAL — implementation.
 * @author  Kiruthik R S
 * @date    2026-08-07
 * @version 1.0.0
 */

#include "nfc_reader.h"

#include <Arduino.h>
#include <Wire.h>

#include <Adafruit_PN532.h>

#include "../config.h"

/* === Private Module State === */
static Adafruit_PN532 s_nfc(-1, -1); // I2C mode; IRQ/RESET not used

/* === Public API Implementation === */

bool nfc_init(void) {
    Wire.begin(SDA_PIN, SCL_PIN);
    s_nfc.begin();

    uint32_t versiondata = s_nfc.getFirmwareVersion();
    if (!versiondata) {
        Serial.println("[HAL][NFC] ERROR: PN532 not detected on I2C bus.");
        return false;
    }

    s_nfc.SAMConfig();
    Serial.println("[HAL][NFC] PN532 initialized successfully.");
    return true;
}

bool nfc_pollCard(NfcCard_t* card) {
    if (!card) {
        return false;
    }
    card->valid = false;
    card->uid_length = 0;

    uint8_t uid[7] = {0};
    uint8_t uidLen = 0;
    uint8_t success = s_nfc.readPassiveTargetID(
        PN532_MIFARE_ISO14443A, uid, &uidLen, NFC_POLL_TIMEOUT_MS
    );

    if (!success || uidLen != NFC_UID_LENGTH) {
        return false;
    }

    for (uint8_t i = 0; i < uidLen; i++) {
        card->uid[i] = uid[i];
    }
    card->uid_length = uidLen;
    card->valid = true;
    return true;
}

NfcRole_t nfc_resolveRole(const NfcCard_t* card) {
    if (!card || !card->valid || card->uid_length != NFC_UID_LENGTH) {
        return NFC_ROLE_UNKNOWN;
    }

    // Compare against registered UIDs
    bool matchAdmin = true;
    bool matchStaff = true;
    bool matchGuest = true;
    for (int i = 0; i < NFC_UID_LENGTH; i++) {
        if (card->uid[i] != ADMIN_UID[i]) { matchAdmin = false; }
        if (card->uid[i] != STAFF_UID[i]) { matchStaff = false; }
        if (card->uid[i] != GUEST_UID[i]) { matchGuest = false; }
    }

    if (matchAdmin) { return NFC_ROLE_ADMIN; }
    if (matchStaff) { return NFC_ROLE_STAFF; }
    if (matchGuest) { return NFC_ROLE_GUEST; }
    return NFC_ROLE_UNKNOWN;
}

const char* nfc_getExpectedLabel(NfcRole_t role) {
    if (role == NFC_ROLE_UNKNOWN || role >= NFC_ROLE_COUNT) {
        return NULL;
    }
    // Role enum: ADMIN=1, STAFF=2, GUEST=3 → index = role - 1
    return ROLE_EXPECTED_LABELS[role - 1];
}

float nfc_getExpectedFrequency(NfcRole_t role) {
    if (role == NFC_ROLE_UNKNOWN || role >= NFC_ROLE_COUNT) {
        return 0.0f;
    }
    return ROLE_FREQUENCIES[role - 1];
}
