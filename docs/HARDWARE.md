# N.O.V.A. Hardware Specification & Wiring Guide

> **Document Status:** Official Hardware Specification v1.0  
> **Source of Truth:** Design Freeze Specification §3, §4, §7, and BOM Files (`hardware/bom/`)

---

## 1. Complete Bill of Materials (BOM) & Procurement Guide (AUD-020)

### Subsystem 1: Secure Access Control
| Item | Component | Manufacturer / MPN | Package | Suppliers (India / Global) | Target Cost (INR) | Acceptable Substitutes |
|:---|:---|:---|:---|:---|:---:|:---|
| MCU-1 | Microcontroller | Espressif `ESP32-S3-DevKitC-1-N16R8` | Dev Board | Robu.in, Silverleafe / Mouser, DigiKey | ₹650 | ESP32-S3-DevKitM-1, ESP32-S3-WROOM-1 |
| NFC-1 | NFC/RFID Reader | NXP `PN5321A3HN/C106` | Breakout | Robu.in, ElectronicsComp / Adafruit 364 | ₹350 | Elechouse PN532 V3, PN532 Red Board |
| OPT-1 | Silicon Photodiode | Vishay `BPW34` | DIP-2 / Flat | QuartzComponents, Local Shop / DigiKey | ₹45 | BPW34S, SFH 203 P, BP104 |
| AMP-1 | Op-Amp TIA | Texas Instruments `LM358P` | DIP-8 | Robu.in, Local Electronics / Mouser | ₹15 | LM358N, MCP6002P, TLV2372 |
| R-1 | Resistor $R_f$ | Yageo `MF0207FTE52-100K` | Axial 1/4W | Local Electronics / DigiKey | ₹2 | $100\text{ k}\Omega$ 1% Metal Film |
| R-2 | Filter Resistor | Yageo `MF0207FTE52-3K3` | Axial 1/4W | Local Electronics / DigiKey | ₹2 | $3.3\text{ k}\Omega$ 1% Metal Film |
| C-1 | Filter Capacitor | KEMET `C320C105K5R5TA` | Radial | Local Electronics / DigiKey | ₹5 | $1.0\ \mu\text{F}$ Ceramic 50V |
| RELAY-1 | Relay Module | Songle `SRD-05VDC-SL-C` | Module | Robu.in, Amazon India | ₹70 | Omron G5Q-14 5V, 5V Active-HIGH Module |
| LOCK-1 | Solenoid Lock | Generic `LY-01 12V` | Metal Frame| Amazon India, Robu.in | ₹450 | 12V Fail-Secure Solenoid Latch |

### Subsystem 2: Autonomous Optical Docking
| Item | Component | Manufacturer / MPN | Package | Suppliers (India / Global) | Target Cost (INR) | Acceptable Substitutes |
|:---|:---|:---|:---|:---|:---:|:---|
| MCU-2 | Microcontroller | Espressif `ESP32-WROOM-32D` | 30-pin Dev | Robu.in, Amazon India / DigiKey | ₹350 | ESP32 DevKit-V1, NodeMCU-32S |
| M-1 | Stepper Motor | Kysan `28BYJ-48 5V` | Geared Motor| Robu.in, ElectronicsComp | ₹120 | 28BYJ-48 5V Unipolar Stepper |
| DRV-1 | Stepper Driver | STMicroelectronics `ULN2003A` | Module | Robu.in, Local Electronics | ₹45 | ULN2003 Board, DARLINGTON Array |
| OPT-2 | Photodiode | Vishay `BPW34` | DIP-2 | QuartzComponents, Local Shop | ₹45 | BPW34S PIN Photodiode |

---

## 2. Pin Mapping & Interconnect Tables

