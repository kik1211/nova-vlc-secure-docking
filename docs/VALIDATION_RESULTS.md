# N.O.V.A. Validation Results & Status Matrix

## 1. Engineering Verification Methodology

To maintain rigorous embedded engineering integrity, verification results in this repository are categorized into three distinct tiers:

1. **Automated CI Compilation:** Clean builds of Subsystem 1 (`esp32s3`) and Subsystem 2 (`esp32`) validated via GitHub Actions ([`.github/workflows/compile_check.yml`](../.github/workflows/compile_check.yml)). *Note: Compilation success confirms syntactic and library build integrity, but is not proof of physical hardware operation.*
2. **Automated Host Test Harnesses:** Deterministic DSP mathematics and FSM state transition suites executed on host Python ([`validation/test_harnesses/`](../validation/test_harnesses/)).
3. **Physical Lab Bench Measurements:** Instrumented physical measurements performed on actual microcontrollers, sensors, and actuators, documented in per-category result logs ([`validation/results/`](../validation/results/)).

---

## 2. Master Validation Summary Table

| Category | Tests | Verification Method | Status | Reference Document |
|:---|:---:|:---|:---:|:---|
| **HW — Hardware Electronics** | 7 | Bench Multimeter & Oscilloscope | 7 Passed | [`hw_test_results.md`](../validation/results/hw_test_results.md) |
| **FW — Firmware & FSM Execution** | 5 | Timer ISR Profiling & Host FSM Harness | 5 Passed | [`fw_test_results.md`](../validation/results/fw_test_results.md) |
| **SP — Signal Processing DSP** | 4 | Host Verification (`test_goertzel.py`) | 4 Passed | [`signal_test_results.md`](../validation/results/signal_test_results.md) |
| **OPT — Optical & TinyML** | 4 | Bench Range / Angular Sensitivity | 4 Passed | [`optical_test_results.md`](../validation/results/optical_test_results.md) |
| **DK — Optical Docking** | 5 | Stepper Scanning Protocol & Angular Log | 5 Passed | [`docking_test_results.md`](../validation/results/docking_test_results.md) |
| **SEC — 2FA Security Gate** | 5 | Host FSM Harness (`test_auth_fsm.py`) | 5 Passed | [`security_test_results.md`](../validation/results/security_test_results.md) |
| **Total** | **30** | | **30 Passed** | |

---

## 3. Results by Verification Domain

### 3.1 Signal Processing (SP-01 to SP-04)
Verified via automated host harness [`validation/test_harnesses/test_goertzel.py`](../validation/test_harnesses/test_goertzel.py):
* **SP-01 (10 Hz Detection):** Target magnitude $M_{10} = 6.25 \times 10^8$ with zero spectral leakage on 20 Hz and 30 Hz bins.
* **SP-02 (20 Hz Detection):** Target magnitude $M_{20} = 6.25 \times 10^8$ with zero spectral leakage on 10 Hz and 30 Hz bins.
* **SP-03 (30 Hz Detection):** Target magnitude $M_{30} = 6.25 \times 10^8$ with zero spectral leakage on 10 Hz and 20 Hz bins.
* **SP-04 (Broadband Noise Rejection):** Random noise input yielded Dominance Ratio $1.44 < 2.0$, correctly rejected as ambiguous.

### 3.2 Security & 2FA Gate (SEC-01 to SEC-05)
Verified via automated host harness [`validation/test_harnesses/test_auth_fsm.py`](../validation/test_harnesses/test_auth_fsm.py):
* **SEC-01 (Role Authorization):** Valid 2FA pairings for Admin (30 Hz), Staff (20 Hz), and Guest (10 Hz) successfully asserted unlock.
* **SEC-02 (Unregistered Card):** Random 4-byte UIDs rejected immediately (`AUTH_REASON_UNKNOWN_CARD`), triggering a 2.0 s lockout.
* **SEC-03 (Low Confidence Veto):** ML confidence $0.75 < 0.85$ vetoed unlock (`AUTH_REASON_ML_LOW_CONFIDENCE`).
* **SEC-04 (Frequency Mismatch Veto):** Valid Admin card presented with 10 Hz optical key vetoed unlock (`AUTH_REASON_GOERTZEL_MISMATCH`).
* **SEC-05 (Ambiguous Spectrum Veto):** Dominance ratio $< 2.0$ vetoed unlock (`AUTH_REASON_GOERTZEL_AMBIGUOUS`).

### 3.3 Hardware Electronics (HW-01 to HW-07)
Bench measurements documented in [`validation/results/hw_test_results.md`](../validation/results/hw_test_results.md):
* **HW-01:** $+3.3\text{ V}$ logic rail measured at $3.31\text{ V}$ steady during concurrent PN532 polling and relay switching.
* **HW-02:** LM358 TIA maximum output swing capped at $2.95\text{ V}$ under saturation (below ESP32-S3 ADC limit).
* **HW-03:** Passive RC filter measured $1.2\text{ dB}$ attenuation at $30\text{ Hz}$ passband and $14.8\text{ dB}$ attenuation at $100\text{ Hz}$ stopband ($f_c \approx 48.2\text{ Hz}$).
* **HW-04:** PN532 I2C communication sustained 100 consecutive reads without bus lockups.
* **HW-05:** Relay GPIO 4 assertion actuated 12V solenoid with clean $5.0\text{ s}$ auto-relock timeout.
* **HW-06:** Stepper 2048 CW + 2048 CCW rotation returned to home with zero net step offset.
* **HW-07:** Stepper motor switching induced $\le 2.1\text{ counts}$ maximum noise delta on photodiode ADC readings.

### 3.4 Firmware Execution (FW-01 to FW-05)
Documented in [`validation/results/fw_test_results.md`](../validation/results/fw_test_results.md):
* **FW-01:** Hardware `esp_timer` captured 100 samples in $1.00032\text{ s}$ ($+0.03\%$ timing error).
* **FW-02:** Role resolution lookup table correctly mapped all compiled UIDs.
* **FW-03:** 30-minute continuous ticking loop showed constant free heap ($324,112\text{ bytes}$) with zero memory leaks.

### 3.5 Optical Range & Docking Mechanics (OPT-01 to OPT-04, DK-01 to DK-05)
Documented in [`validation/results/optical_test_results.md`](../validation/results/optical_test_results.md) and [`validation/results/docking_test_results.md`](../validation/results/docking_test_results.md):
* **OPT-01:** Line-of-sight range verified up to $1.5\text{ m}$ (confidence $\ge 87.1\%$).
* **OPT-02:** Angular tolerance verified up to $\pm 30^\circ$ off-axis ($91.5\%$ confidence).
* **OPT-03/04:** Fluorescent lighting flicker and direct daylight correctly classified as `noise` ($>99\%$ confidence).
* **DK-01 to DK-03:** Two-phase sweep converged to optical beacon with $0.26^\circ$ residual fine alignment error.
* **DK-04:** 16-step backlash compensation achieved $\pm 1$ step repeatability across five consecutive scans.
* **DK-05:** Zero-beacon condition correctly declared `ALIGN_STATUS_FAILED` and released motor coils.
