# N.O.V.A. Validation Results & Status Matrix

## 1. Engineering Verification Methodology

To maintain rigorous embedded engineering integrity, verification results in this repository are categorized into three mutually exclusive tiers across the 30 logical test IDs:

1. **Tier 1 — Automated Host Model & DSP Harnesses (11 Logical Tests):** Deterministic Goertzel DSP recurrence mathematics (`test_goertzel.py`) and non-blocking authentication FSM decision rules (`test_auth_fsm.py`) executed on host Python. *Note: `test_auth_fsm.py` tests an independent Python behavioral model of the state machine and role resolution lookup table; it does not compile or execute the C++ production binary on physical silicon.*
2. **Tier 2 — Documented Bench-Test Results (12 Logical Tests):** Instrumented lab-bench measurements recorded on prototype breadboards and devkits, documented in per-category result files ([`validation/results/`](../validation/results/)). *Note: While Arduino sketch test harnesses exist in [`validation/test_harnesses/`](../validation/test_harnesses/), raw instrument logs (oscilloscope screen captures, logic analyzer traces, UART serial dumps) are not independently archived in `validation/logs/`.*
3. **Tier 3 — Pending Physical Laboratory Execution (7 Logical Tests):** Test protocols and verification procedures are documented in test result files, but physical execution on calibrated mechanical and optical fixtures remains pending.

> **Note on CI Compilation:** GitHub Actions ([`.github/workflows/compile_check.yml`](../.github/workflows/compile_check.yml)) automatically verifies syntactic and library build integrity for the two primary production firmware sketches: `subsystem1_secure_access.ino` (ESP32-S3) and `subsystem2_docking.ino` (ESP32). It does not compile or execute the standalone test sketches in `validation/test_harnesses/`.

---

## 2. Master Validation Status Matrix

| Category | Total Logical Tests | Tier 1: Host Model / DSP Harness | Tier 2: Documented Bench Result *(No Raw Logs)* | Tier 3: Pending Physical Lab Run | Reference Document |
|:---|:---:|:---:|:---:|:---:|:---|
| **HW — Hardware Electronics** | 7 | — | 7 Passed | — | [`hw_test_results.md`](../validation/results/hw_test_results.md) |
| **FW — Firmware Execution** | 5 | 2 Host Model PASS (`FW-02`, `FW-05`)* | 1 Passed (`FW-03`) | 2 Pending (`FW-01`, `FW-04`) | [`fw_test_results.md`](../validation/results/fw_test_results.md) |
| **SP — Signal Processing DSP** | 4 | 4 Passed (`SP-01`–`SP-04`) | — | — | [`signal_test_results.md`](../validation/results/signal_test_results.md) |
| **OPT — Optical & TinyML** | 4 | — | 2 Passed (`OPT-03`, `OPT-04`) | 2 Pending (`OPT-01`**, `OPT-02`) | [`optical_test_results.md`](../validation/results/optical_test_results.md) |
| **DK — Optical Docking** | 5 | — | 2 Passed (`DK-01`, `DK-05`) | 3 Pending (`DK-02`, `DK-03`, `DK-04`) | [`docking_test_results.md`](../validation/results/docking_test_results.md) |
| **SEC — 2FA Security Gate** | 5 | 5 Host Model PASS (`SEC-01`–`SEC-05`) | — | — | [`security_test_results.md`](../validation/results/security_test_results.md) |
| **Total Logical Tests** | **30** | **11 Host Model / DSP PASS (logical IDs)** | **12 Documented** | **7 Pending Lab Execution** | |

---

### Executable Scenario Mapping & Evidence Notes

* `*` **FW-05 / SEC-03 Scenario Overlap & Behavioral Model Limitation:** `FW-05` (low-confidence inference gate rejection) and `SEC-03` (ML low-confidence veto) evaluate the identical decision boundary (confidence $< 0.85$ denies access). In `test_auth_fsm.py`, this is executed as a single shared test scenario (`Test: SEC-03: ML Low Confidence Veto`). Similarly, `FW-02` role resolution is evaluated across the valid role pairings in `SEC-01a`, `SEC-01b`, and `SEC-01c`. *Crucially, the Python behavioral model passing demonstrates the correctness of the high-level decision rules, but does not establish that the production C++ firmware passes the same tests on physical silicon.*
* `**` **OPT-01 Subtest Aggregation:** Subcases `OPT-01a` ($0.50\text{ m}$, $98.4\%$) and `OPT-01b` ($1.00\text{ m}$, $94.2\%$) were verified in bench observations, but `OPT-01c` ($1.50\text{ m}$ max range limit) requires calibrated lab range verification. The parent logical test `OPT-01` is conservatively classified as Pending overall.
* **SP Suite Mains Flicker Scenario:** The 50 Hz AC mains lighting interference test is an automated sub-scenario within `test_goertzel.py` (Test Group 5), verifying mains ripple rejection across 10/20/30 Hz carriers; it is evaluated as part of the SP harness suite rather than a separate logical test ID.
* **Supplementary HAL Edge Case:** The ADC saturation threshold ($>3890$ counts) is implemented in `subsystem1_secure_access/src/hal/optical_sampler.cpp` and included in the test sketch `test_ss1_hal.ino` (Test 2). It represents a driver-level edge condition outside the 30 master logical tests and is unverified on physical hardware.

---

## 3. Results by Verification Domain