### ESP32-S3 Pinout (Subsystem 1)
| GPIO Pin | Function | Peripheral Connection | Signal Type | Logic Level |
|:---|:---|:---|:---|:---|
| **GPIO 1** | ADC Input (`SENSOR_PIN`) | LM358 TIA Output via RC Filter | Analog Input | $0\text{--}3.1\text{ V}$ |
| **GPIO 4** | Relay Control (`RELAY_PIN`) | 5V Relay Module IN | Digital Output | $3.3\text{ V}$ Active HIGH |
| **GPIO 8** | I2C SDA (`SDA_PIN`) | PN532 SDA Pin | Digital I/O | $3.3\text{ V}$ Open-Drain |
| **GPIO 9** | I2C SCL (`SCL_PIN`) | PN532 SCL Pin | Digital Output | $3.3\text{ V}$ Open-Drain |
| **VBUS (5V)** | Power Rail | Relay Module VCC & Solenoid Drive | Power | $5.0\text{ V}$ DC |
| **3V3** | Power Rail | ESP32-S3, PN532 VCC, LM358 VCC | Power | $3.3\text{ V}$ DC |
| **GND** | System Ground | Common System Ground | Ground | $0\text{ V}$ |

### ESP32 Pinout (Subsystem 2)
| GPIO Pin | Function | Peripheral Connection | Signal Type | Logic Level |
|:---|:---|:---|:---|:---|
| **GPIO 13** | Stepper Coil A (`STEPPER_PIN_IN1`)| ULN2003 IN1 | Digital Output | $3.3\text{ V}$ HIGH |
| **GPIO 12** | Stepper Coil B (`STEPPER_PIN_IN2`)| ULN2003 IN2 | Digital Output | $3.3\text{ V}$ HIGH |
| **GPIO 14** | Stepper Coil C (`STEPPER_PIN_IN3`)| ULN2003 IN3 | Digital Output | $3.3\text{ V}$ HIGH |
| **GPIO 27** | Stepper Coil D (`STEPPER_PIN_IN4`)| ULN2003 IN4 | Digital Output | $3.3\text{ V}$ HIGH |
| **GPIO 34** | Light Sensor (`DOCKING_SENSOR_PIN`)| BPW34 Photodiode Signal | Analog Input | $0\text{--}3.3\text{ V}$ |

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

---

## 4. Hardware Assembly Guidelines
1. **Photodiode Shielding:** Mount BPW34 inside a $10\text{ mm}$ black 3D-printed shroud to restrict off-axis ambient noise.
2. **Decoupling Capacitors:** Place $0.1\ \mu\text{F}$ ceramic decoupling capacitors directly adjacent to LM358 VCC and PN532 VCC pins.
3. **Solenoid Flyback Protection:** Solenoid coil must include a 1N4007 reverse-biased flyback diode across its terminals to absorb inductive voltage spikes.

---

## 5. Component Selection Rationale & Engineering Justifications (AUD-019)

### 5.1 ESP32-S3 DevKit-C N16R8 (Subsystem 1 MCU)
* **Why Selected:** Chosen as the primary processing core for Subsystem 1 due to its Xtensa 32-bit dual-core LX7 microcontroller running at $240\text{ MHz}$, integrated vector instructions (ESP-DSP), high-resolution hardware timers (`esp_timer`), and high flash/PSRAM memory bandwidth ($16\text{ MB}$ Flash, $8\text{ MB}$ PSRAM).
* **Major Advantages:** Dedicated hardware vector instructions accelerate TinyML 1D-CNN matrix multiplications; dual-core structure permits dedicated execution of interrupt-driven sampling alongside inference; native USB-OTG simplifies debugging.
* **Known Limitations:** ADC2 peripheral cannot be used simultaneously with Wi-Fi (mitigated by assigning ADC input strictly to ADC1 channel `ADC1_CH0` / GPIO 1).
* **Practical Engineering Trade-offs:** Offers significantly higher compute capacity than standard ESP32 or ARM Cortex-M4 at a slight increase in active power consumption ($\approx 80\text{--}240\text{ mA}$).
* **Alternatives Considered:** ESP32-WROOM-32, STM32F411 Black Pill, Raspberry Pi Pico (RP2040).
* **Why Alternatives Were Rejected:** ESP32-WROOM-32 lacks LX7 SIMD vector extensions for TinyML acceleration; STM32F411 lacks onboard Wi-Fi/Bluetooth capabilities for future OTA deployment and has smaller memory footprint ($512\text{ KB}$ Flash); RP2040 lacks hardware floating-point unit (FPU) required for efficient double-precision floating-point Goertzel and TFLite Micro operations.

