/**
 * @file    alignment_controller.cpp
 * @brief   Two-phase optical docking alignment controller — implementation.
 * @author  Kiruthik R S
 * @date    2026-08-07
 * @version 1.0.0
 *
 * Algorithm overview:
 *
 *   Phase 1 (Coarse):
 *     - 128 positions spaced 32 half-steps apart (2.8° each)
 *     - 16x oversampled ADC read at each position
 *     - Record index and value of maximum
 *
 *   Phase 2 (Fine):
 *     - Reverse to (coarsePeak - 8) × 32 half-steps from peak
 *     - Sweep forward at 1 half-step resolution
 *     - 32x oversampled ADC read at each position
 *     - Record index and value of maximum
 *
 *   Backlash Compensation:
 *     - Overshoot past final position by BACKLASH_COMPENSATION_STEPS (CCW)
 *     - Return CW to final position (approach from consistent direction)
 */

#include "alignment_controller.h"

#include <Arduino.h>

#include "../config.h"
#include "../hal/stepper_driver.h"
#include "../hal/photodiode_adc.h"

/* === Private Helpers === */

/**
 * @brief   Move the stepper a given number of half-steps in a direction.
 * @param   steps  Number of half-steps (positive).
 * @param   dir    Direction to move.
 */
static void _moveSteps(int steps, StepperDir_t dir) {
    for (int i = 0; i < steps; i++) {
        stepper_step(dir);
        delayMicroseconds(STEPPER_STEP_INTERVAL_US);
    }
}

/**
 * @brief   Move to an absolute half-step position.
 * @param   targetPos  Target half-step position.
 */
static void _moveToPosition(int32_t targetPos) {
    int32_t currentPos = stepper_getPosition();
    int32_t delta = targetPos - currentPos;

    StepperDir_t dir = (delta >= 0)
        ? STEPPER_DIR_CW
        : STEPPER_DIR_CCW;

    int steps = (delta >= 0) ? delta : -delta;
    _moveSteps(steps, dir);
}

/* === Public API Implementation === */

void alignment_init(void) {
    stepper_init();
    photodiode_init();
    Serial.println("[DOCKING] Alignment controller initialized.");
}

AlignResult_t alignment_runFullScan(void) {
    AlignResult_t result;
    result.status = ALIGN_STATUS_SCANNING;
    result.finalPosition = 0;
    result.finalAngleDeg = 0.0f;
    result.coarsePeakIndex = 0;
    result.coarsePeakValue = 0;
    result.finePeakValue = 0;

    Serial.println("==========================================");
    Serial.println("[DOCKING] Starting full alignment scan...");
    Serial.println("==========================================");

    // Reset position reference
    stepper_resetPosition();

    // ==================================================================
    // PHASE 1: COARSE SWEEP (128 points × 32 half-steps = full 360°)
    // ==================================================================
    Serial.println("[DOCKING] Phase 1: Coarse sweep (128 points)...");

    int coarseValues[COARSE_SCAN_POINTS];
    int coarsePeakIdx = 0;
    int coarsePeakVal = 0;

    for (int i = 0; i < COARSE_SCAN_POINTS; i++) {
        // Settle after movement
        delay(STEPPER_SETTLE_TIME_MS);

        // Read averaged intensity
        int intensity = photodiode_readAveraged(COARSE_ADC_OVERSAMPLE);
        coarseValues[i] = intensity;

        if (intensity > coarsePeakVal) {
            coarsePeakVal = intensity;
            coarsePeakIdx = i;
        }

        // Move to next coarse position
        if (i < COARSE_SCAN_POINTS - 1) {
            _moveSteps(COARSE_STEPS_PER_POINT, STEPPER_DIR_CW);
        }
    }

    result.coarsePeakIndex = coarsePeakIdx;
    result.coarsePeakValue = coarsePeakVal;

    float coarsePeakAngle = coarsePeakIdx * COARSE_STEPS_PER_POINT
                            * STEPPER_DEGREES_PER_STEP;

    Serial.print("[DOCKING] Coarse peak at index ");
    Serial.print(coarsePeakIdx);
    Serial.print(" (");
    Serial.print(coarsePeakAngle, 1);
    Serial.print("°), intensity: ");
    Serial.println(coarsePeakVal);

    // Check minimum intensity threshold
    if (coarsePeakVal < DOCKING_MIN_INTENSITY_THRESHOLD) {
        Serial.println("[DOCKING] ERROR: No beacon detected above threshold.");
        Serial.println("[DOCKING] >>> ALIGNMENT FAILED <<<");
        stepper_release();
        result.status = ALIGN_STATUS_FAILED;
        return result;
    }

    // ==================================================================
    // PHASE 2: FINE SWEEP (±22.4° around coarse peak at 1 half-step)
    // ==================================================================
    Serial.println("[DOCKING] Phase 2: Fine sweep...");

    // Calculate fine sweep start position
    int32_t fineStartPos = (int32_t)(coarsePeakIdx - FINE_SCAN_COARSE_HALF_WIDTH)
                           * COARSE_STEPS_PER_POINT;

    // Move to fine sweep start
    _moveToPosition(fineStartPos);
    delay(STEPPER_SETTLE_TIME_MS);

    int finePeakVal = 0;
    int32_t finePeakPos = fineStartPos;

    for (int i = 0; i < FINE_SCAN_POINTS; i++) {
        delay(STEPPER_SETTLE_TIME_MS);

        int intensity = photodiode_readAveraged(FINE_ADC_OVERSAMPLE);

        if (intensity > finePeakVal) {
            finePeakVal = intensity;
            finePeakPos = stepper_getPosition();
        }

        // Move one half-step
        if (i < FINE_SCAN_POINTS - 1) {
            stepper_step(STEPPER_DIR_CW);
            delayMicroseconds(STEPPER_STEP_INTERVAL_US);
        }
    }

    result.finePeakValue = finePeakVal;

    float finePeakAngle = finePeakPos * STEPPER_DEGREES_PER_STEP;
    Serial.print("[DOCKING] Fine peak at position ");
    Serial.print(finePeakPos);
    Serial.print(" (");
    Serial.print(finePeakAngle, 1);
    Serial.print("°), intensity: ");
    Serial.println(finePeakVal);

    // ==================================================================
    // BACKLASH COMPENSATION
    // ==================================================================
    Serial.println("[DOCKING] Applying backlash compensation...");

    // Overshoot past target position (CCW direction)
    int32_t overshootPos = finePeakPos - BACKLASH_COMPENSATION_STEPS;
    _moveToPosition(overshootPos);
    delay(STEPPER_SETTLE_TIME_MS);

    // Approach final position from CW direction (consistent approach)
    _moveToPosition(finePeakPos);
    delay(STEPPER_SETTLE_TIME_MS);

    // ==================================================================
    // RESULT
    // ==================================================================
    result.status = ALIGN_STATUS_ALIGNED;
    result.finalPosition = finePeakPos;
    result.finalAngleDeg = finePeakAngle;

    Serial.println("==========================================");
    Serial.print("[DOCKING] >>> ALIGNED at ");
    Serial.print(finePeakAngle, 2);
    Serial.println("° <<<");
    Serial.println("==========================================");

    // Release motor coils (save power; motor holds via gear friction)
    stepper_release();

    return result;
}
