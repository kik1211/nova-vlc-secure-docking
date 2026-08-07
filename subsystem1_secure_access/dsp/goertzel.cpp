/**
 * @file    goertzel.cpp
 * @brief   Goertzel frequency detector — implementation.
 * @author  Kiruthik R S
 * @date    2026-08-07
 * @version 1.0.0
 *
 * Implements the standard iterative Goertzel algorithm.
 *
 * The Goertzel recurrence for bin k over N samples:
 *
 *   coeff = 2 * cos(2 * PI * k / N)
 *
 *   s0 = 0, s1 = 0, s2 = 0
 *   for each sample x[n]:
 *       s0 = x[n] + coeff * s1 - s2
 *       s2 = s1
 *       s1 = s0
 *
 *   magnitude^2 = s1^2 + s2^2 - coeff * s1 * s2
 *
 * This computes exactly the DFT magnitude squared at bin k,
 * using O(N) multiplications instead of O(N*log(N)) for a full FFT.
 */

#include "goertzel.h"
#include "../config.h"

#include <math.h>

/* === Public API Implementation === */

float goertzel_magnitude(
    const float* samples,
    int numSamples,
    float sampleRate,
    float targetFreq
) {
    if (!samples || numSamples <= 0 || sampleRate <= 0.0f) {
        return 0.0f;
    }

    // Compute the normalized frequency index k
    // k = targetFreq / sampleRate * numSamples
    float k = targetFreq / sampleRate * (float)numSamples;

    // Goertzel coefficient: 2 * cos(2 * PI * k / N)
    float omega = 2.0f * (float)M_PI * k / (float)numSamples;
    float coeff = 2.0f * cosf(omega);

    // Iterative recurrence
    float s1 = 0.0f;
    float s2 = 0.0f;

    for (int n = 0; n < numSamples; n++) {
        float s0 = samples[n] + coeff * s1 - s2;
        s2 = s1;
        s1 = s0;
    }

    // Magnitude squared = s1^2 + s2^2 - coeff * s1 * s2
    float magnitudeSq = s1 * s1 + s2 * s2 - coeff * s1 * s2;
    return magnitudeSq;
}

GoertzelResult_t goertzel_analyze(
    const float* samples,
    int numSamples,
    float sampleRate
) {
    GoertzelResult_t result;
    result.magnitude_10hz = 0.0f;
    result.magnitude_20hz = 0.0f;
    result.magnitude_30hz = 0.0f;
    result.dominant_found = false;
    result.dominant_frequency_hz = 0;

    if (!samples || numSamples <= 0) {
        return result;
    }

    // Compute magnitude at each target frequency
    result.magnitude_10hz = goertzel_magnitude(
        samples, numSamples, sampleRate, GOERTZEL_TARGET_FREQ_1
    );
    result.magnitude_20hz = goertzel_magnitude(
        samples, numSamples, sampleRate, GOERTZEL_TARGET_FREQ_2
    );
    result.magnitude_30hz = goertzel_magnitude(
        samples, numSamples, sampleRate, GOERTZEL_TARGET_FREQ_3
    );

    // Find dominant frequency
    float magnitudes[3] = {
        result.magnitude_10hz,
        result.magnitude_20hz,
        result.magnitude_30hz
    };
    uint8_t freqs[3] = {
        (uint8_t)GOERTZEL_TARGET_FREQ_1,
        (uint8_t)GOERTZEL_TARGET_FREQ_2,
        (uint8_t)GOERTZEL_TARGET_FREQ_3
    };

    // Find index of maximum magnitude
    int maxIdx = 0;
    for (int i = 1; i < 3; i++) {
        if (magnitudes[i] > magnitudes[maxIdx]) {
            maxIdx = i;
        }
    }

    // Find second-highest magnitude
    float secondMax = 0.0f;
    for (int i = 0; i < 3; i++) {
        if (i != maxIdx && magnitudes[i] > secondMax) {
            secondMax = magnitudes[i];
        }
    }

    // Check dominance ratio
    if (magnitudes[maxIdx] > 0.0f && secondMax > 0.0f) {
        float ratio = magnitudes[maxIdx] / secondMax;
        if (ratio >= GOERTZEL_MIN_DOMINANCE_RATIO) {
            result.dominant_found = true;
            result.dominant_frequency_hz = freqs[maxIdx];
        }
    } else if (magnitudes[maxIdx] > 0.0f && secondMax == 0.0f) {
        // Only one frequency has any energy
        result.dominant_found = true;
        result.dominant_frequency_hz = freqs[maxIdx];
    }

    return result;
}
