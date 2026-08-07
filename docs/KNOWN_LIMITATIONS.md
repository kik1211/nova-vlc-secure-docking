# N.O.V.A. Known Limitations & Engineering Trade-Offs

> **Document Status:** Official Limitations & Trade-offs Document v1.0  
> **Source of Truth:** Design Freeze Specification §9 and Implementation Specification §9

---

## 1. Subsystem 1 (Secure Access) Limitations

### 1. Optical Transmission Range & Alignment
* **Maximum Line-of-Sight Range:** Tested up to $1.5\text{ meters}$ in indoor ambient lighting. Range degrades beyond $1.5\text{ m}$ due to free-space optical path loss ($1/r^2$).
* **Angular Sensitivity:** Off-axis alignment beyond $\pm 30^\circ$ reduces optical flux below the BPW34 photodiode noise floor.

### 2. Static Credential Memory
* Registered NFC UIDs are hardcoded in `config.h` for prototype demonstration purposes. Physical deployment requires dynamic NVS storage.

### 3. Sampling Window Latency
* The optical signal capture requires a $1.000\text{ second}$ sample window ($100\text{ samples} @ 100\text{ Hz}$), resulting in a fixed $1.0\text{ s}$ sensing delay after NFC card detection.

---

## 2. Subsystem 2 (Autonomous Docking) Limitations

### 1. Unipolar Stepper Gear Backlash
* The 28BYJ-48 unipolar stepper motor exhibits internal gear backlash ($\approx 1.4^\circ$). Compensated via a 16-step counter-clockwise overshoot during alignment.

### 2. Open-Loop Step Position Tracking
* Subsystem 2 tracks angular position incrementally without absolute encoder feedback. On power-up, position 0 is assumed at current platform orientation.

---

## 3. Physical & Environmental Constraints

* **Direct Sunlight Exposure:** Unfiltered direct sunlight ($>50,000\text{ lux}$) saturates the BPW34 photodiode, triggering the ADC saturation safety lock until sunlight is removed.
* **Component Availability Trade-off:** Standard LM358 op-amp selected for universal availability in India despite higher input offset voltage compared to precision TIA ICs (e.g., OPA380).
