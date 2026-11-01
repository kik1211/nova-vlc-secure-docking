/**
 * @file    goertzel.h
 * @brief   Goertzel frequency detector — pure DSP module.
 * @author  Kiruthik R S
 * @date    2026-08-07
 * @version 1.0.0
 *
 * Computes the Goertzel algorithm at three exact DFT bins (k=10, k=20, k=30)
 * for a 100-sample buffer sampled at 100 Hz. Zero spectral leakage because
 * all target frequencies align with integer bin indices.
 *
 * This module is completely stateless and hardware-independent.
 * It includes no Arduino or ESP-IDF headers — only standard math.
 *
 * @note    Design Freeze §5.3. See knowledge-base/08_GOERTZEL/ for theory.
 */

#ifndef NOVA_GOERTZEL_H
#define NOVA_GOERTZEL_H

#include <stdint.h>
#include <stdbool.h>

/* === Public Type Definitions === */

/// @brief  Result of Goertzel analysis across all target frequencies.
typedef struct {
    float   magnitude_10hz;        ///< Goertzel magnitude at 10 Hz (k=10)
    float   magnitude_20hz;        ///< Goertzel magnitude at 20 Hz (k=20)
    float   magnitude_30hz;        ///< Goertzel magnitude at 30 Hz (k=30)
    bool    dominant_found;        ///< true if one frequency clearly dominates
    uint8_t dominant_frequency_hz; ///< 10, 20, or 30 if found; 0 if ambiguous
} GoertzelResult_t;

/* === Public API === */

/**
 * @brief   Compute Goertzel magnitude at a single target frequency.
 * @param   samples     Pointer to float sample buffer.
 * @param   numSamples  Number of samples in buffer (N).
 * @param   sampleRate  Sampling frequency in Hz (fs).
 * @param   targetFreq  Frequency to detect in Hz.
 * @return  Magnitude squared of the DFT at the target frequency.
 *
 * @note    For exact-bin detection: k = targetFreq / sampleRate * numSamples
 *          must be an integer. At N=100, fs=100: 10→k=10, 20→k=20, 30→k=30.
 */
float goertzel_magnitude(
    const float* samples,
    int numSamples,
    float sampleRate,
    float targetFreq
);

/**
 * @brief   Analyze a sample buffer at all three target frequencies.
 * @param   samples     Pointer to float sample buffer.
 * @param   numSamples  Number of samples (expected: SAMPLE_COUNT = 100).
 * @param   sampleRate  Sampling frequency in Hz (expected: 100.0f).
 * @return  GoertzelResult_t with magnitudes and dominant frequency decision.
 *
 * @note    A frequency is considered "dominant" if its magnitude exceeds the
 *          next-highest by at least GOERTZEL_MIN_DOMINANCE_RATIO (from config.h).
 */
GoertzelResult_t goertzel_analyze(
    const float* samples,
    int numSamples,
    float sampleRate
);

#endif /* NOVA_GOERTZEL_H */
