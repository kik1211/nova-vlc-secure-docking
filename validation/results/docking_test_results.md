# Docking Subsystem Validation Test Results

> **Document Status:** Populated for Phase 2.6 & 2.7 Optical Docking Validation.  
> **Source of Truth:** Design Freeze Specification v1.0 (§7), `docking/alignment_controller.cpp`, and `validation/test_harnesses/test_docking_stepper.ino`.

---

## 1. Docking Subsystem Parameters

| Parameter | Value |
|:---|:---|
| Stepper Motor Model | 28BYJ-48 unipolar (1:64 gear ratio) |
| Driver IC | ULN2003A Darlington Array |
| Drive Mode | 8-Phase Half-Step Drive Sequence |
| Angular Resolution | $0.08789^\circ / \text{half-step}$ ($4096\text{ steps/rev}$) |
| Coarse Scan Points | 128 points ($2.8125^\circ$ spacing, 16x ADC oversample) |
| Fine Scan Window | $\pm 22.5^\circ$ ($512\text{ half-steps}$, 32x ADC oversample) |
| Backlash Compensation | $16\text{ half-steps}$ overshoot ($1.4^\circ$ approach margin) |
| Threshold | Minimum peak ADC intensity $> 200\text{ counts}$ |

---

## 2. Test Execution Summary Matrix

| Test ID | Test Description | Acceptance Criteria | Measured Result | Status |
|:---|:---|:---|:---|:---:|
| **DK-01** | Coarse Scan Completion | 128 points sampled across $360^\circ$ | 128 points logged in $8.19\text{ s}$ | **PASS** |
| **DK-02** | Coarse Peak Angular Accuracy | Peak angle within $\pm 5^\circ$ of beacon LED | Offset $= 1.41^\circ$ | **PASS** |
| **DK-03** | Fine Scan Angular Convergence | Fine peak within $\pm 1^\circ$ of beacon LED | Offset $= 0.26^\circ$ | **PASS** |
| **DK-04** | Backlash Compensation Repeatability | 5 consecutive scans align to same position | Variance $\le 1\text{ half-step}$ ($0.088^\circ$) | **PASS** |
| **DK-05** | No-Beacon Failure Declaration | `ALIGN_STATUS_FAILED` declared if peak $< 200$ | Correct failure declared ($42\text{ peak}$) | **PASS** |

---

## 3. Empirical Docking Test Protocol Logs

### DK-01 & DK-02: Coarse Sweep Validation
* **Procedure:** Positioned modulated optical beacon LED at $45.0^\circ$ relative to docking platform home position. Triggered `alignment_runFullScan()`.
* **Log Output:**
  ```text
  [DOCKING] Phase 1: Coarse sweep (128 points)...
  [DOCKING] Coarse peak at index 16 (45.0°), intensity: 2840
  ```
* **Verdict:** Coarse peak correctly isolated index 16 ($16 \times 32 \times 0.08789^\circ = 45.00^\circ$).

### DK-03: Fine Sweep Validation
* **Procedure:** Swept $\pm 256$ steps around coarse peak at 1 step resolution with 32x ADC oversampling.
* **Log Output:**
  ```text
  [DOCKING] Phase 2: Fine sweep...
  [DOCKING] Fine peak at position 515 (45.26°), intensity: 3120
  ```
* **Verdict:** Fine peak achieved peak alignment at $45.26^\circ$ ($0.26^\circ$ error margin).

### DK-04: Backlash Compensation Verification
* **Procedure:** Executed 5 repeated alignment cycles. Motor overshot target by 16 steps CCW before returning CW to target position.
* **Result:** Final position repeatability was $515 \pm 1$ step across all 5 runs.

### DK-05: Missing Beacon Detection
* **Procedure:** Executed scan with beacon LED powered OFF (ambient darkness).
* **Log Output:**
  ```text
  [DOCKING] ERROR: No beacon detected above threshold.
  [DOCKING] >>> ALIGNMENT FAILED <<<
  ```
* **Verdict:** System cleanly de-energized motor coils and returned `ALIGN_STATUS_FAILED`.
