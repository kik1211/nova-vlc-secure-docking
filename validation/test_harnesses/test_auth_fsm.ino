/**
 * @file    test_auth_fsm.ino
 * @brief   Validation Test Harness — 2FA Authentication FSM & Gate Logic.
 * @author  Kiruthik R S
 * @date    2026-08-07
 * 
 * Interactive serial test harness for Subsystem 1 FSM:
 * 1. Monitored FSM tick execution
 * 2. Status reporting via auth_getStatus()
 * 3. Step-by-step state transition logging
 */

#include "../subsystem1_secure_access/config.h"
#include "../subsystem1_secure_access/auth/auth_state_machine.h"

const char* stateToString(AuthState_t state) {
    switch (state) {
        case AUTH_STATE_IDLE:            return "IDLE";
        case AUTH_STATE_NFC_SCAN:        return "NFC_SCAN";
        case AUTH_STATE_OPTICAL_PREP:    return "OPTICAL_PREP";
        case AUTH_STATE_OPTICAL_CAPTURE: return "OPTICAL_CAPTURE";
        case AUTH_STATE_INFERENCE:       return "INFERENCE";
        case AUTH_STATE_TWO_FA_GATE:     return "TWO_FA_GATE";
        case AUTH_STATE_UNLOCK:          return "UNLOCK";
        case AUTH_STATE_DENY:            return "DENY";
        default:                         return "UNKNOWN";
    }
}

const char* reasonToString(AuthReason_t reason) {
    switch (reason) {
        case AUTH_REASON_NONE:            return "NONE";
        case AUTH_REASON_SUCCESS:         return "SUCCESS (Dual Pass)";
        case AUTH_REASON_UNKNOWN_CARD:    return "UNKNOWN_CARD";
        case AUTH_REASON_ML_WRONG_LABEL:  return "ML_WRONG_LABEL";
        case AUTH_REASON_ML_LOW_CONFIDENCE: return "ML_LOW_CONFIDENCE";
        case AUTH_REASON_GOERTZEL_MISMATCH: return "GOERTZEL_MISMATCH";
        case AUTH_REASON_GOERTZEL_AMBIGUOUS: return "GOERTZEL_AMBIGUOUS";
        case AUTH_REASON_CAPTURE_FAILED:  return "CAPTURE_FAILED";
        case AUTH_REASON_INFERENCE_FAILED: return "INFERENCE_FAILED";
        default:                          return "UNKNOWN";
    }
}

static AuthState_t s_lastReportedState = AUTH_STATE_COUNT;

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    unsigned long start = millis();
    while (!Serial && (millis() - start < SERIAL_INIT_DELAY_MS));

    Serial.println("\n==========================================");
    Serial.println("   AUTH FSM VALIDATION HARNESS");
    Serial.println("==========================================");

    bool ok = auth_init();
    if (!ok) {
        Serial.println("CRITICAL: auth_init() failed!");
        while (true) delay(100);
    }
}

void loop() {
    auth_tick();

    AuthState_t currState = auth_getState();
    if (currState != s_lastReportedState) {
        s_lastReportedState = currState;
        Serial.print("[FSM TRANSITION] Current State: ");
        Serial.println(stateToString(currState));

        if (currState == AUTH_STATE_UNLOCK || currState == AUTH_STATE_DENY) {
            AuthStatus_t status = auth_getStatus();
            Serial.println("------------------------------------------");
            Serial.print(" Outcome Reason:    "); Serial.println(reasonToString(status.lastReason));
            Serial.print(" ML Winner Label:   "); Serial.println(status.mlLabel ? status.mlLabel : "NULL");
            Serial.print(" ML Confidence:     "); Serial.print(status.mlConfidence * 100.0f, 1); Serial.println("%");
            Serial.print(" Goertzel Freq:     "); Serial.print(status.goertzelFreqHz); Serial.println(" Hz");
            Serial.print(" Inference Latency: "); Serial.print(status.inferenceLatencyUs); Serial.println(" us");
            Serial.println("------------------------------------------");
        }
    }
}
