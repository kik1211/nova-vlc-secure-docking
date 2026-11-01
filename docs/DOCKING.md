# N.O.V.A. Subsystem 2 — Autonomous Optical Docking Algorithm

## 1. Overview & Operational Concept

Subsystem 2 is an autonomous optical alignment platform. An ESP32 rotates a mounting platform carrying a BPW34 PIN photodiode using a unipolar 28BYJ-48 stepper motor driven by a ULN2003A Darlington driver array. The system maps incoming optical intensity across a $360^\circ$ sweep to find the heading of maximum optical power.

The alignment logic is implemented in [`subsystem2_docking/src/docking/alignment_controller.cpp`](../subsystem2_docking/src/docking/alignment_controller.cpp) and configured via [`subsystem2_docking/config.h`](../subsystem2_docking/config.h).

---

## 2. Two-Phase Scanning Algorithm

```text
                  START SCAN
                      │
                      ▼
┌───────────────────────────────────────────┐
│ Phase 1: Coarse 360° Sweep                │
│ - 128 Points (2.8125° / 32 half-steps)    │
│ - 16x Oversampled ADC Read per point      │
│ - Record Max Intensity & Peak Position    │
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
│ - 1 Half-Step Res     │ │ Motor Coils Released  │
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
│ - De-energize ULN2003 coils (0 mW idle)   │
│ - Gear friction holds platform angle      │
└───────────────────────────────────────────┘
```

---

## 3. Algorithm Specifications & Parameters

| Parameter | Specification | Engineering Value |
|:---|:---|:---|
| **Motor Drive Mode** | 8-Phase Half-Step Sequence | $4096\text{ half-steps / rev}$ ($0.08789^\circ / \text{step}$) |
| **Coarse Sweep Points** | 128 Discrete Positions | 32 half-steps between points ($2.8125^\circ$ spacing) |
| **Coarse Oversampling** | 16 Samples | Averaged ADC reading per coarse position |
| **Fine Sweep Window** | $\pm 8$ Coarse Points | $\pm 256\text{ half-steps}$ ($\pm 22.5^\circ$ around coarse peak) |
| **Fine Resolution** | 1 Half-Step | Single-step angular resolution ($0.08789^\circ$) |
| **Fine Oversampling** | 32 Samples | Averaged ADC reading per fine step |
| **Backlash Compensation** | 16 Half-Steps | $1.406^\circ$ CCW overshoot before final CW alignment |
| **Detection Threshold** | 200 ADC Counts | Minimum light intensity required to validate beacon presence |
| **Settling Delay** | 5 ms | Transient settling time between motor stop and ADC read |

---

## 4. Backlash Compensation & Thermal Management

### 4.1 Mechanical Backlash Mitigation
The 28BYJ-48 stepper motor's internal 1:64 reduction gearbox exhibits $\approx 1.4^\circ$ of mechanical gear play. To prevent hysteresis error when reversing direction after the fine scan:
1. The platform rotates counter-clockwise (CCW) past the fine peak by 16 half-steps ($1.406^\circ$).
2. The platform rotates clockwise (CW) back into the final peak position.
This ensures all gear teeth are engaged in the forward drive direction, yielding a repeatable final positioning accuracy of $\le \pm 1$ step.

### 4.2 Thermal Management (`stepper_release`)
Holding unipolar stepper motor coils continuously energized draws $>350\text{ mA}$, causing thermal dissipation in the ULN2003 driver. 

Upon completing alignment, `alignment_runFullScan()` calls `stepper_release()`, setting all four driver pins (IN1--IN4) to `LOW`. The 1:64 gearbox reduction provides sufficient holding torque to prevent platform drift, dropping idle power consumption to **0 mW**.
