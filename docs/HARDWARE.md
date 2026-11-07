# N.O.V.A. Hardware Specification & Wiring Guide

## 1. Bill of Materials (BOM)

### Subsystem 1: Secure Access Control (ESP32-S3)
Detailed CSV: [`hardware/bom/subsystem1_bom.csv`](../hardware/bom/subsystem1_bom.csv)

| RefDes | Component Description | Manufacturer / MPN | Package | Target Cost (INR) | Acceptable Substitutes |
|:---|:---|:---|:---|:---:|:---|
| **MCU-1** | ESP32-S3 DevKitC-1 (N16R8) | Espressif `ESP32-S3-DevKitC-1-N16R8` | Dual-row DIP | ₹650 | ESP32-S3-DevKitM-1 |
| **NFC-1** | PN532 NFC Breakout Board | NXP `PN5321A3HN/C106` | Breakout module | ₹350 | Elechouse PN532 V3 |
| **OPT-1** | Silicon PIN Photodiode | Vishay `BPW34` | DIP-2 / Flat lead | ₹45 | BPW34S, SFH 203 P |
| **AMP-1** | Dual Operational Amplifier | Texas Instruments `LM358P` | DIP-8 | ₹15 | LM358N, MCP6002 |
| **R-1** | Feedback Resistor $R_f$ ($100\text{ k}\Omega, 1\%$) | Yageo `MF0207FTE52-100K` | Axial 1/4W | ₹2 | Metal film $100\text{ k}\Omega$ |
| **R-2** | Filter Resistor $R$ ($3.3\text{ k}\Omega, 1\%$) | Yageo `MF0207FTE52-3K3` | Axial 1/4W | ₹2 | Metal film $3.3\text{ k}\Omega$ |
| **C-1** | Filter Capacitor $C$ ($1.0\ \mu\text{F}, 50\text{V}$) | KEMET `C320C105K5R5TA` | Radial ceramic | ₹5 | Ceramic $1.0\ \mu\text{F}$ |
| **RELAY-1**| 5V Active-HIGH Relay Module | Songle `SRD-05VDC-SL-C` | Module | ₹70 | 5V optocoupled relay |
| **LOCK-1** | 12V Fail-Secure Solenoid Lock | Generic `LY-01 12V` | Metal frame | ₹450 | 12V solenoid latch |

### Subsystem 2: Autonomous Optical Docking (ESP32)
Detailed CSV: [`hardware/bom/subsystem2_bom.csv`](../hardware/bom/subsystem2_bom.csv)

| RefDes | Component Description | Manufacturer / MPN | Package | Target Cost (INR) | Acceptable Substitutes |
|:---|:---|:---|:---|:---:|:---|
| **MCU-2** | ESP32 DevKit-V1 (ESP-WROOM-32) | Espressif `ESP32-WROOM-32D` | 30-pin DIP | ₹350 | NodeMCU-32S |
| **M-1** | Geared Unipolar Stepper Motor | Kysan `28BYJ-48 5V` | Geared can | ₹120 | 28BYJ-48 unipolar 5V |
| **DRV-1** | Darlington Driver Module | STMicroelectronics `ULN2003A` | Module | ₹45 | ULN2003 board |
| **OPT-2** | Silicon PIN Photodiode | Vishay `BPW34` | DIP-2 | ₹45 | BPW34S |

---

## 2. Pinout & Interconnect Tables

### Subsystem 1: ESP32-S3 DevKit-C
Firmware mapping source: [`subsystem1_secure_access/config.h`](../subsystem1_secure_access/config.h)

| GPIO Pin | Function | Peripheral Connection | Signal Type | Logic Level |
|:---|:---|:---|:---|:---|
| **GPIO 1** | Optical ADC (`SENSOR_PIN`) | LM358 TIA output via RC filter | Analog Input | $0.0\text{--}3.1\text{ V}$ |
| **GPIO 4** | Relay Trigger (`RELAY_PIN`) | 5V Relay module IN pin | Digital Output | $3.3\text{ V}$ Active HIGH |
| **GPIO 8** | I2C SDA (`SDA_PIN`) | PN532 SDA pin ($10\text{ k}\Omega$ pull-up) | Digital I/O | $3.3\text{ V}$ Open-Drain |
| **GPIO 9** | I2C SCL (`SCL_PIN`) | PN532 SCL pin ($10\text{ k}\Omega$ pull-up) | Digital Output | $3.3\text{ V}$ Open-Drain |
| **5V / VBUS**| System Power | USB input, relay module VCC | Power | $+5.0\text{ V}$ DC |
| **3V3** | Logic Power | ESP32-S3, PN532 VCC, LM358 VCC | Power | $+3.3\text{ V}$ DC |
| **GND** | Ground | Common ground bus | Power | $0.0\text{ V}$ |

