/**
 * @file    optical_sampler.h
 * @brief   Timer-driven ADC sampling HAL for optical signal capture.
 * @author  Kiruthik R S
 * @date    2026-08-07
 * @version 1.0.0
 *
 * Replaces the blocking delay(10) loop from the original firmware with
 * a hardware timer ISR that triggers ADC reads at precisely 100 Hz.
 * The captured buffer is shared with the DSP and ML pipelines.
 *
 * @note    Uses esp_timer periodic timer. The ISR reads the ADC and stores
 *          the sample; the main thread polls sampler_isComplete().
 */

#ifndef NOVA_OPTICAL_SAMPLER_H
#define NOVA_OPTICAL_SAMPLER_H

#include <stdint.h>
#include <stdbool.h>

/* === Public API === */

/**
 * @brief   Initialize ADC channel and timer peripheral.
 * @return  true if ADC and timer were configured successfully.
 */
bool sampler_init(void);

/**
 * @brief   Begin a new 100-sample capture sequence.
 * @note    Non-blocking — returns immediately. The timer ISR fills the
 *          internal buffer. Poll sampler_isComplete() to know when done.
 * @return  true if capture started successfully.
 * @return  false if a capture is already in progress.
 */
bool sampler_startCapture(void);

/**
 * @brief   Check whether the current capture is complete.
 * @return  true if all SAMPLE_COUNT samples have been collected.
 */
bool sampler_isComplete(void);

/**
 * @brief   Get a pointer to the captured sample buffer.
 * @return  Pointer to float array of SAMPLE_COUNT elements.
 *          Valid only after sampler_isComplete() returns true.
 *          Buffer contents are overwritten on next sampler_startCapture().
 * @note    Returned pointer points to a module-internal static buffer.
 *          Do NOT free it.
 */
const float* sampler_getBuffer(void);

/**
 * @brief   Check whether any ADC saturation was detected during capture.
 * @return  true if any sample exceeded ADC_SATURATION_THRESHOLD.
 */
bool sampler_wasSaturated(void);

/**
 * @brief   Get the actual elapsed time of the last capture in microseconds.
 * @return  Elapsed time from first sample to last sample.
 *          Useful for validating that sampling rate is correct.
 */
uint32_t sampler_getElapsedUs(void);

#endif /* NOVA_OPTICAL_SAMPLER_H */
