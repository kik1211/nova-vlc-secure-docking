/**
 * @file    auth_state_machine.h
 * @brief   Two-factor authentication finite state machine.
 * @author  Kiruthik R S
 * @date    2026-08-07
 * @version 1.0.0
 *
 * Implements the complete 2FA authentication flow as a non-blocking FSM:
 *
 *   IDLE → NFC_SCAN → OPTICAL_PREP → OPTICAL_CAPTURE →
 *   INFERENCE → TWO_FA_GATE → UNLOCK / DENY → IDLE
 *
 * The FSM is ticked from loop(). Each tick performs at most one non-blocking
 * operation and returns. No tick ever blocks the CPU for more than a few
 * milliseconds.
 *
 * @note    Design Freeze §8.2. The dual-verdict 2FA gate requires BOTH
 *          the ML classifier AND the Goertzel detector to agree on the
 *          expected frequency before granting access.
 */

#ifndef NOVA_AUTH_STATE_MACHINE_H
#define NOVA_AUTH_STATE_MACHINE_H

#include <stdint.h>
#include <stdbool.h>

/* === Public Type Definitions === */

/// @brief  Authentication FSM states.
typedef enum {
    AUTH_STATE_IDLE = 0,        ///< Waiting for NFC card
    AUTH_STATE_NFC_SCAN,        ///< Card detected — resolving role
    AUTH_STATE_OPTICAL_PREP,    ///< Waiting for user to aim LED
    AUTH_STATE_OPTICAL_CAPTURE, ///< Timer-driven ADC capture in progress
    AUTH_STATE_INFERENCE,       ///< Running ML + Goertzel analysis
    AUTH_STATE_TWO_FA_GATE,     ///< Evaluating dual-verdict gate
    AUTH_STATE_UNLOCK,          ///< Door unlocked — waiting for relock
    AUTH_STATE_DENY,            ///< Access denied — lockout period
    AUTH_STATE_COUNT            ///< Sentinel
} AuthState_t;

/// @brief  Reason code for the last authentication outcome.
typedef enum {
    AUTH_REASON_NONE = 0,            ///< No decision yet
    AUTH_REASON_SUCCESS,             ///< Both ML and Goertzel agree
    AUTH_REASON_UNKNOWN_CARD,        ///< NFC UID not in credential store
    AUTH_REASON_ML_WRONG_LABEL,      ///< ML label does not match expected
    AUTH_REASON_ML_LOW_CONFIDENCE,   ///< ML confidence below threshold
    AUTH_REASON_GOERTZEL_MISMATCH,   ///< Goertzel dominant != expected freq
    AUTH_REASON_GOERTZEL_AMBIGUOUS,  ///< Goertzel could not find dominant
    AUTH_REASON_CAPTURE_FAILED,      ///< Optical capture did not complete
    AUTH_REASON_INFERENCE_FAILED,    ///< ML inference returned error
    AUTH_REASON_COUNT                ///< Sentinel
} AuthReason_t;

/// @brief  Snapshot of last authentication attempt for logging/diagnostics.
typedef struct {
    AuthState_t  currentState;
    AuthReason_t lastReason;
    uint8_t      lastRole;            ///< NfcRole_t of last scanned card
    const char*  mlLabel;             ///< ML classification label
    float        mlConfidence;        ///< ML classification confidence
    uint8_t      goertzelFreqHz;      ///< Goertzel dominant frequency
    bool         goertzelFound;       ///< Whether Goertzel found a dominant
    uint32_t     inferenceLatencyUs;  ///< ML inference time
} AuthStatus_t;

/* === Public API === */

/**
 * @brief   Initialize the authentication FSM and all dependent modules.
 * @return  true if all hardware modules initialized successfully.
 * @return  false if any critical module (NFC, sampler) failed to init.
 */
bool auth_init(void);

/**
 * @brief   Non-blocking FSM tick — call once per loop() iteration.
 *
 * Each call advances the FSM by at most one state transition.
 * Never blocks for more than a few milliseconds.
 */
void auth_tick(void);

/**
 * @brief   Get a read-only snapshot of the current authentication status.
 * @return  AuthStatus_t with current state, last outcome, and diagnostics.
 */
AuthStatus_t auth_getStatus(void);

/**
 * @brief   Get the current FSM state.
 * @return  AuthState_t enum value.
 */
AuthState_t auth_getState(void);

#endif /* NOVA_AUTH_STATE_MACHINE_H */
