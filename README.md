# N.O.V.A. — Navigational Optical Verification & Authentication

[![Compile Check](https://github.com/kik1211/nova-vlc-secure-docking/actions/workflows/compile_check.yml/badge.svg)](https://github.com/kik1211/nova-vlc-secure-docking/actions/workflows/compile_check.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-ESP32--S3%20%7C%20ESP32-blue.svg)](docs/HARDWARE.md)
[![Status](https://img.shields.io/badge/Status-v1.0.0--Prototype-success.svg)](CHANGELOG.md)

> An embedded engineering research prototype demonstrating physical access security and autonomous optical docking using Visible Light Communication (VLC), Near Field Communication (NFC), and Edge Impulse TinyML.  
> **Awarded First Prize at College Project Expo.**

### Key Engineering Benchmarks & Design Specifications
| Performance Metric | Design Target / Verification Status | Significance & Engineering Impact |
|:---|:---:|:---|
| **ADC Sampling Target** | $100.0\text{ Hz}$ Target ($T_s = 10.000\text{ ms}$) | Hardware timer `esp_timer` ISR minimizes software sampling jitter |
| **Goertzel DSP Latency** | Measured $< 0.8\text{ ms}$ ($k=10, 20, 30$) | Algorithmic $O(NK)$ complexity for deterministic spectral verification |
| **TinyML Inference Latency** | Measured $\approx 2.14\text{ ms}$ ($2,140\ \mu\text{s}$) | Vector-accelerated spectral feature extraction + Dense MLP inference on Xtensa LX7 |
| **Model Size & Memory Footprint** | $534\text{ parameters}$ ($2.1\text{ KB}$ weights, $2.9\text{ KB}$ arena) | Lightweight footprint executing within internal SRAM |
| **Angular Scanning Resolution** | $0.08789^\circ / \text{step}$ ($4096\text{ half-steps/rev}$) | Angular step resolution of 1:64 geared unipolar stepper in 8-phase drive mode |
| **Security Verification (Harness)** | 0 False Unlocks in Test Suite | Dual-verdict gate (NFC verified ∧ ML confidence ≥ 85% ∧ Goertzel match) enforces independent confirmation |

---

## 1. Project Background & Engineering Motivation

Physical access control systems rely heavily on Radio Frequency (RF) technologies like RFID, Bluetooth, or Wi-Fi. However, RF signals penetrate walls and are susceptible to remote eavesdropping, relay attacks, and signal jamming. **N.O.V.A.** (Navigational Optical Verification & Authentication) was developed to demonstrate a localized physical alternative by pairing **Near Field Communication (NFC)** with directional **Visible Light Communication (VLC)**.

Key engineering milestones include:
1. **Confining Transmission Boundaries:** Modulated optical signals require direct line-of-sight propagation, bounding access credentials within physical sight and eliminating RF eavesdropping.
2. **Dual-Verdict Verification Pipeline:** To prevent optical flicker or ambient light from causing false unlocks, a dual-verdict pipeline runs on the ESP32-S3. An **Edge Impulse TinyML model** executes in parallel with a deterministic **Goertzel DFT algorithm**—access is granted only when both algorithms independently confirm the optical key matches the NFC cardholder's role.
3. **Autonomous Docking Platform:** To address optical transceiver alignment in autonomous environments (e.g., robotic docking stations), Subsystem 2 provides autonomous angular alignment driven by a 28BYJ-48 stepper motor.
4. **Recognition:** Awarded **First Prize at the College Project Expo** for embedded architecture rigor, signal processing implementation, and edge machine learning integration.

---

## 2. Visual Overview & Signal Flow

### 2.1 Overall System Architecture
The prototype comprises two independent standalone hardware subsystems designed with complete electrical isolation:

```mermaid
graph TD
    subgraph SS1 ["Subsystem 1: Secure Access Control (ESP32-S3)"]
        A[MIFARE NFC Card] -->|13.56 MHz RFID| B[PN532 Reader I2C]
        B -->|Resolved Role| C[Auth State Machine FSM]
        D[Modulated Optical LED] -->|Free-Space Light| E[BPW34 PIN Photodiode]
        E -->|Photocurrent I_pd| F[LM358 Op-Amp TIA Gain=100k]
        F -->|Analog Voltage V_out| G[RC Filter fc=48.2 Hz]
        G -->|GPIO 1 ADC1_CH0| H[100 Hz esp_timer ISR]
        H -->|100 Float Samples| I[Dual-Verdict Verification]
        I -->|Spectral Features| J[Edge Impulse Classifier]
        I -->|Raw Buffer| K[Goertzel DFT k=10,20,30]
        J -->|ML Verdict >= 85%| L{Dual-Verdict 2FA Gate}
        K -->|DSP Dominance Ratio >= 2.0| L
        C --> L
        L -->|BOTH VERDICTS PASS| M[5V Relay Module]
        M -->|12V DC Pulse| N[Fail-Secure Solenoid Lock]
    end

    subgraph SS2 ["Subsystem 2: Autonomous Optical Docking (ESP32)"]
        O[Beacon Light LED] -->|Free-Space Light| P[Docking BPW34 Sensor]
        P -->|ADC Read GPIO 34| Q[Alignment Controller]
        Q -->|8-Phase Half-Step| R[ULN2003 Driver + 28BYJ-48 Stepper]
        R -->|Rotate Platform 360 deg| P
    end
```

### 2.2 Analog Front-End Signal Flow
```mermaid
graph LR
    LED[LED Beacon 10/20/30 Hz] -->|Modulated Light| PD[BPW34 Photodiode]
    PD -->|Photocurrent I_pd| TIA[LM358 TIA Rf=100k]
    TIA -->|Voltage V_out| LPF[RC Low-Pass Filter fc=48.2Hz]
    LPF -->|Filtered Signal 0-1.8V| ADC[ESP32-S3 ADC GPIO 1]
    ADC -->|100Hz Sampling| BUF[100-Sample Buffer]
    BUF --> DSP[Goertzel DFT]
    BUF --> ML[TinyML Classifier]
    DSP --> GATE{Authentication FSM}
    ML --> GATE
    GATE -->|GRANT| RELAY[5V Relay] --> LOCK[12V Solenoid]
```

---

## 3. Hardware Bill of Materials (BOM) & Pinout Table

### Primary Pinout Mapping
| Subsystem | Microcontroller | Component | GPIO Pin | Function | Logic Level |
|:---|:---|:---|:---:|:---|:---|
| **Subsystem 1** | ESP32-S3 DevKit-C | BPW34 + LM358 TIA | **GPIO 1** | ADC1_CH0 Optical Signal Input | $0.0\text{--}3.1\text{ V}$ Analog |
| **Subsystem 1** | ESP32-S3 DevKit-C | 5V Relay Module | **GPIO 4** | Solenoid Unlock Assert | $3.3\text{ V}$ Active HIGH |
| **Subsystem 1** | ESP32-S3 DevKit-C | PN532 NFC Module | **GPIO 8 / 9** | I2C SDA / SCL | $3.3\text{ V}$ I2C Open-Drain |
| **Subsystem 2** | ESP32 DevKit-V1 | ULN2003 IN1--IN4 | **GPIO 13, 12, 14, 27** | Stepper Motor Coils A, B, C, D | $3.3\text{ V}$ Digital |
| **Subsystem 2** | ESP32 DevKit-V1 | BPW34 Photodiode | **GPIO 34** | Docking Sensor ADC Input | $0.0\text{--}3.3\text{ V}$ Analog |

Detailed component specifications and schematics: [`docs/HARDWARE.md`](docs/HARDWARE.md) and [`hardware/bom/`](hardware/bom/).

---

## 4. Building & Flashing Instructions

### Prerequisites
* Arduino IDE 2.x or `arduino-cli`
* ESP32 Board Package (`esp32` by Espressif Systems v2.0.11+)
* Adafruit PN532 Library (`arduino-cli lib install "Adafruit PN532"`)

### Compiling via Arduino CLI
```bash
# 1. Compile Subsystem 1 (ESP32-S3)
arduino-cli compile --fqbn esp32:esp32:esp32s3 subsystem1_secure_access/subsystem1_secure_access.ino

# 2. Compile Subsystem 2 (ESP32)
arduino-cli compile --fqbn esp32:esp32:esp32 subsystem2_docking/subsystem2_docking.ino
```

### Running Test Verification Suite
```bash
# Run Goertzel DSP mathematical verification suite
python validation/test_harnesses/test_goertzel.py

# Run Authentication FSM & 2FA gate verification suite
python validation/test_harnesses/test_auth_fsm.py
```

---

## 5. Documentation Reference Index

For full technical specifications, see [`docs/README.md`](docs/README.md):

| Document | Purpose & Key Topics | Technical Focus |
|:---|:---|:---|
| [`docs/README.md`](docs/README.md) | Technical Documentation Index | Navigation hub for engineering documentation |
| [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md) | System architecture, subsystem boundaries, interface specs, and ADRs (ADR-001 through ADR-008) | System design & design decisions |
| [`docs/HARDWARE.md`](docs/HARDWARE.md) | Component rationales, electrical ratings, BOM, schematics, and wiring guides | Electrical & circuit design |
| [`docs/SIGNAL_PROCESSING.md`](docs/SIGNAL_PROCESSING.md) | 100 Hz sampling, Goertzel derivation, integer-bin alignment, and leakage analysis | DSP mathematics & filter design |
| [`docs/TINYML.md`](docs/TINYML.md) | Spectral feature extraction (13 features), 534-parameter Dense MLP, memory footprint, and wrapper API | Edge machine learning deployment |
| [`docs/DOCKING.md`](docs/DOCKING.md) | Two-phase coarse/fine scanning, 28BYJ-48 half-step kinematics, backlash math, and coil power management | Embedded motion control |
| [`docs/SECURITY_MODEL.md`](docs/SECURITY_MODEL.md) | Threat model, attack countermeasure matrix, 2FA gate logic, and state machine transitions | Embedded physical security ([`subsystem1_secure_access/src/auth/`](subsystem1_secure_access/src/auth/)) |
| [`docs/VALIDATION_RESULTS.md`](docs/VALIDATION_RESULTS.md) | 30-test verification matrix with strict separation of CI builds, test harnesses, and physical bench tests | Verification & test methodology |
| [`docs/KNOWN_LIMITATIONS.md`](docs/KNOWN_LIMITATIONS.md) | Measured operating bounds, optical link budget, gear backlash, and planned engineering improvements | Trade-offs & technical roadmap |
| [`docs/TROUBLESHOOTING.md`](docs/TROUBLESHOOTING.md) | Hardware and firmware diagnostic checklists, bench test points, and fault recovery procedures | Lab diagnostics & debugging |

---

## 6. Repository Topics

`embedded-systems` • `esp32` • `esp32-s3` • `visible-light-communication` • `vlc` • `tinyml` • `edge-impulse` • `goertzel` • `nfc` • `signal-processing` • `autonomous-docking` • `embedded-security` • `electronics` • `iot` • `cpp` • `arduino`

---

## 7. Frequently Asked Questions (FAQ)

#### Q1: Why combine the Goertzel DSP algorithm with a TinyML classifier?
**Answer:** Neural network classifiers are probabilistic and can misclassify under unexpected optical waveforms or noise. The Goertzel algorithm provides a fast ($<0.8\text{ ms}$), deterministic mathematical check for target frequency magnitudes. Combining both in a **Dual-Verdict Gate** ensures access is granted only when both algorithms independently confirm the optical key.

#### Q2: Why use Visible Light Communication (VLC) instead of RF (Wi-Fi, Bluetooth, RFID)?
**Answer:** RF signals penetrate walls and can be intercepted, jammed, or spoofed from a distance. Modulated light requires direct line-of-sight propagation, creating a tight physical security boundary where eavesdropping without line-of-sight is impossible.

#### Q3: Why was the BPW34 PIN photodiode chosen over phototransistors or integrated sensors?
**Answer:** The BPW34 provides a fast response time ($20\text{ ns}$ rise time) and a large radiant sensitive area ($7.5\text{ mm}^2$), delivering linear photocurrent conversion across visible and near-IR wavelengths without the non-linear saturation associated with phototransistors.

#### Q4: Why was the LM358 operational amplifier selected for the Transimpedance Amplifier (TIA)?
**Answer:** The LM358 operates from a single $+3.3\text{ V}$ supply rail matching the ESP32-S3 logic levels. Powering the LM358 from $+3.3\text{ V}$ naturally limits maximum output voltage swing to $\approx 1.8\text{ V}$, protecting the ESP32-S3 ADC pin from overvoltage damage without requiring external clamping diodes.

#### Q5: Why use two microcontrollers (ESP32-S3 + ESP32 DevKit-V1) instead of a single MCU?
**Answer:** Motor drive switching transients ($>300\text{ mA}$ peak coil current) generate electromagnetic noise and ground bounce. Separating Subsystem 1 (Access Control) from Subsystem 2 (Docking Stepper) maintains complete physical and electrical isolation for microsecond-accurate ADC sampling.

#### Q6: Do Subsystem 1 and Subsystem 2 communicate with each other over UART?
**Answer:** No. In the current N.O.V.A. prototype, Subsystem 1 and Subsystem 2 operate as **independent standalone hardware modules**. Inter-MCU serial communication (e.g., optically isolated UART or ESP-NOW) is designated as a planned engineering improvement.

#### Q7: Why use two-factor authentication (NFC + VLC)?
**Answer:** Single-factor NFC cards can be cloned, lost, or stolen. Single-factor VLC optical keyfobs could be intercepted if pointed carelessly. Requiring both physical possession of a registered NFC card AND optical alignment to a key transmitter configured to the cardholder's role enforces two-factor physical access security (NFC ∧ VLC).

---

## 8. License
MIT License — see [LICENSE](LICENSE). Edge Impulse generated library files subject to separate terms.
