/**
 * @file    photodiode_adc.h
 * @brief   Photodiode ADC reader HAL for docking subsystem.
 * @author  Kiruthik R S
 * @date    2026-08-07
 * @version 1.0.0
 *
 * Provides ADC reading and multi-sample averaging for the BPW34 photodiode
 * used in the optical alignment scan. Simpler than the SS1 optical sampler
 * because docking reads are trigger-based (not timer-ISR-based).
 */

#ifndef NOVA_PHOTODIODE_ADC_H
#define NOVA_PHOTODIODE_ADC_H

#include <stdint.h>
#include <stdbool.h>

/* === Public API === */

/**
 * @brief   Initialize the ADC channel for the docking photodiode.
 */
void photodiode_init(void);

/**
 * @brief   Read a single raw ADC sample.
 * @return  12-bit raw ADC value (0–4095).
 */
int photodiode_readRaw(void);

/**
 * @brief   Read and average multiple ADC samples.
 * @param   numSamples  Number of consecutive reads to average.
 * @return  Averaged 12-bit ADC value (integer-averaged).
 */
int photodiode_readAveraged(int numSamples);

#endif /* NOVA_PHOTODIODE_ADC_H */
