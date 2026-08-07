/**
 * @file    test_goertzel.ino
 * @brief   Validation Test Harness — Goertzel DSP Frequency Classification.
 * @author  Kiruthik R S
 * @date    2026-08-07
 * 
 * Verifies Goertzel algorithm accuracy across synthetic test vectors:
 * 1. Pure 10 Hz Sine Wave (Guest Key)
 * 2. Pure 20 Hz Sine Wave (Staff Key)
 * 3. Pure 30 Hz Sine Wave (Admin Key)
 * 4. Mixed Noise / Ambiguous Frequencies
 */

#include <math.h>
#include "../subsystem1_secure_access/config.h"
#include "../subsystem1_secure_access/dsp/goertzel.h"

void generateSyntheticSine(float* buffer, int n, float sampleRate, float signalFreq, float amplitude, float offset) {
    for (int i = 0; i < n; i++) {
        float t = (float)i / sampleRate;
        buffer[i] = offset + amplitude * sinf(2.0f * (float)M_PI * signalFreq * t);
    }
}

void testFrequency(float targetFreq, const char* label) {
    Serial.print("\n------------------------------------------\n");
    Serial.print("Testing Input Signal: "); Serial.print(label);
    Serial.print(" ("); Serial.print(targetFreq, 1); Serial.println(" Hz)");

    float samples[SAMPLE_COUNT];
    generateSyntheticSine(samples, SAMPLE_COUNT, SAMPLING_FREQUENCY_HZ, targetFreq, 500.0f, 2048.0f);

    GoertzelResult_t res = goertzel_analyze(samples, SAMPLE_COUNT, SAMPLING_FREQUENCY_HZ);

    Serial.print("  Magnitude 10 Hz: "); Serial.println(res.magnitude_10hz, 1);
    Serial.print("  Magnitude 20 Hz: "); Serial.println(res.magnitude_20hz, 1);
    Serial.print("  Magnitude 30 Hz: "); Serial.println(res.magnitude_30hz, 1);
    Serial.print("  Dominant Found:  "); Serial.println(res.dominant_found ? "YES" : "NO");
    Serial.print("  Dominant Freq:   "); Serial.print(res.dominant_frequency_hz); Serial.println(" Hz");

    bool pass = res.dominant_found && (res.dominant_frequency_hz == (uint8_t)targetFreq);
    Serial.print("  TEST RESULT:     "); Serial.println(pass ? "PASS" : "FAIL");
}

void testAmbiguousNoise() {
    Serial.println("\n------------------------------------------");
    Serial.println("Testing Input Signal: Ambiguous Broadband Noise");

    float samples[SAMPLE_COUNT];
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        samples[i] = 2048.0f + (float)(random(-100, 100));
    }

    GoertzelResult_t res = goertzel_analyze(samples, SAMPLE_COUNT, SAMPLING_FREQUENCY_HZ);

    Serial.print("  Magnitude 10 Hz: "); Serial.println(res.magnitude_10hz, 1);
    Serial.print("  Magnitude 20 Hz: "); Serial.println(res.magnitude_20hz, 1);
    Serial.print("  Magnitude 30 Hz: "); Serial.println(res.magnitude_30hz, 1);
    Serial.print("  Dominant Found:  "); Serial.println(res.dominant_found ? "YES" : "NO");

    bool pass = !res.dominant_found;
    Serial.print("  TEST RESULT:     "); Serial.println(pass ? "PASS (Correctly Rejected)" : "FAIL (False Positive)");
}

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    unsigned long start = millis();
    while (!Serial && (millis() - start < SERIAL_INIT_DELAY_MS));

    Serial.println("\n==========================================");
    Serial.println("   GOERTZEL DSP VALIDATION HARNESS");
    Serial.println("==========================================");

    testFrequency(10.0f, "GUEST KEY");
    testFrequency(20.0f, "STAFF KEY");
    testFrequency(30.0f, "ADMIN KEY");
    testAmbiguousNoise();

    Serial.println("\n==========================================");
    Serial.println("   ALL GOERTZEL DSP TESTS COMPLETE");
    Serial.println("==========================================");
}

void loop() {
    // Single-shot test in setup()
}
