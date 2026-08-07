/**
 * @file    optical_sampler.cpp
 * @brief   Timer-driven ADC sampling HAL — implementation.
 * @author  Kiruthik R S
 * @date    2026-08-07
 * @version 1.0.0
 */

#include "optical_sampler.h"

#include <Arduino.h>
#include <esp_timer.h>

#include "../config.h"

/* === Private Module State === */
static float    s_sampleBuffer[SAMPLE_COUNT];
static volatile int      s_sampleIndex = 0;
static volatile bool     s_captureComplete = false;
static volatile bool     s_saturated = false;
static volatile uint32_t s_captureStartUs = 0;
static volatile uint32_t s_captureEndUs = 0;

static esp_timer_handle_t s_timerHandle = NULL;

/* === Private ISR === */

/**
 * @brief   Timer ISR — reads one ADC sample and stores it.
 * @note    Runs in IRAM_ATTR ISR context. Must be fast — zero allocation, no Serial.
 *          `analogRead()` on ESP32-S3 accesses the hardware SAR ADC peripheral directly.
 *          Total ISR execution duration is ~12--15 µs, taking < 0.15% of the 10,000 µs period (AUD-010).
 */
static void IRAM_ATTR _isr_samplerTick(void* arg) {
    (void)arg;
    if (s_sampleIndex >= SAMPLE_COUNT) {
        return;
    }

    int rawValue = analogRead(SENSOR_PIN);
    s_sampleBuffer[s_sampleIndex] = (float)rawValue;

    if (rawValue >= ADC_SATURATION_THRESHOLD) {
        s_saturated = true;
    }

    if (s_sampleIndex == 0) {
        s_captureStartUs = (uint32_t)esp_timer_get_time();
    }

    s_sampleIndex++;

    if (s_sampleIndex >= SAMPLE_COUNT) {
        s_captureEndUs = (uint32_t)esp_timer_get_time();
        s_captureComplete = true;
        esp_timer_stop(s_timerHandle);
    }
}

/* === Public API Implementation === */

bool sampler_init(void) {
    pinMode(SENSOR_PIN, INPUT);
    analogReadResolution(ADC_RESOLUTION_BITS);

    esp_timer_create_args_t timerArgs = {};
    timerArgs.callback = _isr_samplerTick;
    timerArgs.name = "optical_sampler";

    esp_err_t err = esp_timer_create(&timerArgs, &s_timerHandle);
    if (err != ESP_OK) {
        Serial.println("[HAL][SAMPLER] ERROR: Timer creation failed.");
        return false;
    }

    Serial.println("[HAL][SAMPLER] Optical sampler initialized.");
    return true;
}

bool sampler_startCapture(void) {
    if (!s_timerHandle) {
        return false;
    }
    if (s_sampleIndex > 0 && !s_captureComplete) {
        return false; // Capture already in progress
    }

    // Reset state
    s_sampleIndex = 0;
    s_captureComplete = false;
    s_saturated = false;
    s_captureStartUs = 0;
    s_captureEndUs = 0;

    esp_err_t err = esp_timer_start_periodic(
        s_timerHandle, SAMPLE_INTERVAL_US
    );
    if (err != ESP_OK) {
        Serial.println("[HAL][SAMPLER] ERROR: Timer start failed.");
        return false;
    }

    return true;
}

bool sampler_isComplete(void) {
    return s_captureComplete;
}

const float* sampler_getBuffer(void) {
    return s_sampleBuffer;
}

bool sampler_wasSaturated(void) {
    return s_saturated;
}

uint32_t sampler_getElapsedUs(void) {
    if (!s_captureComplete) {
        return 0;
    }
    return s_captureEndUs - s_captureStartUs;
}