### 5.2 ESP32 DevKit-V1 30-Pin (Subsystem 2 MCU)
* **Why Selected:** Selected for Subsystem 2 to maintain complete physical and electrical isolation between motor drive inductive transients and sensitive optical ADC sampling.
* **Major Advantages:** Cost-effective dual-core $240\text{ MHz}$ MCU with abundant GPIO pins for 4-phase stepper motor driving and analog scanning; robust hardware timer peripherals.
* **Known Limitations:** Higher power consumption than single-core low-power MCUs.
* **Practical Engineering Trade-offs:** Using two microcontrollers increases total BOM cost slightly ($\approx \text{\textrupee}350$ extra) but eliminates ground-bounce noise coupling between stepper motor switching and photodiode TIA sampling.
* **Alternatives Considered:** Single MCU architecture (driving both SS1 and SS2 from ESP32-S3), Arduino Nano (ATmega328P).
* **Why Alternatives Were Rejected:** Single MCU architecture violates noise-isolation boundaries and introduces timing jitter during simultaneous motor stepping and $100\text{ Hz}$ ADC sampling; ATmega328P lacks processing speed ($16\text{ MHz}$) and memory ($2\text{ KB}$ SRAM) for 128-point floating-point scanning buffers.

### 5.3 PN532 NFC/RFID Breakout Module
* **Why Selected:** Selected as the primary physical credential reader operating over $13.56\text{ MHz}$ Near Field Communication.
* **Major Advantages:** Supports MIFARE Classic 1K, Ultralight, and NTAG cards; flexible bus interfaces (I2C, SPI, HSU UART); robust on-chip framing engine.
* **Known Limitations:** High active RF emission current ($\approx 100\text{--}120\text{ mA}$).
* **Practical Engineering Trade-offs:** Configured in I2C polling mode (GPIO 8 SDA, GPIO 9 SCL) to minimize GPIO count by omitting hardware IRQ and Reset pins while maintaining reliable polling.
* **Alternatives Considered:** RC522 RFID module.
* **Why Alternatives Were Rejected:** RC522 only supports ISO/IEC 14443A RFID cards and lacks true NFC peer-to-peer and NTAG compatibility; PN532 offers superior I2C bus stability and broader tag standard support.

### 5.4 BPW34 Silicon PIN Photodiode
* **Why Selected:** Standard silicon PIN photodiode selected for free-space optical signal reception across visible and near-infrared spectrums ($400\text{--}1100\text{ nm}$).
* **Major Advantages:** High spectral sensitivity, fast response time ($20\text{ ns}$ rise time), large radiant sensitive area ($7.5\text{ mm}^2$), highly accessible commercial availability.
* **Known Limitations:** High sensitive area increases junction capacitance ($C_j \approx 25\text{ pF}$ at $V_R = 3\text{ V}$), limiting maximum bandwidth in high-frequency communications (not restrictive for $10\text{--}30\text{ Hz}$ VLC).
* **Practical Engineering Trade-offs:** Requires external optical shroud to restrict ambient off-axis sunlight and indoor lighting flicker.
* **Alternatives Considered:** OPT101 integrated photodiode+op-amp, LDR (Light Dependent Resistor), Phototransistor (2N5777).
* **Why Alternatives Were Rejected:** LDR response time ($10\text{--}50\text{ ms}$) is far too slow to resolve $30\text{ Hz}$ optical square waves; Phototransistors exhibit severe non-linear saturation under ambient sunlight; OPT101 has fixed internal feedback ($1\text{ M}\Omega$) restricting custom gain tuning.

### 5.5 LM358 Dual Operational Amplifier
* **Why Selected:** Configured as a single-supply Transimpedance Amplifier (TIA) with feedback resistor $R_f = 100\text{ k}\Omega$.
* **Major Advantages:** Operates reliably from a single $+3.3\text{ V}$ rail, matching MCU ADC reference levels; extremely low cost ($\approx \text{\textrupee}15$); common dual-inline package.
* **Known Limitations:** Low Gain-Bandwidth Product ($\text{GBW} \approx 1\text{ MHz}$) and limited output voltage swing ($0\text{ V} \text{ to } V_{CC} - 1.5\text{ V}$).
* **Practical Engineering Trade-offs:** Powering LM358 from $+3.3\text{ V}$ restricts maximum output swing to $\approx 1.8\text{ V}$, naturally preventing ADC overvoltage damage to the ESP32-S3 GPIO 1 pin without requiring clamping Zener diodes.
* **Alternatives Considered:** OPA380, MCP6002, TL072.
* **Why Alternatives Were Rejected:** OPA380 is expensive and difficult to source locally; TL072 requires dual supply ($\pm 5\text{ V}$ or $\ge 7\text{ V}$) and suffers from phase reversal near ground; MCP6002 rail-to-rail op-amp is a viable alternative but less universally stocked than LM358.

