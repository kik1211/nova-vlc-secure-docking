# N.O.V.A. Validation Results & Status Matrix

> **Document Status:** Official Validation Summary  
> **Source of Truth:** Design Freeze Specification §11, `validation/results/*.md`, and `validation/test_harnesses/`

---

## 1. Engineering State Definitions

To maintain absolute engineering integrity, every test case is categorized into one of four distinct states:

* **Implemented:** Module code and drivers authored and compiling cleanly in CI.
* **Documented:** Test protocols, mathematical derivations, and expected criteria written.
* **Harness / Simulation Verified:** Verified via automated host Python test suite (`test_goertzel.py`, `test_auth_fsm.py`) or compiled Arduino verification harness.
* **Pending Physical Hardware Test / Not Yet Validated:** Requires physical lab bench measurement on actual target microcontrollers and hardware components.

---

## 2. Master Validation Summary Table

| Category | Total Tests | Harness / Simulation Verified | Pending Physical Hardware Test | Document Status |
|:---|:---:|:---:|:---:|:---|
| **HW — Hardware Tests** | 7 | 7 | 0 | Documented (`hw_test_results.md`) |
| **FW — Firmware Tests** | 5 | 3 | 2 | Documented (`fw_test_results.md`) |
| **SP — Signal Processing** | 4 | 4 | 0 | **Harness Verified** (`test_goertzel.py`) |
| **OPT — Optical / TinyML** | 4 | 2 | 2 | Documented (`optical_test_results.md`)|
| **DK — Optical Docking** | 5 | 2 | 3 | Documented (`docking_test_results.md`)|
| **SEC — 2FA Security** | 5 | 5 | 0 | **Harness Verified** (`test_auth_fsm.py`) |
| **Total** | **30** | **23** | **7** | **100% Documented** |

---

## 3. Detailed Validation Results by Category

### Signal Processing (SP-01 to SP-04) — Verified via `test_goertzel.py`
* **SP-01 (10 Hz Detection):** **Harness Verified** ($M_{10} = 6.25 \times 10^8$, zero leakage on $20/30\text{ Hz}$).
* **SP-02 (20 Hz Detection):** **Harness Verified** ($M_{20} = 6.25 \times 10^8$, zero leakage on $10/30\text{ Hz}$).
* **SP-03 (30 Hz Detection):** **Harness Verified** ($M_{30} = 6.25 \times 10^8$, zero leakage on $10/20\text{ Hz}$).
* **SP-04 (Broadband Noise Rejection):** **Harness Verified** (Dominance ratio $= 1.44 < 2.0$, correctly rejected).

### Security & 2FA Gate (SEC-01 to SEC-05) — Verified via `test_auth_fsm.py`
* **SEC-01 (Admin/Staff/Guest Valid 2FA):** **Harness Verified** (All 3 roles authorized).
* **SEC-02 (Unregistered NFC UID):** **Harness Verified** (`AUTH_REASON_UNKNOWN_CARD` locked out).
* **SEC-03 (Low Confidence ML Veto):** **Harness Verified** (Confidence $75.0\% < 85.0\%$ denied).
* **SEC-04 (Goertzel Mismatch Veto):** **Harness Verified** (Admin card + $10\text{ Hz}$ optical key denied).
* **SEC-05 (Ambiguous Noise Veto):** **Harness Verified** (`GOERTZEL_AMBIGUOUS` denied).

### Physical Hardware & Bench Tests (HW, FW, OPT, DK)
* **HW-01 to HW-07:** Documented in `hw_test_results.md` — all 7 tests record PASS with measured values (bench-verified on prototype hardware).
* **FW-01 to FW-05:** Partial Harness Verified / Pending Physical Hardware Test (Protocols in `fw_test_results.md`).
* **OPT-01 to OPT-04:** Partial Harness Verified / Pending Physical Hardware Test (Protocols in `optical_test_results.md`).
* **DK-01 to DK-05:** Partial Harness Verified / Pending Physical Hardware Test (Protocols in `docking_test_results.md`).
