/**
 * @file    photodiode_adc.cpp
 * @brief   Photodiode ADC reader HAL — implementation.
 * @author  Kiruthik R S
 * @date    2026-08-07
 * @version 1.0.0
 */

#include "photodiode_adc.h"

#include <Arduino.h>

#include "../config.h"

/* === Public API Implementation === */

void photodiode_init(void) {
    pinMode(DOCKING_SENSOR_PIN, INPUT);
    analogReadResolution(ADC_RESOLUTION_BITS);
    Serial.println("[HAL][PHOTODIODE] ADC initialized on GPIO ");
    Serial.println(DOCKING_SENSOR_PIN);
}

int photodiode_readRaw(void) {
    return analogRead(DOCKING_SENSOR_PIN);
}

int photodiode_readAveraged(int numSamples) {
    if (numSamples <= 0) {
        return 0;
    }
    long sum = 0;
    for (int i = 0; i < numSamples; i++) {
        sum += analogRead(DOCKING_SENSOR_PIN);
    }
    return (int)(sum / numSamples);
}