### 5.6 28BYJ-48 Unipolar Stepper Motor & ULN2003A Driver
* **Why Selected:** Selected for Subsystem 2 autonomous optical docking platform rotation.
* **Major Advantages:** Integrated $64:1$ reduction gearbox delivers high angular resolution ($4096\text{ half-steps/rev}$, $0.08789^\circ/\text{step}$) and holding torque without power consumption when stationary; low operating voltage ($5\text{ V}$).
* **Known Limitations:** Mechanical gearbox introduces backlash ($\approx 1\text{--}2^\circ$), requiring software backlash compensation.
* **Practical Engineering Trade-offs:** Speed is limited to $\approx 15\text{ RPM}$, but high angular step precision is prioritized over rotational speed.
* **Alternatives Considered:** NEMA 17 Bipolar Stepper Motor + A4988, SG90 Servo Motor.
* **Why Alternatives Were Rejected:** SG90 positional servo lacks full $360^\circ$ continuous precise stepping and suffers from potentiometer jitter; NEMA 17 requires high supply voltage ($\ge 12\text{ V}$) and bulky motor drivers, exceeding physical platform requirements.

### 5.7 5V Active-HIGH Relay Module & 12V Solenoid Lock
* **Why Selected:** Active-HIGH transistor-buffered 5V relay module drives a 12V DC fail-secure solenoid door lock mechanism.
* **Major Advantages:** Optocoupler/transistor isolation prevents 12V solenoid inductive spikes from entering MCU power rails; fail-secure solenoid remains locked during power failure.
* **Known Limitations:** Mechanical relay contacts exhibit switching delay ($\approx 5\text{--}10\text{ ms}$) and acoustic click.
* **Practical Engineering Trade-offs:** Mechanical relay selected over MOSFET switch for clear optical/electrical separation and universal load handling.
* **Alternatives Considered:** N-Channel MOSFET (IRLZ44N) high-side/low-side switch.
* **Why Alternatives Were Rejected:** Direct MOSFET switching shares ground return path with solenoid coil, risking ground bounce during inductive discharge if flyback layout is suboptimal.

---

## 6. Consolidated Electrical Operating Limits & Absolute Maximum Ratings (AUD-023)

The following table provides datasheet-supported electrical parameters, operating ranges, and absolute maximum ratings for all hardware components in N.O.V.A.

