/**
 * @file    auth_state_machine.cpp
 * @brief   Two-factor authentication FSM — implementation.
 * @author  Kiruthik R S
 * @date    2026-08-07
 * @version 1.0.0
 *
 * State flow: IDLE → NFC_SCAN → OPTICAL_PREP → OPTICAL_CAPTURE →
 *             INFERENCE → TWO_FA_GATE → UNLOCK/DENY → IDLE
 */

#include "auth_state_machine.h"

#include <Arduino.h>
#include <string.h>

#include "../config.h"
#include "../hal/nfc_reader.h"
#include "../hal/relay_lock.h"
#include "../hal/optical_sampler.h"
#include "../dsp/goertzel.h"
#include "../ml/classifier_wrapper.h"

/* === Private Module State === */
static AuthState_t  s_state = AUTH_STATE_IDLE;
static AuthReason_t s_lastReason = AUTH_REASON_NONE;
static uint32_t     s_stateEntryMs = 0;
static NfcRole_t    s_currentRole = NFC_ROLE_UNKNOWN;
static const char*  s_expectedLabel = NULL;
static float        s_expectedFreqHz = 0.0f;
static const char*  s_lastMlLabel = NULL;
static float        s_lastMlConfidence = 0.0f;
static uint8_t      s_lastGoertzelFreq = 0;
static bool         s_lastGoertzelFound = false;
static int32_t      s_lastInferenceUs = 0;

/* === Private Module Helper Functions (AUD-011) === */

/**
 * @brief   Updates the internal state and records state entry timestamp.
 * @param   newState Target state to transition into.
 */
static void _setState(AuthState_t newState) {
    s_state = newState;
    s_stateEntryMs = millis();
}

/**
 * @brief   Calculates elapsed duration in current state (ms).
 */
static uint32_t _timeInState(void) { return millis() - s_stateEntryMs; }

static void _logUid(const NfcCard_t* card) {
    Serial.print("[AUTH] UID:");
    for (uint8_t i = 0; i < card->uid_length; i++) {
        Serial.print(" 0x");
        if (card->uid[i] <= 0x0F) { Serial.print("0"); }
        Serial.print(card->uid[i], HEX);
    }
    Serial.println();
}

static const char* _roleToStr(NfcRole_t role) {
    switch (role) {
        case NFC_ROLE_ADMIN: return "ADMIN";
        case NFC_ROLE_STAFF: return "STAFF";
        case NFC_ROLE_GUEST: return "GUEST";
        default:             return "UNKNOWN";
    }
}

/* === State Handlers === */

static void _handleIdle(void) {
    NfcCard_t card;
    if (nfc_pollCard(&card)) {
        _logUid(&card);
        s_currentRole = nfc_resolveRole(&card);
        _setState(AUTH_STATE_NFC_SCAN);
    }
}

static void _handleNfcScan(void) {
    if (s_currentRole == NFC_ROLE_UNKNOWN) {
        Serial.println("[AUTH] DENIED: Unregistered card.");
        s_lastReason = AUTH_REASON_UNKNOWN_CARD;
        _setState(AUTH_STATE_DENY);
        return;
    }
    s_expectedLabel = nfc_getExpectedLabel(s_currentRole);
    s_expectedFreqHz = nfc_getExpectedFrequency(s_currentRole);

    Serial.print("[AUTH] "); Serial.print(_roleToStr(s_currentRole));
    Serial.print(" authorized. Required key: ");
    Serial.println(s_expectedLabel);
    Serial.println("[AUTH] Aim LED now. Capturing in 1.5s...");
    _setState(AUTH_STATE_OPTICAL_PREP);
}

static void _handleOpticalPrep(void) {
    if (_timeInState() >= OPTICAL_PREP_DELAY_MS) {
        if (sampler_startCapture()) {
            Serial.println("[AUTH] Capturing optical signal...");
            _setState(AUTH_STATE_OPTICAL_CAPTURE);
        } else {
            Serial.println("[AUTH] ERROR: Capture failed to start.");
            s_lastReason = AUTH_REASON_CAPTURE_FAILED;
            _setState(AUTH_STATE_DENY);
        }
    }
}

static void _handleOpticalCapture(void) {
    if (sampler_isComplete()) {
        Serial.print("[AUTH] Capture done. Elapsed: ");
        Serial.print(sampler_getElapsedUs());
        Serial.println(" us");
        if (sampler_wasSaturated()) {
            Serial.println("[AUTH] WARNING: ADC saturation detected.");
        }
        _setState(AUTH_STATE_INFERENCE);
    }
}

static void _handleInference(void) {
    const float* buffer = sampler_getBuffer();

    // --- ML Classification ---
    ClassifierResult_t mlResult;
    bool mlOk = classifier_run(buffer, SAMPLE_COUNT, &mlResult);
    if (!mlOk || !mlResult.valid) {
        Serial.println("[AUTH] ERROR: ML inference failed.");
        s_lastReason = AUTH_REASON_INFERENCE_FAILED;
        s_lastMlLabel = NULL;
        s_lastMlConfidence = 0.0f;
        _setState(AUTH_STATE_DENY);
        return;
    }
    s_lastMlLabel = mlResult.label;
    s_lastMlConfidence = mlResult.confidence;
    s_lastInferenceUs = mlResult.latency_us;

    Serial.print("[AUTH] ML: ");
    Serial.print(mlResult.label);
    Serial.print(" ("); Serial.print(mlResult.confidence * 100.0f, 1);
    Serial.print("%) ["); Serial.print(mlResult.latency_us);
    Serial.println(" us]");

    // --- Goertzel Analysis ---
    GoertzelResult_t gResult = goertzel_analyze(
        buffer, SAMPLE_COUNT, SAMPLING_FREQUENCY_HZ
    );
    s_lastGoertzelFreq = gResult.dominant_frequency_hz;
    s_lastGoertzelFound = gResult.dominant_found;

    Serial.print("[AUTH] Goertzel: ");
    if (gResult.dominant_found) {
        Serial.print(gResult.dominant_frequency_hz); Serial.println(" Hz");
    } else {
        Serial.println("AMBIGUOUS");
    }
    Serial.print("[AUTH] Magnitudes — 10Hz: ");
    Serial.print(gResult.magnitude_10hz, 1);
    Serial.print(" | 20Hz: "); Serial.print(gResult.magnitude_20hz, 1);
    Serial.print(" | 30Hz: "); Serial.println(gResult.magnitude_30hz, 1);

    _setState(AUTH_STATE_TWO_FA_GATE);
}

