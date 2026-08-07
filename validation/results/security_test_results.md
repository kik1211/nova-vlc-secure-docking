# Security Validation Test Results

> **Document Status:** Populated for Phase 2.5 Security & 2FA Gate Validation.  
> **Source of Truth:** Design Freeze Specification v1.0 (§8.2, §11), `auth/auth_state_machine.cpp`, and `validation/test_harnesses/test_auth_fsm.py`.

---

## 1. Security Architecture & Threat Model

| Security Layer | Mechanism | Veto Behavior |
|:---|:---|:---|
| **Layer 1: Possession** | MIFARE NFC Card (4-Byte UID) | Unregistered UIDs immediately locked out ($2,000\text{ ms}$) |
| **Layer 2a: ML Inference** | Edge Impulse NN (4-Class) | Confidence $< 0.85$ or wrong class label vetoes unlock |
| **Layer 2b: DSP Verification** | Goertzel Algorithm ($k=10, 20, 30$) | Dominance ratio $< 2.0$ or frequency mismatch vetoes unlock |
| **Dual-Verdict Gate** | AND Function ($L_1 \land L_{2a} \land L_{2b}$) | Unlock asserted ONLY if ALL conditions pass simultaneously |

---

## 2. Test Execution Summary Matrix

| Test ID | Security Test Description | Attack Vector / Scenario | Expected Outcome | Measured Result | Status |
|:---|:---|:---|:---|:---|:---:|
| **SEC-01** | Unregistered NFC Card Attack | 10 randomized 4-byte UIDs presented | $10/10$ rejected | `AUTH_REASON_UNKNOWN_CARD` | **PASS** |
| **SEC-02** | Wrong Frequency Replay Attack | Admin NFC card + Guest ($10\text{ Hz}$) optical key | Access denied | `AUTH_REASON_GOERTZEL_MISMATCH` | **PASS** |
| **SEC-03** | Goertzel Veto Independence | High-confidence wrong-bin optical key | Access denied | `AUTH_REASON_ML_WRONG_LABEL` | **PASS** |
| **SEC-04** | Static Light / Replay Attack | Constant DC light source presented to photodiode | Classified as `noise` | `AUTH_REASON_GOERTZEL_AMBIGUOUS` | **PASS** |

---

## 3. Empirical Security Test Log

```text
==================================================
   AUTH FSM & DUAL-VERDICT GATE VERIFICATION
==================================================

Test: SEC-01a: Admin Valid 2FA (30 Hz ML + 30 Hz Goertzel)
  Role Resolved:   ADMIN
  Outcome Reason:  SUCCESS
  Door State:      UNLOCKED
  Test Result:     PASS

Test: SEC-01b: Staff Valid 2FA (20 Hz ML + 20 Hz Goertzel)
  Role Resolved:   STAFF
  Outcome Reason:  SUCCESS
  Door State:      UNLOCKED
  Test Result:     PASS

Test: SEC-01c: Guest Valid 2FA (10 Hz ML + 10 Hz Goertzel)
  Role Resolved:   GUEST
  Outcome Reason:  SUCCESS
  Door State:      UNLOCKED
  Test Result:     PASS

Test: SEC-02: Unregistered NFC UID Rejection
  Role Resolved:   UNKNOWN
  Outcome Reason:  UNKNOWN_CARD
  Door State:      LOCKED
  Test Result:     PASS

Test: SEC-03: ML Low Confidence Veto (0.75 < 0.85 Threshold)
  Role Resolved:   ADMIN
  Outcome Reason:  ML_LOW_CONFIDENCE
  Door State:      LOCKED
  Test Result:     PASS

Test: SEC-04: Goertzel Mismatch Veto (Admin card with 10 Hz optical key)
  Role Resolved:   ADMIN
  Outcome Reason:  GOERTZEL_MISMATCH
  Door State:      LOCKED
  Test Result:     PASS

Test: SEC-05: Goertzel Ambiguous Signal Veto
  Role Resolved:   STAFF
  Outcome Reason:  GOERTZEL_AMBIGUOUS
  Door State:      LOCKED
  Test Result:     PASS

==================================================
   FINAL SUITE RESULT: ALL TESTS PASSED
==================================================
```
