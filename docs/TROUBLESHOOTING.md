# N.O.V.A. Hardware & Firmware Troubleshooting Guide

## 1. Quick Diagnostic Checklist

Perform these hardware checks prior to detailed diagnosis:

* [ ] **Power Rails:** Measure $+3.3\text{ V}$ logic and $+5.0\text{ V}$ VBUS rails with a digital multimeter ($3.25\text{ V} \le V_{\text{3V3}} \le 3.35\text{ V}$).
* [ ] **PN532 I2C Bus:** Verify SDA (GPIO 8) and SCL (GPIO 9) lines have $10\text{ k}\Omega$ pull-up resistors to $3.3\text{ V}$ and DIP switches are configured for I2C mode (`I0=0, I1=1`).
* [ ] **Photodiode Polarity:** BPW34 Anode connected to GND, Cathode connected to LM358 inverting input (Pin 2).
* [ ] **Relay Module & Flyback Diode:** Relay VCC powered from $5\text{ V}$, control pin to GPIO 4, 12V DC solenoid circuit protected by a reverse 1N4007 flyback diode across the coil.
* [ ] **Stepper Motor Wiring:** 28BYJ-48 5-pin connector plugged into ULN2003 driver; driver inputs IN1--IN4 connected to GPIOs 13, 12, 14, 27.

---

## 2. Symptom-Based Diagnostic Procedures

### Symptom 1: PN532 Initialization Fails (`[NFC] ERROR: PN532 board not found`)
* **Possible Causes:**
  1. Incorrect DIP switch configuration on PN532 breakout board (must be set to I2C mode: `I0=0, I1=1`).
  2. Missing pull-up resistors on SDA (GPIO 8) or SCL (GPIO 9).
  3. Incorrect I2C device address (default is `0x24`).
* **Diagnostic Steps:**
  1. Verify DIP switch settings on the PN532 module.
  2. Measure DC voltage on SDA and SCL lines while idle (should be $\approx 3.3\text{ V}$).
  3. Run an I2C scanner sketch to confirm the device responds at address `0x24`.

---

### Symptom 2: Optical ADC Saturation Warning (`AUTH_REASON_ADC_SATURATED`)
* **Possible Causes:**
  1. Unfiltered ambient sunlight or intense overhead direct lighting incident on the BPW34 sensor.
  2. Transimpedance feedback resistor $R_f$ value too high ($>100\text{ k}\Omega$).
  3. Op-amp VCC rail shorted to $+5\text{ V}$ instead of $+3.3\text{ V}$.
* **Diagnostic Steps:**
  1. Add an opaque shroud around the BPW34 photodiode to reduce ambient light capture.
  2. Measure DC voltage at LM358 Pin 1 under normal room light (expected: $0.2\text{ V} \text{ to } 1.2\text{ V}$).
  3. Confirm LM358 VCC is tied to the $+3.3\text{ V}$ rail, capping maximum output swing to $\approx 1.8\text{ V}$.

---

### Symptom 3: Goertzel Ambiguous Spectrum (`AUTH_REASON_GOERTZEL_AMBIGUOUS`)
* **Possible Causes:**
  1. Transmitter LED modulation frequency offset exceeds $\pm 2.5\%$.
  2. Transmitter distance exceeds $1.5\text{ m}$, yielding low Signal-to-Noise Ratio (SNR).
  3. Dominant $50\text{ Hz}$ or $100\text{ Hz}$ artificial lighting flicker interfering with capture.
* **Diagnostic Steps:**
  1. Verify LED modulation frequency using an oscilloscope or logic analyzer.
  2. Verify distance between transmitter LED and photodiode is $\le 1.0\text{ m}$.
  3. Run the host DSP test harness ([`validation/test_harnesses/test_goertzel.py`](../validation/test_harnesses/test_goertzel.py)) to verify that the dominance ratio threshold ($\ge 2.0$) is calculated correctly.

---

### Symptom 4: Stepper Motor Stalls or Vibrates Without Smooth Rotation
* **Possible Causes:**
  1. Coil drive sequence mismatch on GPIOs 13, 12, 14, 27.
  2. ULN2003 supply voltage under load drops below $4.5\text{ V}$.
  3. Mechanical binding in platform shaft coupling.
* **Diagnostic Steps:**
  1. Verify the 8-phase drive sequence in [`subsystem2_docking/src/hal/stepper_driver.cpp`](../subsystem2_docking/src/hal/stepper_driver.cpp).
  2. Measure ULN2003 $V_{CC}$ during stepping (must stay $\ge 4.75\text{ V}$).
  3. Rotate the platform manually by hand while power is off to verify free rotation.

---

### Symptom 5: Relay Clicks but Solenoid Lock Does Not Retract
* **Possible Causes:**
  1. 12V DC power supply disconnected or insufficient current capacity ($< 800\text{ mA}$).
  2. Missing common ground return between 12V supply and solenoid circuit.
  3. Mechanical latch binding under door frame friction.
* **Diagnostic Steps:**
  1. Measure 12V power supply voltage during relay assertion (voltage drop should be $< 0.5\text{ V}$).
  2. Test the solenoid lock independently by applying 12V directly to its leads.
