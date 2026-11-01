# N.O.V.A. Known Limitations & Engineering Trade-Offs

## 1. Subsystem 1 (Secure Access) Limitations

### 1.1 Optical Transmission Range & Alignment
* **Maximum Line-of-Sight Range:** Tested up to $1.5\text{ m}$ in indoor ambient lighting. Beyond $1.5\text{ m}$, optical flux diminishes according to the inverse-square law ($1/r^2$), reducing receiver Signal-to-Noise Ratio (SNR) below the classifier confidence threshold.
* **Angular Sensitivity:** Off-axis alignment beyond $\pm 30^\circ$ substantially reduces incident light on the BPW34 PIN photodiode radiant area ($7.5\text{ mm}^2$), triggering low-confidence rejection.

### 1.2 Static Credential Storage
* In the current prototype, registered NFC UIDs are hardcoded in [`subsystem1_secure_access/config.h`](../subsystem1_secure_access/config.h) as compile-time constants:
  * Admin: `{0xAE, 0x9A, 0xF9, 0x06}` (expects 30 Hz optical key)
  * Staff: `{0x03, 0x5F, 0x1A, 0x32}` (expects 20 Hz optical key)
  * Guest: `{0xDE, 0xB0, 0x29, 0x07}` (expects 10 Hz optical key)
* Production deployments require dynamic credential provisioning stored in encrypted Non-Volatile Storage (NVS).

### 1.3 Sampling Window Latency
* Capturing 100 samples at $100.0\text{ Hz}$ imposes a fixed $1.000\text{ s}$ observation window. After NFC card tap and a $1.5\text{ s}$ user preparation delay, authentication requires $\approx 1.05\text{ s}$ total compute and capture latency.

### 1.4 Direct Sunlight Saturation
* Direct sunlight ($>50,000\text{ lux}$) saturates the BPW34 photodiode, driving the LM358 amplifier output to its upper rail ($\approx 1.8\text{ V}$) and tripping the ADC saturation threshold ($>3890\text{ counts}$). While this safely prevents false unlocks, physical optical baffling or optical narrow-bandpass filtering is required for outdoor operation.

---

## 2. Subsystem 2 (Autonomous Docking) Limitations

### 2.1 Unipolar Stepper Gear Backlash
* The 28BYJ-48 stepper motor uses a 1:64 reduction gearbox that exhibits mechanical backlash ($\approx 1.4^\circ$).
* **Mitigation:** The firmware executes an intentional 16 half-step ($1.406^\circ$) counter-clockwise overshoot during fine alignment before making its final approach clockwise.

### 2.2 Open-Loop Step Tracking
* Platform orientation is tracked by accumulating step counts in firmware without absolute angular encoder feedback.
* On power-up, position 0 is assumed at current platform heading. A manual mechanical homing routine or physical limit switch is required to calibrate absolute zero.

---

## 3. System-Level Limitations

### 3.1 Subsystem Independence
* Subsystem 1 (Access Control) and Subsystem 2 (Docking Platform) operate as independent standalone units. There is no active inter-MCU communication link in the current hardware build.

### 3.2 Operational Amplifier Input Offset
* The LM358 was chosen for widespread availability and single-supply 3.3V operation. Its input offset voltage (up to $7\text{ mV}$) and moderate slew rate ($0.5\text{ V}/\mu\text{s}$) are adequate for low-frequency modulated VLC ($10\text{--}30\text{ Hz}$), but would limit high-frequency data throughput.

---

## 4. Planned Engineering Improvements

The following improvements address the limitations identified above:

### 4.1 Security Hardening
* **Encrypted NVS Credential Storage:** Migrate static card UIDs to ESP32-S3 Non-Volatile Storage (NVS) protected by AES-XTS-256 Flash Encryption.
* **MIFARE DESFire EV3 Support:** Upgrade from 4-byte UID MIFARE Classic to DESFire EV3 with mutual AES-128 challenge-response authentication.
* **Dynamic Rolling Optical Tokens:** Replace static frequency tones with pseudo-random rolling Frequency-Shift Keying (FSK) tokens synchronized between mobile transmitter and access lock.

### 4.2 Signal Processing & Edge ML Optimization
* **Adaptive Sampling Window:** Adjust ADC sample count dynamically based on measured optical SNR to reduce sampling latency under high-flux conditions.
* **INT8 Quantization:** Quantize the Edge Impulse spectral classifier to 8-bit integer precision, reducing Flash weight storage and memory footprint.
* **Bandpass Optical Shrouding:** Add an optical bandpass filter matched to the transmitter LED wavelength to reject ambient sunlight and high-lux DC interference.

### 4.3 Autonomous Docking Enhancements
* **Closed-Loop Position Feedback:** Integrate a magnetic angle sensor (such as the AS5600 over I2C) or an optical limit switch to provide absolute angular reference and eliminate gear backlash drift.
* **Continuous Optical Tracking:** Implement a proportional closed-loop tracking loop that adjusts platform azimuth dynamically as a moving optical beacon transitions across the field of view.
* **Two-Axis Gimbal Platform:** Extend scanning to elevation and azimuth using a dual-motor pan-tilt assembly.

### 4.4 Platform Integration
* **Inter-Subsystem Communication Bus:** Implement an optically isolated UART link or ESP-NOW wireless interface between Subsystem 1 and Subsystem 2, enabling coordinated dock-and-unlock routines.
* **Integrated Carrier PCB:** Design a custom 2-layer PCB combining the ESP32-S3 and ESP32 modules, 12V-to-5V DC-DC buck converter, 3.3V LDO regulator, and isolated driver stages.
* **Power Optimization:** Implement ESP32 deep-sleep modes with hardware interrupts on NFC card detection and photodiode edge transitions.