| Component | Parameter | Min | Typical | Max | Unit | Datasheet Condition / Reference |
|:---|:---|:---:|:---:|:---:|:---:|:---|
| **ESP32-S3** | Supply Voltage ($V_{DD}$) | $3.0$ | $3.3$ | $3.6$ | $\text{V}$ | Recommended Operating Conditions |
| | Input Bus Voltage ($V_{BUS}$) | $4.5$ | $5.0$ | $5.5$ | $\text{V}$ | USB VBUS Supply Rail |
| | Logic Input Low ($V_{IL}$) | $-0.3$ | — | $0.8$ | $\text{V}$ | $V_{DD} = 3.3\text{ V}$ |
| | Logic Input High ($V_{IH}$) | $2.4$ | — | $3.6$ | $\text{V}$ | $V_{DD} = 3.3\text{ V}$ |
| | ADC Input Voltage Range | $0.0$ | — | $3.1$ | $\text{V}$ | $11\text{ dB}$ Attenuation setting |
| | Per-GPIO Current Sink/Source | — | — | $40.0$ | $\text{mA}$ | Absolute Maximum Rating |
| | Active Supply Current | $80.0$ | $140.0$ | $240.0$ | $\text{mA}$ | CPU @ 240MHz, Wi-Fi OFF |
| **ESP32 DevKit-V1** | Supply Voltage ($V_{DD}$) | $3.0$ | $3.3$ | $3.6$ | $\text{V}$ | Recommended Operating Conditions |
| | ADC Input Voltage Range | $0.0$ | — | $3.3$ | $\text{V}$ | $11\text{ dB}$ Attenuation setting |
| | Active Supply Current | $50.0$ | $95.0$ | $160.0$ | $\text{mA}$ | CPU @ 240MHz, Wi-Fi OFF |
| **PN532 NFC** | Supply Voltage ($V_{CC}$) | $3.3$ | $5.0$ | $5.5$ | $\text{V}$ | Operating Voltage |
| | RF Field Active Current | — | $100.0$ | $120.0$ | $\text{mA}$ | During 13.56 MHz Card Polling |
| | Standby Current | — | $4.0$ | $10.0$ | $\text{mA}$ | Power-down / Idle state |
| **BPW34 Photodiode** | Reverse Breakdown Voltage | $60.0$ | — | — | $\text{V}$ | $I_R = 100\ \mu\text{A}, E = 0$ |
| | Spectral Range ($\lambda_{0.5}$) | $400$ | $900$ | $1100$ | $\text{nm}$ | Wavelength of Peak Sensitivity |
| | Reverse Dark Current ($I_R$) | — | $2.0$ | $30.0$ | $\text{nA}$ | $V_R = 10\text{ V}, E = 0$ |
| | Output Photocurrent ($I_{ph}$) | $10.0$ | $25.0$ | $50.0$ | $\mu\text{A}$ | Indoor VLC Modulation Level |
| **LM358 Op-Amp** | Single Supply Voltage ($V_{CC}$) | $3.0$ | $3.3$ | $32.0$ | $\text{V}$ | Operating Rating ($+3.3\text{ V}$ in SS1) |
| | Input Common-Mode Voltage | $0.0$ | — | $V_{CC}-1.5$ | $\text{V}$ | $0.0\text{--}1.8\text{ V}$ at $3.3\text{ V}$ Supply |
| | Maximum Output Voltage Swing | $0.0$ | — | $V_{CC}-1.5$ | $\text{V}$ | $0.0\text{--}1.8\text{ V}$ Max output swing |
| | Supply Current (Dual Op-Amp) | — | $0.7$ | $1.2$ | $\text{mA}$ | Across $V_{CC} = 3.3\text{--}30\text{ V}$ |
| **28BYJ-48 Stepper** | Operating Voltage | $4.75$ | $5.0$ | $5.25$ | $\text{V}$ | Nominal DC Voltage Rating |
| | Coil Resistance per Phase | $46.5$ | $50.0$ | $53.5$ | $\Omega$ | Measured at $25^\circ\text{C}$ |
| | Peak Coil Current per Phase | — | $100.0$ | $200.0$ | $\text{mA}$ | In 2-phase overlap step states |
| **ULN2003A Driver** | Collector-Emitter Voltage ($V_{CE}$)| — | — | $50.0$ | $\text{V}$ | Absolute Maximum Rating |
| | Collector Current per Channel | — | — | $500.0$ | $\text{mA}$ | Absolute Maximum Rating |
| | Input Voltage High ($V_{IN\_ON}$) | $2.4$ | $3.3$ | $5.0$ | $\text{V}$ | Compatible with 3.3V ESP32 GPIO |
| | $V_{CE}$ Saturation Voltage | — | $0.9$ | $1.1$ | $\text{V}$ | $I_C = 100\text{ mA}, I_B = 350\ \mu\text{A}$ |
| **5V Relay Module** | Operating Voltage ($V_{CC}$) | $4.5$ | $5.0$ | $5.5$ | $\text{V}$ | Coil Power Voltage |
| | Trigger Input High ($V_{IH}$) | $2.0$ | $3.3$ | $5.0$ | $\text{V}$ | Active-HIGH Transistor Buffer |
| | Relay Coil Current | — | $70.0$ | $90.0$ | $\text{mA}$ | Coil energized state |
| **Solenoid Lock** | Nominal Operating Voltage | $10.8$ | $12.0$ | $13.2$ | $\text{V}$ | Fail-Secure DC Solenoid |
| | Operating Current | $600.0$ | $750.0$ | $850.0$ | $\text{mA}$ | $V_{IN} = 12.0\text{ V}$ DC |
| | Maximum Actuation Pulse | — | — | $10.0$ | $\text{s}$ | Continuous pulse thermal limit |

