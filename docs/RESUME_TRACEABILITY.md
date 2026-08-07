# N.O.V.A. Resume Traceability Matrix

> **Document Status:** Official Resume Mapping & Technical Evidence Matrix v1.0  
> **Author:** Kiruthik R S  
> **Source Files Under Review:** Repository Firmware, Hardware Schematics, DSP Modules, TinyML Libraries, and Validation Harnesses.

---

## 1. Executive Summary

This document establishes 1-to-1 traceability between every engineering resume bullet point, technical claim, supporting repository source file, concrete code evidence, and technical interview discussion topic.

---

## 2. Resume Claim to Code Evidence Matrix

| Resume Claim / Bullet Point | Primary Source Files | Repository Evidence & Code Locations | Technical Interview Discussion Topics |
|:---|:---|:---|:---|
| **1. Dual-Factor VLC + NFC Physical Access System** | [`subsystem1_secure_access/subsystem1_secure_access.ino`](file:///c:/Users/rskir/OneDrive/Desktop/GIT%20PROJECTS/VLC/subsystem1_secure_access/subsystem1_secure_access.ino), [`auth/auth_state_machine.cpp`](file:///c:/Users/rskir/OneDrive/Desktop/GIT%20PROJECTS/VLC/subsystem1_secure_access/auth/auth_state_machine.cpp) | Implemented 8-state FSM combining MIFARE NFC card reading with optical signal frequency classification. 2FA gate asserts 5V relay ONLY when NFC role matches optical key. | Physical security threat modeling, RF jamming immunity, line-of-sight optical security, non-blocking FSM design. |
| **2. Deterministic Goertzel DSP Algorithm** | [`subsystem1_secure_access/dsp/goertzel.cpp`](file:///c:/Users/rskir/OneDrive/Desktop/GIT%20PROJECTS/VLC/subsystem1_secure_access/dsp/goertzel.cpp), [`dsp/goertzel.h`](file:///c:/Users/rskir/OneDrive/Desktop/GIT%20PROJECTS/VLC/subsystem1_secure_access/dsp/goertzel.h) | Derived and implemented second-order Goertzel IIR filter for target bins ($k=10, 20, 30$). Designed for $<0.8\text{ ms}$ processing budget with 0.00% theoretical leakage under coherent integer-bin alignment. | Goertzel vs Cooley-Tukey FFT complexity ($O(NK)$ vs $O(N \log N)$), coherent sampling integer-bin alignment, dominance ratio noise thresholding. |
| **3. Edge Impulse 1D-CNN TinyML Deployment** | [`subsystem1_secure_access/ml/classifier_wrapper.cpp`](file:///c:/Users/rskir/OneDrive/Desktop/GIT%20PROJECTS/VLC/subsystem1_secure_access/ml/classifier_wrapper.cpp), [`NOVA_Secure_Lock_inferencing/`](file:///c:/Users/rskir/OneDrive/Desktop/GIT%20PROJECTS/VLC/NOVA_Secure_Lock_inferencing/) | Integrated 534-parameter 1D-CNN ($2.1\text{ KB}$ weights, $448\text{ B}$ tensor arena) targeted for $<15\text{ ms}$ inference budget on ESP32-S3 Xtensa LX7 processor. | TinyML deployment, CMSIS-NN / ESP-NN vector extensions, RAM arena optimization, neural network confidence thresholding ($\ge 85\%$). |
| **4. Timer-Driven ADC Sampling Engine** | [`subsystem1_secure_access/hal/optical_sampler.cpp`](file:///c:/Users/rskir/OneDrive/Desktop/GIT%20PROJECTS/VLC/subsystem1_secure_access/hal/optical_sampler.cpp) | Hardware `esp_timer` ISR sampling ADC at $100.0\text{ Hz}$ target interval ($T_s = 10.000\text{ ms}$) into a 100-sample float buffer. | Interrupt service routine (ISR) execution constraints (estimated $~12\ \mu\text{s}$ execution budget), zero allocation in ISR, ADC SAR non-linearity handling. |
| **5. Analog Transimpedance Amplifier (TIA)** | [`docs/HARDWARE.md`](file:///c:/Users/rskir/OneDrive/Desktop/GIT%20PROJECTS/VLC/docs/HARDWARE.md#L67), [`hardware/bom/`](file:///c:/Users/rskir/OneDrive/Desktop/GIT%20PROJECTS/VLC/hardware/bom/) | Single-supply LM358 op-amp TIA ($R_f=100\text{ k}\Omega$) paired with passive RC low-pass filter ($f_c=48.2\text{ Hz}$) for hardware overvoltage protection ($V_{\text{max}} \le 3.1\text{ V}$). | Photodiode photocurrent-to-voltage conversion, $RC$ anti-aliasing filter design, single-supply op-amp headroom bounds. |
| **6. Autonomous 2-Phase Optical Docking** | [`subsystem2_docking/subsystem2_docking.ino`](file:///c:/Users/rskir/OneDrive/Desktop/GIT%20PROJECTS/VLC/subsystem2_docking/subsystem2_docking.ino), [`docking/alignment_controller.cpp`](file:///c:/Users/rskir/OneDrive/Desktop/GIT%20PROJECTS/VLC/subsystem2_docking/docking/alignment_controller.cpp) | 2-phase scanning platform driven by 28BYJ-48 stepper motor ($4096\text{ half-steps/rev}, 0.08789^\circ/\text{step}$). 128-point coarse scan followed by fine $\pm 22.5^\circ$ peak search. | Gearbox backlash compensation math, 8-phase half-stepping unipolar motor drive, open-loop angular step tracking trade-offs. |

---

## 3. Defense Checklist for Technical Interviews

When defending project claims during technical interviews, reference the following repository artifacts:

1. **"Why use Goertzel alongside TinyML?"** $\rightarrow$ Reference [`docs/SIGNAL_PROCESSING.md`](file:///c:/Users/rskir/OneDrive/Desktop/GIT%20PROJECTS/VLC/docs/SIGNAL_PROCESSING.md) and [`docs/SECURITY_MODEL.md`](file:///c:/Users/rskir/OneDrive/Desktop/GIT%20PROJECTS/VLC/docs/SECURITY_MODEL.md). ML is probabilistic; Goertzel provides a deterministic, zero-allocation mathematical check.
2. **"How did you eliminate sampling jitter?"** $\rightarrow$ Reference [`subsystem1_secure_access/hal/optical_sampler.cpp`](file:///c:/Users/rskir/OneDrive/Desktop/GIT%20PROJECTS/VLC/subsystem1_secure_access/hal/optical_sampler.cpp#L32). Replaced software `delay()` with hardware `esp_timer` ISR.
3. **"How is the ADC protected from overvoltage?"** $\rightarrow$ Reference [`docs/HARDWARE.md`](file:///c:/Users/rskir/OneDrive/Desktop/GIT%20PROJECTS/VLC/docs/HARDWARE.md#L125). Powering LM358 from $+3.3\text{ V}$ limits max output swing to $\approx 1.8\text{ V}$, well under the $3.1\text{ V}$ ESP32-S3 ADC limit.
