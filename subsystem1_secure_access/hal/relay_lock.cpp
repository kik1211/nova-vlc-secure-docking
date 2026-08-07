/**
 * @file    relay_lock.cpp
 * @brief   Relay/solenoid lock HAL — implementation.
 * @author  Kiruthik R S
 * @date    2026-08-07
 * @version 1.0.0
 */

#include "relay_lock.h"

#include <Arduino.h>

#include "../config.h"

/* === Private Module State === */
static bool     s_isUnlocked = false;
static uint32_t s_unlockStartMs = 0;

/* === Public API Implementation === */

void relay_init(void) {
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, RELAY_INACTIVE_LEVEL);
    s_isUnlocked = false;
    Serial.println("[HAL][RELAY] Relay initialized. Door LOCKED.");
}

void relay_unlock(void) {
    digitalWrite(RELAY_PIN, RELAY_ACTIVE_LEVEL);
    s_isUnlocked = true;
    s_unlockStartMs = millis();
}

void relay_lock(void) {
    digitalWrite(RELAY_PIN, RELAY_INACTIVE_LEVEL);
    s_isUnlocked = false;
}

bool relay_tick(void) {
    if (s_isUnlocked) {
        uint32_t elapsed = millis() - s_unlockStartMs;
        if (elapsed >= DOOR_OPEN_DURATION_MS) {
            relay_lock();
            Serial.println("[HAL][RELAY] Auto-relock: Door LOCKED.");
        }
    }
    return s_isUnlocked;
}

bool relay_isUnlocked(void) {
    return s_isUnlocked;
}
