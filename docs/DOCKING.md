# N.O.V.A. Subsystem 2 — Autonomous Optical Docking Algorithm

> **Document Status:** Official Docking Specification v1.0  
> **Source of Truth:** Design Freeze Specification §7, `docking/alignment_controller.cpp`, and `validation/results/docking_test_results.md`

---

## 1. Overview & Operational Concept

Subsystem 2 is a proof-of-concept autonomous optical alignment platform inspired by spacecraft docking systems. An ESP32 rotates a platform mounted with a BPW34 photodiode through a unipolar 28BYJ-48 stepper motor driven by a ULN2003 driver module, mapping incoming optical intensity across a $360^\circ$ sweep.

---

## 2. Two-Phase Coarse-Fine Scanning Algorithm

```text
                  START SCAN
                      │
                      ▼
┌───────────────────────────────────────────┐
│ Phase 1: Coarse 360° Sweep                │
│ - 128 Positions (2.8125° / 32 steps)      │
│ - 16x Oversampled ADC Read                │
│ - Record Max Intensity & Index            │
└─────────────────────┬─────────────────────┘
                      │
                      ▼
┌───────────────────────────────────────────┐
│ Intensity Threshold Check                 │
│ - Peak ADC >= 200 counts?                 │
└───────────┬───────────────────┬───────────┘
         YES│                 NO│
            ▼                   ▼
┌───────────────────────┐ ┌───────────────────────┐
│ Phase 2: Fine Sweep   │ │ Declare Failure       │
│ - Range: Peak +-22.5° │ │ ALIGN_STATUS_FAILED   │
│ - 1 Step Resolution   │ │ Motor Coils Released  │
│ - 32x ADC Oversample  │ └───────────────────────┘
│ - Record Max Angle    │
└───────────┬───────────┘
            │
            ▼
┌───────────────────────────────────────────┐
│ Backlash Compensation                     │
│ - Overshoot target CCW by 16 half-steps   │
│ - Final approach CW to peak position      │
└───────────┬───────────────────────────────┘
            │
            ▼
┌───────────────────────────────────────────┐
│ Motor Release (`stepper_release`)         │
│ - De-energize ULN2003 coils               │
│ - Gear friction holds platform angle      │
└───────────────────────────────────────────┘
```

---

## 3. Algorithm Specification & Parameters

| Parameter | Specification | Value |
|:---|:---|:---|
| Total Resolution | Half-Step Mode | $4096\text{ half-steps / rev}$ ($0.08789^\circ / \text{step}$) |
| Coarse Sweep Points | 128 Points | 32 steps between points ($2.8125^\circ$ spacing) |
| Coarse Oversampling | 16 Samples | Averaged ADC reading per coarse point |
| Fine Sweep Window | $\pm 8$ Coarse Points | $\pm 256\text{ half-steps}$ ($\pm 22.5^\circ$ around peak) |
| Fine Oversampling | 32 Samples | Averaged ADC reading per fine step |
| Backlash Compensation | $16\text{ half-steps}$ | $1.406^\circ$ CCW overshoot before final CW alignment |
| Minimum Threshold | 200 ADC Counts | Minimum light intensity required for valid beacon |

---

## 4. Power & Thermal Optimization (`stepper_release`)

Holding a unipolar stepper motor energized at rest draws over $350\text{ mA}$, causing rapid heating of the ULN2003 Darlington transistor array. 

Once alignment is completed, `alignment_runFullScan()` executes `stepper_release()`, setting all 4 control pins (IN1--IN4) to `LOW`. The internal 1:64 reduction gearbox of the 28BYJ-48 motor provides mechanical holding torque, holding platform orientation while reducing idle power consumption to **0 mW**.