static void _handleTwoFaGate(void) {
    bool mlLabelMatch = false;
    if (s_lastMlLabel && s_expectedLabel) {
        mlLabelMatch = (strcmp(s_lastMlLabel, s_expectedLabel) == 0);
    }
    bool mlConfPass = (s_lastMlConfidence >= ML_CONFIDENCE_THRESHOLD);
    uint8_t expectedFreqInt = (uint8_t)s_expectedFreqHz;
    bool goertzelMatch = s_lastGoertzelFound
        && (s_lastGoertzelFreq == expectedFreqInt);

    // Dual-verdict gate: BOTH must agree (Design Freeze §5.3)
    bool gatePass = mlLabelMatch && mlConfPass && goertzelMatch;

    Serial.print("[2FA] ML_label:"); Serial.print(mlLabelMatch?"PASS":"FAIL");
    Serial.print(" ML_conf:"); Serial.print(mlConfPass ? "PASS" : "FAIL");
    Serial.print(" Goertzel:"); Serial.println(goertzelMatch?"PASS":"FAIL");
    Serial.print("[2FA] GATE: "); Serial.println(gatePass?"PASS":"FAIL");

    if (gatePass) {
        s_lastReason = AUTH_REASON_SUCCESS;
        Serial.println("[AUTH] >>> 2FA SUCCESS: UNLOCKING <<<");
        relay_unlock();
        _setState(AUTH_STATE_UNLOCK);
    } else {
        if (!mlLabelMatch)            { s_lastReason = AUTH_REASON_ML_WRONG_LABEL; }
        else if (!mlConfPass)         { s_lastReason = AUTH_REASON_ML_LOW_CONFIDENCE; }
        else if (!s_lastGoertzelFound){ s_lastReason = AUTH_REASON_GOERTZEL_AMBIGUOUS; }
        else                          { s_lastReason = AUTH_REASON_GOERTZEL_MISMATCH; }
        Serial.println("[AUTH] >>> 2FA FAILED <<<");
        _setState(AUTH_STATE_DENY);
    }
}

static void _handleUnlock(void) {
    relay_tick();
    if (!relay_isUnlocked()) {
        Serial.println("[AUTH] Door re-locked. IDLE.");
        _setState(AUTH_STATE_IDLE);
    }
}

static void _handleDeny(void) {
    if (_timeInState() >= NFC_DENY_LOCKOUT_MS) {
        Serial.println("[AUTH] Lockout expired. Waiting...");
        _setState(AUTH_STATE_IDLE);
    }
}

/* === Public API Implementation === */

bool auth_init(void) {
    Serial.begin(SERIAL_BAUD_RATE);
    unsigned long waitStart = millis();
    while (!Serial && (millis() - waitStart < SERIAL_INIT_DELAY_MS)) {}

    Serial.println("\n=========================================");
    Serial.println("   N.O.V.A. SECURE AI LOCK — v1.0.0");
    Serial.println("   Dual-Verdict 2FA (ML + Goertzel)");
    Serial.println("=========================================");

    bool nfcOk = nfc_init();
    if (!nfcOk) {
        Serial.println("[AUTH] CRITICAL: NFC init failed.");
        return false;
    }
    relay_init();

    bool samplerOk = sampler_init();
    if (!samplerOk) {
        Serial.println("[AUTH] CRITICAL: Sampler init failed.");
        return false;
    }
    classifier_init();

    Serial.println("[AUTH] All modules initialized. System ARMED.");
    Serial.println("Waiting for NFC card...\n");
    _setState(AUTH_STATE_IDLE);
    return true;
}

void auth_tick(void) {
    switch (s_state) {
        case AUTH_STATE_IDLE:            _handleIdle();           break;
        case AUTH_STATE_NFC_SCAN:        _handleNfcScan();        break;
        case AUTH_STATE_OPTICAL_PREP:    _handleOpticalPrep();    break;
        case AUTH_STATE_OPTICAL_CAPTURE: _handleOpticalCapture(); break;
        case AUTH_STATE_INFERENCE:       _handleInference();      break;
        case AUTH_STATE_TWO_FA_GATE:     _handleTwoFaGate();      break;
        case AUTH_STATE_UNLOCK:          _handleUnlock();         break;
        case AUTH_STATE_DENY:            _handleDeny();           break;
        default:                         _setState(AUTH_STATE_IDLE); break;
    }
}

AuthStatus_t auth_getStatus(void) {
    AuthStatus_t status;
    status.currentState = s_state;
    status.lastReason = s_lastReason;
    status.lastRole = (uint8_t)s_currentRole;
    status.mlLabel = s_lastMlLabel;
    status.mlConfidence = s_lastMlConfidence;
    status.goertzelFreqHz = s_lastGoertzelFreq;
    status.goertzelFound = s_lastGoertzelFound;
    status.inferenceLatencyUs = s_lastInferenceUs;
    return status;
}

AuthState_t auth_getState(void) { return s_state; }
