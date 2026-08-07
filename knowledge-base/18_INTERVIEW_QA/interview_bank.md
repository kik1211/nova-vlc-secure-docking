# Technical Interview Preparation Bank — N.O.V.A. Project

> **Knowledge Base Module:** `18_INTERVIEW_QA/interview_bank.md`  
> **Target Roles:** Recruiter Pitch, Embedded Firmware Engineer, DSP / ML Engineer, Hardware / Security Engineer

---

## Category 1: Recruiter / 60-Second Elevator Pitch

### Q1: What is the N.O.V.A. project in simple terms?
**Answer:** N.O.V.A. is a physical access control and autonomous optical docking system inspired by spacecraft technology. It replaces single-factor access locks with a two-factor security mechanism: an NFC card scan followed by an optical key sent via Visible Light Communication (VLC). On the receiver side, an ESP32-S3 uses an Edge Impulse TinyML neural network combined with a Goertzel DSP algorithm to verify the light frequency before releasing a 12V solenoid lock. Additionally, it features a self-aligning optical docking platform powered by a 28BYJ-48 stepper motor.

### Q2: What was the main engineering challenge you solved?
**Answer:** The primary challenge was reliable frequency classification of weak optical signals in noisy ambient light environments using low-cost hardware. We solved this by designing a transimpedance amplifier (TIA) coupled with a passive anti-aliasing filter, an $esp\_timer$-driven 100 Hz sampler, and a dual-verdict verification gate that pairs a probabilistic neural network with a deterministic Goertzel DFT algorithm.

---

## Category 2: Embedded Firmware Engineer Level

### Q3: Why did you use `esp_timer` instead of standard Arduino `delay()` for optical sampling?
**Answer:** Standard `delay()` calls block the CPU execution thread, introducing jitter due to background RTOS tasks and variable loop timing. The `esp_timer` peripheral hardware timer triggers a dedicated Interrupt Service Routine (ISR) every $10,000\ \mu\text{s}$ ($100\text{ Hz}$), guaranteeing strict sample interval timing necessary for coherent Discrete Fourier Transform (DFT) analysis.

### Q4: How is non-blocking execution achieved in Subsystem 1?
**Answer:** Subsystem 1 is implemented as an 8-state Finite State Machine (FSM). Each iteration of `loop()` executes `auth_tick()`, which checks conditions using `millis()` and non-blocking flags, performing at most one state transition before returning immediately.

---

## Category 3: DSP & TinyML Engineer Level

### Q5: Why choose the Goertzel algorithm over a 256-point Fast Fourier Transform (FFT)?
**Answer:** A full FFT calculates magnitudes across all $\frac{N}{2}$ frequency bins with computational complexity $O(N \log N)$. However, our system only needs to detect three specific target frequencies ($10\text{ Hz}, 20\text{ Hz}, 30\text{ Hz}$). The Goertzel algorithm evaluates individual DFT bins with $O(N)$ complexity, drastically reducing RAM footprint and CPU cycles while achieving zero spectral leakage since $k = \frac{f_{\text{target}}}{f_s} \cdot N$ evaluates to exact integers ($k=10, 20, 30$).

### Q6: How does the Dual-Verdict 2FA Gate work?
**Answer:** The dual-verdict gate evaluates two independent conditions:
1. **TinyML Condition:** Edge Impulse 1D-CNN classifier winner label matches expected role label AND confidence $\ge 85\%$.
2. **DSP Condition:** Goertzel magnitude at expected frequency exceeds the second-highest magnitude by a dominance ratio $\ge 2.0$.
Access is granted ONLY if BOTH conditions evaluate to TRUE.

---

## Category 4: Security & Hardware Engineer Level

### Q7: How does the system prevent ambient light saturation and optical replay attacks?
**Answer:** 
* **DC Overvoltage Protection:** The LM358 TIA is powered from $3.3\text{ V}$, ensuring output voltage never exceeds the ESP32-S3 ADC pin threshold.
* **Ambient Flicker Rejection:** Standard artificial lighting ($100\text{ Hz} / 120\text{ Hz}$ flicker) is classified as `noise` by the neural network with $>99\%$ confidence.
* **Static Light Rejection:** Unmodulated static light (sunlight/flashlights) yields zero Goertzel AC energy, causing immediate rejection by the dominance ratio check.

---

## Category 5: Senior System Architecture & Senior Design Trade-Offs (AUD-033)

### Q8: What are the fundamental architectural limitations of your prototype, and how would you address them in a commercial revision?
**Answer:**
1. **LM358 Op-Amp Output Swing:** The LM358 is not a rail-to-rail op-amp, limiting max output swing to $V_{CC}-1.5\text{ V}$ ($\approx 1.8\text{ V}$ at 3.3V supply). While this conveniently protects the ADC from overvoltage, it compresses dynamic range. *Fix:* Upgrade to a rail-to-rail I/O op-amp like MCP6002 with active clamping.
2. **Open-Loop Stepper Motor Tracking:** The 28BYJ-48 motor relies on open-loop step counting without absolute encoder feedback. *Fix:* Integrate a magnetic rotary encoder (AS5600) for closed-loop position feedback.
3. **Subsystem Independence:** The current prototype operates Subsystem 1 and Subsystem 2 as independent standalone modules without inter-MCU communication. *Fix:* Implement an isolated UART or ESP-NOW link to enable automatic triggering of docking scans upon NFC presentation.

### Q9: Why did you choose single-supply op-amp topology instead of dual-supply ($\pm 5\text{ V}$)?
**Answer:** Single-supply topology ($+3.3\text{ V}$) eliminates the need for negative voltage charge pumps or DC-DC converters, reducing BOM cost and PCB footprint. By referencing the photodiode photocurrent to ground, the op-amp output swings positively from ground up to $1.8\text{ V}$, matching the single-ended SAR ADC input range of the ESP32-S3.

### Q10: How do you defend using TinyML for a simple frequency classification task?
**Answer:** While Goertzel DSP can detect pure sinusoids, real-world optical signals suffer from non-linear LED switching, ambient lighting flicker, and partial optical occlusions. The TinyML 1D-CNN model excels at learning complex non-linear spectral signatures and background noise patterns. Combining both algorithms in a Dual-Verdict Gate provides the best of both worlds: deterministic mathematical rigor and probabilistic noise tolerance.