### 3.1 Signal Processing (SP-01 to SP-04) — Tier 1 (Host Python)
Verified via automated host harness [`validation/test_harnesses/test_goertzel.py`](../validation/test_harnesses/test_goertzel.py) (100% reproducible):
* **SP-01 (10 Hz Detection):** Target magnitude $M_{10} = 6.25 \times 10^8$ with zero spectral leakage on 20 Hz and 30 Hz bins.
* **SP-02 (20 Hz Detection):** Target magnitude $M_{20} = 6.25 \times 10^8$ with zero spectral leakage on 10 Hz and 30 Hz bins.
* **SP-03 (30 Hz Detection):** Target magnitude $M_{30} = 6.25 \times 10^8$ with zero spectral leakage on 10 Hz and 20 Hz bins.
* **SP-04 (Broadband Noise Rejection):** Random noise input yielded Dominance Ratio $1.44 < 2.0$, correctly rejected as ambiguous.
* *Sub-scenario:* 50 Hz mains flicker (Group 5) confirmed rejection of AC lighting ripple.

### 3.2 Security & 2FA Gate (SEC-01 to SEC-05, FW-02, FW-05) — Tier 1 (Host Python Model)
Verified via automated host harness [`validation/test_harnesses/test_auth_fsm.py`](../validation/test_harnesses/test_auth_fsm.py) (*Note: verifies the Python behavioral model; does not establish production C++ firmware execution*):
* **SEC-01 (Role Authorization / FW-02):** Valid 2FA pairings for Admin (30 Hz), Staff (20 Hz), and Guest (10 Hz) successfully asserted unlock and resolved roles (**Host Model PASS**).
* **SEC-02 (Unregistered Card):** Random 4-byte UIDs rejected immediately (`AUTH_REASON_UNKNOWN_CARD`), triggering a 2.0 s lockout (**Host Model PASS**).
* **SEC-03 / FW-05 (Low Confidence Veto):** ML confidence $0.75 < 0.85$ vetoed unlock (`AUTH_REASON_ML_LOW_CONFIDENCE`) (**Host Model PASS**).
* **SEC-04 (Frequency Mismatch Veto):** Valid Admin card presented with 10 Hz optical key vetoed unlock (`AUTH_REASON_GOERTZEL_MISMATCH`) (**Host Model PASS**).
* **SEC-05 (Ambiguous Spectrum Veto):** Dominance ratio $< 2.0$ vetoed unlock (`AUTH_REASON_GOERTZEL_AMBIGUOUS`) (**Host Model PASS**).

### 3.3 Hardware Electronics (HW-01 to HW-07) — Tier 2 (Documented Bench Measurements)
Documented in [`validation/results/hw_test_results.md`](../validation/results/hw_test_results.md) *(raw instrument logs not archived)*:
* **HW-01:** $+3.3\text{ V}$ logic rail measured at $3.31\text{ V}$ steady during concurrent PN532 polling and relay switching.
* **HW-02:** LM358 TIA maximum output swing capped at $2.95\text{ V}$ under saturation (below ESP32-S3 ADC limit).
* **HW-03:** Passive RC filter measured $1.2\text{ dB}$ attenuation at $30\text{ Hz}$ (HW-03a) and $14.8\text{ dB}$ attenuation at $100\text{ Hz}$ (HW-03b) ($f_c \approx 48.2\text{ Hz}$).
* **HW-04:** PN532 I2C communication sustained 100 consecutive reads without bus lockups.
* **HW-05:** Relay GPIO 4 assertion actuated 12V solenoid with clean $5.0\text{ s}$ auto-relock timeout.
* **HW-06:** Stepper 2048 CW + 2048 CCW rotation returned to home with zero net step offset.
* **HW-07:** Stepper motor switching induced $\le 2.1\text{ counts}$ maximum noise delta on photodiode ADC readings.

### 3.4 Firmware Execution (FW-01, FW-03, FW-04)
* **FW-03 — Tier 2:** 30-minute continuous ticking loop showed constant free heap ($324,112\text{ bytes}$) with zero memory leaks ([`fw_test_results.md`](../validation/results/fw_test_results.md)).
* **FW-01 — Tier 3:** Hardware `esp_timer` capture precision ($1.00032\text{ s}$, $+0.03\%$ error) documented in `fw_test_results.md`, pending physical oscilloscope/analyzer verification.
* **FW-04 — Tier 3:** PN532 I2C physical disconnect recovery ($50\text{ ms}$ timeout) pending physical fault injection test.

### 3.5 Optical Range & Docking Mechanics (OPT-01 to OPT-04, DK-01 to DK-05)
* **OPT-03 & OPT-04 — Tier 2:** Fluorescent lighting flicker and direct daylight correctly classified as `noise` ($>99\%$ confidence) in bench ambient testing ([`optical_test_results.md`](../validation/results/optical_test_results.md)).
* **DK-01 & DK-05 — Tier 2:** Coarse 128-point sweep executed in $8.19\text{ s}$, and zero-beacon condition correctly declared `ALIGN_STATUS_FAILED` ([`docking_test_results.md`](../validation/results/docking_test_results.md)).
* **OPT-01 — Tier 3:** Maximum range limit ($1.50\text{ m}$, subcase OPT-01c) pending calibrated optical bench measurement.
* **OPT-02 — Tier 3:** Angular tolerance ($\pm 30^\circ$ off-axis) pending calibrated rotary fixture measurement.
* **DK-02, DK-03, DK-04 — Tier 3:** Coarse peak accuracy ($\pm 5^\circ$), fine peak convergence ($\pm 1^\circ$), and backlash repeatability ($\pm 1$ step) pending calibrated angular jig measurement.
