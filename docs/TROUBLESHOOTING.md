# N.O.V.A. Hardware & Firmware Troubleshooting Guide (AUD-034)

> **Document Status:** Official Hardware & Firmware Diagnostic Guide v1.0  
> **Author:** Kiruthik R S  
> **Source of Truth:** Repository HAL Modules, System Architecture Spec, and Electrical Operating Limits.

---

## 1. Quick Diagnostic Checklist

Before detailed troubleshooting, perform these fundamental hardware checks:
- [ ] **Power Supply:** $+5.0\text{ V}$ VBUS and $+3.3\text{ V}$ logic rails stable (measure with DMM: $3.25\text{ V} \le V_{\text{3V3}} \le 3.35\text{ V}$).
- [ ] **PN532 I2C Bus:** SDA (GPIO 8) and SCL (GPIO 9) connected with $10\text{ k}\Omega$ pull-up resistors to $3.3\text{ V}$.
- [ ] **Optical Sensor:** BPW34 photodiode installed with correct polarity (Anode to GND, Cathode to TIA inverting input).
- [ ] **Optocoupler Relay:** Relay VCC connected to $5\text{ V}$, IN pin to GPIO 4 ($3.3\text{ V}$ logic), Solenoid coil powered from $12\text{ V}$ DC supply with reverse 1N4007 flyback diode.
- [ ] **Stepper Motor:** 28BYJ-48 5-pin connector plugged into ULN2003 driver, IN1--IN4 connected to GPIOs 13, 12, 14, 27.

---

## 2. Symptom-Based Diagnostic Procedures

### Symptom 1: PN532 Initialization Fails (`[NFC] ERROR: PN532 board not found`)
* **Possible Root Causes:**
  1. Incorrect DIP switch configuration on PN532 breakout board (must be set to I2C mode: `I0=0, I1=1`).
  2. Missing I2C pull-up resistors on SDA (GPIO 8) / SCL (GPIO 9).
  3. I2C bus address mismatch (`0x24` default for PN532).
* **Diagnostic Steps:**
  1. Check DIP switches on PN532 breakout board.
  2. Run an I2C bus scanner sketch to verify device responds at address `0x24`.
  3. Measure SDA and SCL voltages when idle (should be $\approx 3.3\text{ V}$).

---

### Symptom 2: Optical Signal Read Error / ADC Saturation (`AUTH_REASON_ADC_SATURATED`)
* **Possible Root Causes:**
  1. Direct unshielded sunlight or bright overhead room lighting incident on BPW34 photodiode.
  2. TIA feedback resistor $R_f$ value too high ($>100\text{ k}\Omega$).
  3. Op-amp rail shorted to $5\text{ V}$.
* **Diagnostic Steps:**
  1. Mount black 3D-printed shroud ($10\text{ mm}$ depth) around BPW34 sensor.
  2. Measure DC output voltage at LM358 Pin 1 under ambient light (should be $0.2\text{ V} \text{ to } 1.2\text{ V}$).
  3. Verify LM358 VCC is connected to $+3.3\text{ V}$ rail, limiting output swing under saturation to $\approx 1.8\text{ V}$.

---

### Symptom 3: Goertzel Ambiguous Spectrum (`AUTH_REASON_GOERTZEL_AMBIGUOUS`)
* **Possible Root Causes:**
  1. Transmitter modulation frequency offset ($> \pm 2.5\%$).
  2. Weak optical transmitter LED signal causing low Signal-to-Noise Ratio (SNR).
  3. 50 Hz AC mains lighting flicker dominating optical capture window.
* **Diagnostic Steps:**
  1. Verify LED beacon modulation frequency using an oscilloscope or logic analyzer.
  2. Decrease distance between transmitter LED and BPW34 photodiode to $<1\text{ m}$.
  3. Run Python DSP test harness (`python validation/test_harnesses/test_goertzel.py`) to verify Goertzel dominance ratio calculation ($M_{\text{dominant}} / M_{\text{second}} \ge 2.0$).

---

### Symptom 4: Stepper Motor Stalls or Vibrates Without Rotating
* **Possible Root Causes:**
  1. Incorrect coil stepping sequence on GPIOs 13, 12, 14, 27.
  2. Insufficient motor supply voltage ($< 4.5\text{ V}$ DC at ULN2003 driver).
  3. Mechanical binding in sensor platform shaft coupling.
* **Diagnostic Steps:**
  1. Verify 8-phase half-step sequence in `subsystem2_docking/hal/stepper_driver.cpp`.
  2. Measure ULN2003 supply voltage during stepping (must be $\ge 4.8\text{ V}$).
  3. Manually rotate sensor platform to ensure smooth mechanical motion.

---

### Symptom 5: Solenoid Relay Clicks but Solenoid Lock Does Not Retract
* **Possible Root Causes:**
  1. 12V DC power supply disconnected or insufficient current capacity ($< 800\text{ mA}$).
  2. Missing ground return connection between 12V supply and solenoid lock.
  3. Solenoid coil mechanical latch binding under door frame pressure.
* **Diagnostic Steps:**
  1. Measure 12V DC power rail during relay assertion (voltage drop should be $< 0.5\text{ V}$).
  2. Test solenoid lock independently by connecting directly to 12V supply.
