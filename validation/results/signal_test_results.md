# Signal Processing Validation Test Results

> **Document Status:** Populated for Phase 2.3 DSP Verification.  
> **Source of Truth:** Design Freeze Specification v1.0 (§5), `dsp/goertzel.cpp`, and `validation/test_harnesses/test_goertzel.py`.

---

## 1. Test Setup & Parameters

| Parameter | Specification / Value |
|:---|:---|
| Sampling Frequency ($f_s$) | $100.0\text{ Hz}$ |
| Window Length ($N$) | $100\text{ samples}$ ($1.000\text{ s}$ capture) |
| DFT Bin Indices | $k=10$ ($10\text{ Hz}$), $k=20$ ($20\text{ Hz}$), $k=30$ ($30\text{ Hz}$) |
| Dominance Ratio Threshold | $\frac{M_{\text{dominant}}}{M_{\text{second}}} \ge 2.0$ |
| Spectral Leakage Expectation | Exactly zero leakage (integer DFT bins) |

---

## 2. Test Execution Summary Matrix

| Test ID | Test Description | Target Frequency | Dominant Bin Detected | Measured Leakage | Dominance Ratio | Status |
|:---|:---|:---:|:---:|:---:|:---:|:---:|
| **SP-01** | Goertzel 10 Hz Detection | $10.0\text{ Hz}$ ($k=10$) | $10\text{ Hz}$ | $0.00\text{ (0.0\%)}$ | $\infty$ | **PASS** |
| **SP-02** | Goertzel 20 Hz Detection | $20.0\text{ Hz}$ ($k=20$) | $20\text{ Hz}$ | $0.00\text{ (0.0\%)}$ | $\infty$ | **PASS** |
| **SP-03** | Goertzel 30 Hz Detection | $30.0\text{ Hz}$ ($k=30$) | $30\text{ Hz}$ | $0.00\text{ (0.0\%)}$ | $\infty$ | **PASS** |
| **SP-04** | Broadband Ambient Noise Rejection | White / Ambient Noise | None (`0 Hz`) | Broad distribution | $1.44 < 2.0$ | **PASS** |

---

## 3. Empirical Test Outputs

### SP-01: 10 Hz (Guest Key Bin $k=10$)
* **Input Signal:** $A = 500\text{ ADC counts}$, $f = 10.0\text{ Hz}$, $N = 100$.
* **Calculated Magnitudes:**
  * $M(10\text{ Hz}) = 625,000,000.0$
  * $M(20\text{ Hz}) = 0.0$
  * $M(30\text{ Hz}) = 0.0$
* **Verdict:** Dominant frequency $10\text{ Hz}$ identified with $0\%$ spectral leakage.

### SP-02: 20 Hz (Staff Key Bin $k=20$)
* **Input Signal:** $A = 500\text{ ADC counts}$, $f = 20.0\text{ Hz}$, $N = 100$.
* **Calculated Magnitudes:**
  * $M(10\text{ Hz}) = 0.0$
  * $M(20\text{ Hz}) = 625,000,000.0$
  * $M(30\text{ Hz}) = 0.0$
* **Verdict:** Dominant frequency $20\text{ Hz}$ identified with $0\%$ spectral leakage.

### SP-03: 30 Hz (Admin Key Bin $k=30$)
* **Input Signal:** $A = 500\text{ ADC counts}$, $f = 30.0\text{ Hz}$, $N = 100$.
* **Calculated Magnitudes:**
  * $M(10\text{ Hz}) = 0.0$
  * $M(20\text{ Hz}) = 0.0$
  * $M(30\text{ Hz}) = 625,000,000.0$
* **Verdict:** Dominant frequency $30\text{ Hz}$ identified with $0\%$ spectral leakage.

### SP-04: Ambient Noise Baseline Rejection
* **Input Signal:** Random broadband noise ($\pm 50\text{ ADC counts}$).
* **Calculated Magnitudes:**
  * $M(10\text{ Hz}) = 26,953.3$
  * $M(20\text{ Hz}) = 5,057.6$
  * $M(30\text{ Hz}) = 18,635.3$
* **Dominance Ratio:** $\frac{26953.3}{18635.3} = 1.446 < 2.0$ (Threshold).
* **Verdict:** Rejected as ambiguous (`dominant_found = false`).
