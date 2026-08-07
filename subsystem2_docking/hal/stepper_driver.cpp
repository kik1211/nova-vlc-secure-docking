/**
 * @file    stepper_driver.cpp
 * @brief   28BYJ-48 stepper motor HAL — implementation.
 * @author  Kiruthik R S
 * @date    2026-08-07
 * @version 1.0.0
 *
 * Half-step drive sequence for the 28BYJ-48 through ULN2003:
 *
 *   Phase | IN1 | IN2 | IN3 | IN4
 *   ------|-----|-----|-----|-----
 *     0   |  1  |  0  |  0  |  0
 *     1   |  1  |  1  |  0  |  0
 *     2   |  0  |  1  |  0  |  0
 *     3   |  0  |  1  |  1  |  0
 *     4   |  0  |  0  |  1  |  0
 *     5   |  0  |  0  |  1  |  1
 *     6   |  0  |  0  |  0  |  1
 *     7   |  1  |  0  |  0  |  1
 */

#include "stepper_driver.h"

#include <Arduino.h>

#include "../config.h"

/* === Private Constants === */

/// @brief  ULN2003 input pins in order: IN1, IN2, IN3, IN4.
static const int s_pins[4] = {
    STEPPER_PIN_IN1, STEPPER_PIN_IN2,
    STEPPER_PIN_IN3, STEPPER_PIN_IN4
};

/// @brief  Half-step phase sequence (8 phases × 4 coils).
static const uint8_t s_sequence[STEPPER_PHASE_COUNT][4] = {
    {1, 0, 0, 0},  // Phase 0
    {1, 1, 0, 0},  // Phase 1
    {0, 1, 0, 0},  // Phase 2
    {0, 1, 1, 0},  // Phase 3
    {0, 0, 1, 0},  // Phase 4
    {0, 0, 1, 1},  // Phase 5
    {0, 0, 0, 1},  // Phase 6
    {1, 0, 0, 1}   // Phase 7
};

/* === Private Module State === */
static int     s_currentPhase = 0;
static int32_t s_position = 0;

/* === Private Helpers === */

static void _applyPhase(int phase) {
    for (int i = 0; i < 4; i++) {
        digitalWrite(s_pins[i], s_sequence[phase][i]);
    }
}

/* === Public API Implementation === */

void stepper_init(void) {
    for (int i = 0; i < 4; i++) {
        pinMode(s_pins[i], OUTPUT);
        digitalWrite(s_pins[i], LOW);
    }
    s_currentPhase = 0;
    s_position = 0;
    Serial.println("[HAL][STEPPER] 28BYJ-48 initialized (half-step).");
}

void stepper_step(StepperDir_t dir) {
    if (dir == STEPPER_DIR_CW) {
        s_currentPhase++;
        s_position++;
    } else {
        s_currentPhase--;
        s_position--;
    }

    // Wrap phase within [0, STEPPER_PHASE_COUNT)
    if (s_currentPhase >= STEPPER_PHASE_COUNT) {
        s_currentPhase = 0;
    } else if (s_currentPhase < 0) {
        s_currentPhase = STEPPER_PHASE_COUNT - 1;
    }

    _applyPhase(s_currentPhase);
}

void stepper_release(void) {
    for (int i = 0; i < 4; i++) {
        digitalWrite(s_pins[i], LOW);
    }
}

int32_t stepper_getPosition(void) {
    return s_position;
}

void stepper_resetPosition(void) {
    s_position = 0;
}
