/**
 * @file    alignment_controller.h
 * @brief   Two-phase optical docking alignment controller.
 * @author  Kiruthik R S
 * @date    2026-08-07
 * @version 1.0.0
 *
 * Implements the coarse-then-fine scan algorithm:
 *
 *   1. Coarse sweep: 128 points, 32 half-steps each, 16x ADC average.
 *      Finds approximate angle of maximum intensity.
 *
 *   2. Fine sweep: ±22.4° around coarse peak, 1 half-step resolution,
 *      32x ADC average. Finds precise peak angle.
 *
 *   3. Backlash compensation: final position approached from CCW only.
 *
 * @note    Design Freeze §7.4.
 */

#ifndef NOVA_ALIGNMENT_CONTROLLER_H
#define NOVA_ALIGNMENT_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>

/* === Public Type Definitions === */

/// @brief  Docking alignment result code.
typedef enum {
    ALIGN_STATUS_IDLE = 0,   ///< Not started
    ALIGN_STATUS_SCANNING,   ///< Scan in progress
    ALIGN_STATUS_ALIGNED,    ///< Peak found, motor positioned
    ALIGN_STATUS_FAILED      ///< No beacon detected above threshold
} AlignStatus_t;

/// @brief  Full scan result with diagnostic data.
typedef struct {
    AlignStatus_t status;
    int32_t       finalPosition;    ///< Half-step position of peak
    float         finalAngleDeg;    ///< Peak angle in degrees
    int           coarsePeakIndex;  ///< Coarse scan point index of peak
    int           coarsePeakValue;  ///< ADC value at coarse peak
    int           finePeakValue;    ///< ADC value at fine peak
} AlignResult_t;

/* === Public API === */

/**
 * @brief   Run the complete two-phase alignment scan.
 * @return  AlignResult_t with status, angle, and diagnostic values.
 *
 * @note    This function is BLOCKING — it runs both coarse and fine scans
 *          sequentially. Total time ≈ (128 × 32 + 512) × 2 ms ≈ 9 seconds.
 *          This is acceptable for a proof-of-concept demonstration where
 *          alignment runs once on user command.
 *
 * @note    After completion, the motor is positioned at the peak angle
 *          and then released (de-energized) to save power.
 */
AlignResult_t alignment_runFullScan(void);

/**
 * @brief   Initialize the alignment controller and underlying HAL modules.
 */
void alignment_init(void);

#endif /* NOVA_ALIGNMENT_CONTROLLER_H */
