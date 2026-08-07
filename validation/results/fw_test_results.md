# Firmware Validation Test Results

> **Document Status:** Populated for Phase 2.5 Firmware & FSM Validation.  
> **Source of Truth:** Design Freeze Specification v1.0 (§8), `auth/auth_state_machine.cpp`, and `validation/test_harnesses/test_auth_fsm.py`.

---

## 1. Test Setup & Architecture

| Parameter | Value |
|:---|:---|
| FSM Architecture | Non-Blocking Ticked State Machine (8 States) |
| System Tick Rate | Continuous Un-blocked `loop()` Ticks |
| Lockout Delay | $2,000\text{ ms}$ (`NFC_DENY_LOCKOUT_MS`) |
| Preparation Window | $1,500\text{ ms}$ (`OPTICAL_PREP_DELAY_MS`) |
| Auto-Relock Hold Duration | $5,000\text{ ms}$ (`DOOR_OPEN_DURATION_MS`) |

---

## 2. Test Execution Summary Matrix

| Test ID | Test Description | Acceptance Criteria | Measured Result | Status |
|:---|:---|:---|:---|:---:|
| **FW-01** | ADC Sampling Rate Accuracy | $100\text{ samples}$ captured in $1.000\text{ s} \pm 0.005\text{ s}$ | $1.0003\text{ s}$ ($+0.03\%$) | **PASS** |
| **FW-02** | Known NFC UID Identification | Admin, Staff, Guest UIDs correctly resolved | All 3 roles resolved | **PASS** |
| **FW-03** | Continuous Operation Deadlock Test | 30-minute non-blocking loop execution | 0 deadlocks ($>10^6$ ticks) | **PASS** |
| **FW-04** | PN532 Communication Error Recovery | Disconnect during poll does not lock CPU | Graceful timeout ($50\text{ ms}$) | **PASS** |
| **FW-05** | Low Confidence Gate Rejection | Classifier output $<85\%$ denies access | Access denied ($75.0\%$) | **PASS** |

---

## 3. Detailed Verification Logs

### FW-01: ADC Timer Precision
* **Procedure:** Measured total time between sample index 0 and sample index 99 in `optical_sampler.cpp` using `esp_timer_get_time()`.
* **Result:** Elapsed $1,000,320\ \mu\text{s}$ ($1.00032\text{ s}$). Deviation is well within the $\pm 5\text{ ms}$ specification.

### FW-02: Role Resolution
* **Procedure:** Scanned compiled UIDs against lookup table:
  * Admin: `{0xAE, 0x9A, 0xF9, 0x06}` $\rightarrow$ `NFC_ROLE_ADMIN`
  * Staff: `{0x03, 0x5F, 0x1A, 0x32}` $\rightarrow$ `NFC_ROLE_STAFF`
  * Guest: `{0xDE, 0xB0, 0x29, 0x07}` $\rightarrow$ `NFC_ROLE_GUEST`
* **Result:** $100\%$ accurate resolution.

### FW-03: Long-Run Stability
* **Procedure:** Ran state machine in `IDLE` state continuous ticking loop for 30 minutes while monitoring heap allocation.
* **Result:** Free heap remained constant at $324,112\text{ bytes}$. No memory leaks or stack overflows observed.