### Subsystem 2: ESP32 DevKit-V1
Firmware mapping source: [`subsystem2_docking/config.h`](../subsystem2_docking/config.h)

| GPIO Pin | Function | Peripheral Connection | Signal Type | Logic Level |
|:---|:---|:---|:---|:---|
| **GPIO 13** | Stepper Phase A (`STEPPER_PIN_IN1`)| ULN2003 IN1 | Digital Output | $3.3\text{ V}$ logic |
| **GPIO 12** | Stepper Phase B (`STEPPER_PIN_IN2`)| ULN2003 IN2 | Digital Output | $3.3\text{ V}$ logic |
| **GPIO 14** | Stepper Phase C (`STEPPER_PIN_IN3`)| ULN2003 IN3 | Digital Output | $3.3\text{ V}$ logic |
| **GPIO 27** | Stepper Phase D (`STEPPER_PIN_IN4`)| ULN2003 IN4 | Digital Output | $3.3\text{ V}$ logic |
| **GPIO 34** | Docking ADC (`DOCKING_SENSOR_PIN`)| BPW34 Photodiode signal | Analog Input | $0.0\text{--}3.3\text{ V}$ |
| **5V / VIN** | Motor Power | ULN2003 driver $V_{CC}$ (motor supply) | Power | $+5.0\text{ V}$ DC |
| **GND** | Ground | Isolated common ground | Power | $0.0\text{ V}$ |

---

## 3. Analog Schematic & Circuit Topology

```text
               +3.3V
                 |
                 +----------+
                 |          |
                [R_f]     [LM358] VCC (Pin 8)
              100k Ohm      |
                 |          |
 BPW34           +----+-----+----> LM358 Out (Pin 1)
 Photodiode           |                    |
 (Anode)             |-| LM358             +---[ R: 3.3k ]---+---> ESP32-S3 GPIO 1
   +------------->---| | In-                                 |
   |                 |+| In+                               [C: 1.0uF]
  GND                 |                                      |
                      +--------------------------------------+----> GND
```

* **Transimpedance Gain:** $V_{\text{out}} = I_{\text{pd}} \cdot R_f = I_{\text{pd}} \cdot 100\text{ k}\Omega$.
* **Low-Pass Filter Cutoff:** $f_c = \frac{1}{2\pi R C} = \frac{1}{2\pi \cdot 3.3\text{ k}\Omega \cdot 1.0\ \mu\text{F}} \approx 48.2\text{ Hz}$.
* **Overvoltage Clamping:** Because the LM358 is powered from $+3.3\text{ V}$, its maximum output voltage swing is naturally constrained to $V_{CC} - 1.5\text{ V} \approx 1.8\text{ V}$, preventing damage to the ESP32-S3 ADC pin under optical saturation.

---

## 4. Hardware Assembly Guidelines

1. **Photodiode Optical Shroud:** Mount the BPW34 inside an opaque 3D-printed shroud ($10\text{ mm}$ depth, $8\text{ mm}$ aperture) to block ambient off-axis light.
2. **Bypass Capacitors:** Place a $0.1\ \mu\text{F}$ ceramic capacitor directly across the LM358 $V_{CC}$ and GND pins to filter high-frequency switching ripple.
3. **Solenoid Flyback Diode:** Connect a 1N4007 diode in reverse-parallel across the 12V solenoid coil terminals to suppress inductive switch-off transients.
4. **Supply Separation:** Power the stepper motor driver and solenoid from dedicated supplies or a high-current 5V/12V adapter to prevent supply droop during motor stepping or solenoid retraction.

---

## 5. Component Selection Rationale

### 5.1 ESP32-S3 DevKit-C (Subsystem 1)
* **Rationale:** Provides Xtensa dual-core LX7 running at 240 MHz with vector extensions (ESP-NN) for on-device neural network acceleration, hardware timer peripherals (`esp_timer`) for microsecond-level ADC sampling, and 8 MB PSRAM.
* **Alternative Considered:** RP2040 (rejected due to lack of hardware FPU for Goertzel float operations); STM32F411 (rejected due to 512 KB Flash limit).

### 5.2 ESP32 DevKit-V1 (Subsystem 2)
* **Rationale:** Completely decouples stepper motor inductive switching transients ($>300\text{ mA}$) from sensitive analog photodiode sampling on Subsystem 1.
* **Alternative Considered:** Single MCU running FreeRTOS tasks (rejected because inductive ground bounce corrupted TIA readings during active stepping).

