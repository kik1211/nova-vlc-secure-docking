/**
 * @file    test_classifier.ino
 * @brief   Validation Test Harness — Edge Impulse TinyML Classifier Integration.
 * @author  Kiruthik R S
 * @date    2026-08-07
 * 
 * Verifies TinyML model execution:
 * 1. Edge Impulse DSP & Classifier Initialization
 * 2. Inference latency measurement on ESP32-S3
 * 3. Confidence score evaluation against ML_CONFIDENCE_THRESHOLD (0.85)
 * 4. Label resolution matching (freq_10hz, freq_20hz, freq_30hz, noise)
 */

#include "../subsystem1_secure_access/config.h"
#include "../subsystem1_secure_access/hal/optical_sampler.h"
#include "../subsystem1_secure_access/ml/classifier_wrapper.h"

void printClassification(const ClassifierResult_t& res) {
    Serial.print("  Valid:       "); Serial.println(res.valid ? "YES" : "NO");
    Serial.print("  Winning Label:"); Serial.println(res.label ? res.label : "NONE");
    Serial.print("  Confidence:  "); Serial.print(res.confidence * 100.0f, 2); Serial.println("%");
    Serial.print("  Latency:     "); Serial.print(res.latency_us); Serial.println(" us");
    Serial.print("  Threshold:   "); Serial.print(ML_CONFIDENCE_THRESHOLD * 100.0f, 1); Serial.println("%");
    Serial.print("  Gate Pass:   "); Serial.println(res.confidence >= ML_CONFIDENCE_THRESHOLD ? "PASS" : "REJECT");
}

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    unsigned long start = millis();
    while (!Serial && (millis() - start < SERIAL_INIT_DELAY_MS));

    Serial.println("\n==========================================");
    Serial.println("   TINYML CLASSIFIER VALIDATION HARNESS");
    Serial.println("==========================================");

    bool mlOk = classifier_init();
    Serial.print("[TEST_ML] Classifier Init: "); Serial.println(mlOk ? "PASS" : "FAIL");

    bool samplerOk = sampler_init();
    Serial.print("[TEST_ML] Sampler Init:    "); Serial.println(samplerOk ? "PASS" : "FAIL");

    Serial.println("\nReady for optical test capture.");
    Serial.println("Point modulated LED at photodiode and press 'c' in Serial Monitor...");
}

void runCaptureAndInference() {
    Serial.println("\n[TEST_ML] Capturing 1.000s optical buffer...");
    if (!sampler_startCapture()) {
        Serial.println("ERROR: Sampler start failed!");
        return;
    }
    while (!sampler_isComplete());

    Serial.println("[TEST_ML] Capture complete. Running Edge Impulse inference...");
    const float* buf = sampler_getBuffer();

    ClassifierResult_t res;
    bool ok = classifier_run(buf, SAMPLE_COUNT, &res);

    if (!ok) {
        Serial.println("ERROR: Classifier execution failed!");
        return;
    }

    printClassification(res);
}

void loop() {
    if (Serial.available() > 0) {
        char ch = Serial.read();
        if (ch == 'c' || ch == 'C') {
            runCaptureAndInference();
        }
    }
}
