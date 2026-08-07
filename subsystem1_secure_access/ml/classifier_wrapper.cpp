/**
 * @file    classifier_wrapper.cpp
 * @brief   Edge Impulse TinyML classifier wrapper — implementation.
 * @author  Kiruthik R S
 * @date    2026-08-07
 * @version 1.0.0
 */

#include "classifier_wrapper.h"

#include <Arduino.h>

#include <NOVA_Secure_Lock_inferencing.h>

#include "classifier_wrapper.h"

#include <Arduino.h>

#include <NOVA_Secure_Lock_inferencing.h>

#include "../config.h"

/* === Logging Abstraction (AUD-008) === */
#if defined(DEBUG) || defined(ARDUINO)
#define ML_LOG_PRINT(x)   Serial.print(x)
#define ML_LOG_PRINTLN(x) Serial.println(x)
#else
#define ML_LOG_PRINT(x)
#define ML_LOG_PRINTLN(x)
#endif

/* === Public API Implementation === */

bool classifier_init(void) {
    // Currently a no-op. Edge Impulse library initializes lazily
    // on first call to run_classifier(). This function exists for
    // API consistency and future initialization needs.
    ML_LOG_PRINTLN("[ML] Classifier wrapper initialized.");
    return true;
}

bool classifier_run(
    const float* samples,
    int numSamples,
    ClassifierResult_t* result
) {
    if (!samples || !result || numSamples <= 0) {
        return false;
    }

    // Initialize result to safe defaults
    result->label = NULL;
    result->confidence = 0.0f;
    result->valid = false;
    result->latency_us = 0;

    // Edge Impulse requires a mutable float buffer for signal_from_buffer.
    // Our sampler buffer is const, so we use const_cast (AUD-009). The EI library
    // does not modify the buffer during inference.
    signal_t signal;
    int err = numpy::signal_from_buffer(
        const_cast<float*>(samples), numSamples, &signal
    );
    if (err != 0) {
        ML_LOG_PRINT("[ML] ERROR: signal_from_buffer failed, code: ");
        ML_LOG_PRINTLN(err);
        return false;
    }

    // Run the classifier — measure latency
    uint32_t startUs = micros();

    ei_impulse_result_t eiResult = {0};
    err = run_classifier(&signal, &eiResult, false);

    uint32_t endUs = micros();
    result->latency_us = (int32_t)(endUs - startUs);

    if (err != EI_IMPULSE_OK) {
        ML_LOG_PRINT("[ML] ERROR: run_classifier failed, code: ");
        ML_LOG_PRINTLN(err);
        return false;
    }

    // Find the label with highest confidence
    float highestConf = 0.0f;
    const char* bestLabel = NULL;

    for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        if (eiResult.classification[i].value > highestConf) {
            highestConf = eiResult.classification[i].value;
            bestLabel = eiResult.classification[i].label;
        }
    }

    result->label = bestLabel;
    result->confidence = highestConf;
    result->valid = true;

    return true;
}