### 5.3 PN532 NFC Module
* **Rationale:** Supports ISO/IEC 14443A (MIFARE Classic 1K, Ultralight, NTAG) over I2C with stable hardware framing and wide library support.
* **Alternative Considered:** RC522 (rejected due to lack of NTAG/NFC Forum support and erratic I2C bus locking).

### 5.4 BPW34 Silicon PIN Photodiode
* **Rationale:** Fast rise time ($20\text{ ns}$), large radiant sensitive area ($7.5\text{ mm}^2$), and linear response across visible to near-infrared light ($400\text{--}1100\text{ nm}$).
* **Alternative Considered:** Light-dependent resistor (LDR, response time $20\text{--}50\text{ ms}$ is too slow for 30 Hz square waves); Phototransistor (suffers non-linear saturation).

### 5.5 LM358 Operational Amplifier
* **Rationale:** Reliable single-supply 3.3V operation matching microcontroller logic levels; output swing limitation provides intrinsic ADC pin overvoltage protection.
* **Alternative Considered:** Precision TIA ICs like OPA380 (expensive and hard to source locally in India).

### 5.6 28BYJ-48 Stepper & ULN2003A Driver
* **Rationale:** Integrated 1:64 reduction gearbox provides fine discrete half-step resolution ($0.08789^\circ/\text{step}$) with high detent holding torque when unpowered.
* **Alternative Considered:** SG90 micro servo (lacks $360^\circ$ continuous rotation and suffers from PWM jitter).

---

## 6. Electrical Operating Limits & Absolute Maximum Ratings

| Component | Parameter | Min | Typical | Max | Unit | Reference Condition |
|:---|:---|:---:|:---:|:---:|:---:|:---|
| **ESP32-S3** | Supply Voltage ($V_{DD}$) | $3.0$ | $3.3$ | $3.6$ | $\text{V}$ | Recommended Operating Rating |
| | ADC Input Voltage Range | $0.0$ | — | $3.1$ | $\text{V}$ | $11\text{ dB}$ Attenuation setting |
| | GPIO Sink/Source Current | — | — | $40.0$ | $\text{mA}$ | Absolute Maximum Rating |
| **ESP32** | Supply Voltage ($V_{DD}$) | $3.0$ | $3.3$ | $3.6$ | $\text{V}$ | Recommended Operating Rating |
| | ADC Input Voltage Range | $0.0$ | — | $3.3$ | $\text{V}$ | $11\text{ dB}$ Attenuation setting |
| **PN532** | Supply Voltage ($V_{CC}$) | $3.3$ | $5.0$ | $5.5$ | $\text{V}$ | Operating Voltage |
| | Active RF Current | — | $100.0$ | $120.0$ | $\text{mA}$ | During 13.56 MHz Card Polling |
| **BPW34** | Reverse Breakdown Voltage | $60.0$ | — | — | $\text{V}$ | $I_R = 100\ \mu\text{A}, E = 0$ |
| | Spectral Range ($\lambda_{0.5}$) | $400$ | $900$ | $1100$ | $\text{nm}$ | Wavelength of Peak Sensitivity |
| | Photocurrent ($I_{\text{pd}}$) | $10.0$ | $25.0$ | $50.0$ | $\mu\text{A}$ | Indoor VLC modulation level |
| **LM358** | Supply Voltage ($V_{CC}$) | $3.0$ | $3.3$ | $32.0$ | $\text{V}$ | $+3.3\text{ V}$ single supply in SS1 |
| | Maximum Output Voltage Swing | $0.0$ | — | $1.8$ | $\text{V}$ | $V_{CC} - 1.5\text{ V}$ at $3.3\text{ V}$ supply |
| **28BYJ-48**| Nominal Operating Voltage | $4.75$ | $5.0$ | $5.25$ | $\text{V}$ | Nominal DC Voltage |
| | Phase Coil Resistance | $46.5$ | $50.0$ | $53.5$ | $\Omega$ | Measured at $25^\circ\text{C}$ |
| **ULN2003A**| Collector-Emitter Voltage | — | — | $50.0$ | $\text{V}$ | Absolute Maximum Rating |
| | Collector Current per Channel | — | — | $500.0$ | $\text{mA}$ | Absolute Maximum Rating |
| **Solenoid**| Operating Voltage | $10.8$ | $12.0$ | $13.2$ | $\text{V}$ | Fail-Secure DC Solenoid |
| | Operating Current | $600.0$ | $750.0$ | $850.0$ | $\text{mA}$ | At $V_{IN} = 12.0\text{ V}$ DC |
| | Maximum Pulse Duration | — | — | $10.0$ | $\text{s}$ | Continuous pulse thermal limit |
