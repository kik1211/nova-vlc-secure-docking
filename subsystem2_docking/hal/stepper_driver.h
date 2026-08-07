/**
 * @file    stepper_driver.h
 * @brief   28BYJ-48 stepper motor HAL via ULN2003 driver — half-step mode.
 * @author  Kiruthik R S
 * @date    2026-08-07
 * @version 1.0.0
 *
 * Provides non-blocking single-step commands and position tracking
 * for the 28BYJ-48 unipolar stepper motor driven through a ULN2003A.
 *
 * @note    Half-step mode (8 phases) is used for 0.088°/step resolution.
 * @note    The step() function is non-blocking — it asserts one coil pattern
 *          and returns. Timing between steps is managed by the caller.
 */

#ifndef NOVA_STEPPER_DRIVER_H
#define NOVA_STEPPER_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

/// @brief  Stepper rotation direction.
typedef enum {
    STEPPER_DIR_CW  = 0, ///< Clockwise (viewed from motor shaft end)
    STEPPER_DIR_CCW = 1  ///< Counter-clockwise
} StepperDir_t;

/* === Public API === */

/**
 * @brief   Initialize stepper GPIO pins and set coils to idle.
 */
void stepper_init(void);

/**
 * @brief   Advance the motor by one half-step in the given direction.
 * @param   dir  STEPPER_DIR_CW or STEPPER_DIR_CCW.
 * @note    Non-blocking. The caller is responsible for step timing
 *          (i.e., delaying STEPPER_STEP_INTERVAL_US between calls).
 */
void stepper_step(StepperDir_t dir);

/**
 * @brief   De-energize all stepper coils (save power, reduce heat).
 * @note    Motor will not hold position after this call.
 */
void stepper_release(void);

/**
 * @brief   Get the current cumulative half-step position.
 * @return  Total half-steps from initial position (wraps modulo STEPS_PER_REV).
 */
int32_t stepper_getPosition(void);

/**
 * @brief   Reset the position counter to zero without moving the motor.
 */
void stepper_resetPosition(void);

#endif /* NOVA_STEPPER_DRIVER_H */
