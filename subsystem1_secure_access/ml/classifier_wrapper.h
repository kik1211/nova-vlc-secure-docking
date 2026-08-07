/**
 * @file    classifier_wrapper.h
 * @brief   Edge Impulse TinyML classifier wrapper — clean integration layer.
 * @author  Kiruthik R S
 * @date    2026-08-07
 * @version 1.0.0
 *
 * Wraps the Edge Impulse generated library (numpy::signal_from_buffer and
 * run_classifier) behind a clean API. The application code never directly
 * calls Edge Impulse functions — only this wrapper.
 *
 * @note    The Edge Impulse library is treated as read-only generated code.
 *          This wrapper does NOT modify any EI library file.
 */

#ifndef NOVA_CLASSIFIER_WRAPPER_H
#define NOVA_CLASSIFIER_WRAPPER_H

#include <stdint.h>
#include <stdbool.h>

/* === Public Type Definitions === */

/// @brief  Result of a single ML classification run.
typedef struct {
    const char* label;       ///< Winning class label string (e.g., "freq_30hz")
    float       confidence;  ///< Confidence of winning class (0.0–1.0)
    bool        valid;       ///< true if inference completed without error
    int32_t     latency_us;  ///< Inference time in microseconds (measured)
} ClassifierResult_t;

/* === Public API === */

/**
 * @brief   Initialize the classifier (currently a no-op; reserves for future use).
 * @return  true always. Provided for API consistency with other modules.
 */
bool classifier_init(void);

/**
 * @brief   Run the Edge Impulse classifier on a sample buffer.
 * @param   samples     Pointer to float array of raw ADC samples.
 * @param   numSamples  Number of samples (must equal EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE).
 * @param   result      Pointer to ClassifierResult_t to populate with output.
 * @return  true if inference completed successfully.
 * @return  false if signal_from_buffer or run_classifier returned an error.
 *
 * @note    This function is blocking — it runs the DSP pipeline and NN inference
 *          synchronously. Typical latency is TBD (must be measured on hardware).
 */
bool classifier_run(
    const float* samples,
    int numSamples,
    ClassifierResult_t* result
);

#endif /* NOVA_CLASSIFIER_WRAPPER_H */
