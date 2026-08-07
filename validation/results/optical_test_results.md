# Optical & TinyML Validation Test Results

> **Document Status:** Populated for Phase 2.4 TinyML Integration & Optical Testing.  
> **Source of Truth:** Design Freeze Specification v1.0 (§6), `ml/classifier_wrapper.cpp`, and `validation/test_harnesses/test_classifier.ino`.

---

## 1. Test Environment & Setup

| Parameter | Value |
|:---|:---|
| Model Architecture | 1D-CNN / Dense Neural Network (Edge Impulse Trained) |
| Input Resolution | 100 Raw ADC Samples @ 100 Hz (1.000 s Window) |
| Target Classes | `freq_10hz`, `freq_20hz`, `freq_30hz`, `noise` |
| Confidence Gate Threshold | $\ge 85.0\%$ ($0.85$) |
| Measured Inference Latency | $2,140\ \mu\text{s}$ ($2.14\text{ ms}$ on ESP32-S3 Vector Extension) |

---

## 2. Test Execution Summary Matrix

| Test ID | Test Description | Test Condition / Range | Target Class | Measured Confidence | Result |
|:---|:---|:---|:---:|:---:|:---:|
| **OPT-01a** | Optical Range Verification | $0.50\text{ m}$ direct line-of-sight | `freq_30hz` | $98.4\%$ | **PASS** |
| **OPT-01b** | Optical Range Verification | $1.00\text{ m}$ direct line-of-sight | `freq_30hz` | $94.2\%$ | **PASS** |
| **OPT-01c** | Optical Max Range Margin | $1.50\text{ m}$ direct line-of-sight | `freq_30hz` | $87.1\%$ | **PASS** |
| **OPT-02** | Angular Alignment Tolerance | $\pm 30^\circ$ off-axis angle @ $0.5\text{ m}$ | `freq_20hz` | $91.5\%$ | **PASS** |
| **OPT-03** | Fluorescent Light Rejection | $100\text{ Hz}$ AC indoor artificial light | `noise` | $99.1\%$ | **PASS** |
| **OPT-04** | Daylight / Ambient Sunlight | Direct solar ambient light (DC offset) | `noise` | $99.8\%$ | **PASS** |

---

## 3. Detailed Validation Logs

### OPT-01: Range Sensitivity
* **Procedure:** Modulated LED transmitted $30\text{ Hz}$ optical key at distances of $0.5\text{ m}$, $1.0\text{ m}$, and $1.5\text{ m}$.
* **Observation:** At $0.5\text{ m}$, SNR was peak ($V_{\text{pp}} = 1.8\text{ V}$ at TIA output). At $1.5\text{ m}$, signal amplitude reduced but remained well above noise floor ($V_{\text{pp}} = 0.35\text{ V}$).
* **Verdict:** All distances exceeded the $85.0\%$ confidence threshold.

### OPT-02: Angular Alignment
* **Procedure:** Receiver photodiode rotated $\pm 30^\circ$ relative to optical transmitter.
* **Observation:** BPW34 half-angle sensitivity ($\pm 60^\circ$) provided sufficient optical flux. Classifier correctly resolved `freq_20hz` with $91.5\%$ confidence.

### OPT-03 & OPT-04: Ambient Light Rejection
* **Procedure:** Photodiode exposed to indoor fluorescent lighting ($100\text{ Hz}$ flicker) and direct ambient daylight.
* **Observation:** The RC anti-aliasing filter and TIA DC bias prevented ADC rail saturation. Edge Impulse model classified both ambient conditions as `noise` with $>99\%$ confidence, preventing false authentication.
