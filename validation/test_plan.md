# Validation Test Plan

> Master test plan for N.O.V.A. — references Design Freeze §11 and
> Implementation Specification §11.

<!-- See docs/VALIDATION_RESULTS.md for actual measured results. -->
<!-- See validation/results/ for per-category detail files. -->
<!-- See validation/logs/ for raw UART serial captures. -->
<!-- See validation/test_harnesses/ for test sketch .ino files. -->

---

## Test Categories

| Category | Test IDs | Count |
|:---|:---|:---:|
| Hardware Validation | HW-01 through HW-07 | 7 |
| Firmware Validation | FW-01 through FW-05 | 5 |
| Signal Processing | SP-01 through SP-05 | 5 |
| Optical Tests | OPT-01 through OPT-04 | 4 |
| Docking Tests | DK-01 through DK-05 | 5 |
| Security Tests | SEC-01 through SEC-04 | 4 |
| **Total** | | **30** |

---

## Test Execution Protocol

1. Record hardware state, firmware version, and ambient conditions.
2. Execute test procedure as documented in each result file.
3. Record measured values — never estimated values.
4. Mark PASS, FAIL, or SKIP with justification.
5. Commit result to `validation/results/` and raw log to `validation/logs/`.

---

## Negative Test Cases & Edge Conditions

The validation suite explicitly includes negative testing scenarios to verify veto logic and fault recovery:

| Test ID | Scenario Description | Expected Outcome & Veto Logic | Validation Status |
|:---|:---|:---|:---:|
| **SEC-02** | Unregistered NFC Card UID | Immediate lockout (`AUTH_REASON_UNKNOWN_CARD`), FSM enters $2.0\text{ s}$ delay | **Harness PASS** |
| **SEC-03** | Low ML Confidence ($<85\%$) | Vetoes unlock (`AUTH_REASON_ML_LOW_CONFIDENCE`), door remains LOCKED | **Harness PASS** |
| **SEC-04** | Goertzel Frequency Mismatch | Vetoes unlock (`AUTH_REASON_GOERTZEL_MISMATCH`), door remains LOCKED | **Harness PASS** |
| **SEC-05** | Ambiguous Optical Spectrum (Ratio $<2.0$) | Vetoes unlock (`AUTH_REASON_GOERTZEL_AMBIGUOUS`), door remains LOCKED | **Harness PASS** |
| **HW-05** | ADC Saturation ($>3890$ counts) | Resets optical capture buffer, reports saturation warning | **Simulation PASS** |
| **SP-06** | 50 Hz AC Mains Lighting Flicker | Filters out 50 Hz mains ripple, detects dominant $10\text{ Hz}/20\text{ Hz}/30\text{ Hz}$ key | **Harness PASS** |

---

## Regression Testing Strategy & Automated Test Suite

To prevent regressions during firmware updates or configuration tuning, automated mathematical verification scripts are executed before every release:

### 1. Automated Execution Commands
```bash
# Execute Goertzel DSP mathematical verification suite
python validation/test_harnesses/test_goertzel.py

# Execute FSM authentication state machine & 2FA gate verification suite
python validation/test_harnesses/test_auth_fsm.py
```

### 2. Regression Pass Criteria
* **Goertzel DSP Suite:** Must pass $100\%$ of ideal sine wave tests, square wave optical vectors, frequency offset tolerance ($\pm 2.5\%$), DC bias offsets, 50 Hz lighting flicker rejection, and low SNR broadband noise tests.
* **Auth FSM Suite:** Must pass $100\%$ of valid role 2FA combinations and 5 negative security veto cases (`SEC-02` through `SEC-05`).
* **Release Gate Requirement:** $100\%$ pass status is required prior to tagging releases or updating main branches.
