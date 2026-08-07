# N.O.V.A. — Knowledge Base Master Guide

> **Single Entry Point & Study Roadmap**  
> Navigational Optical Verification & Authentication (N.O.V.A.)

---

## 1. Overview & Purpose

This Knowledge Base is a reference educational repository for the **N.O.V.A.** embedded systems, TinyML, and Visible Light Communication (VLC) project. It serves three primary engineering purposes:

1. **Academic Reference:** Deep mathematical derivations, hardware schematics, and signal processing theory.
2. **Technical Interview Defense:** Detailed technical context to defend design decisions during engineering interviews.
3. **Open-Source Documentation:** Structured guide for developers understanding embedded TinyML, DSP, and optical communications.

---

## 2. Knowledge Base Directory Map

```text
knowledge-base/
├── MASTER_GUIDE.md                    ← Single entry point and study roadmap
├── DESIGN_DECISIONS.md                ← Decision log & alternatives (ADRs)
│
├── 01_SYSTEM/                         ← Architecture & subsystem boundaries
├── 02_HARDWARE/                       ← Component-level datasheets & pinouts
├── 03_ELECTRONICS/                    ← TIA, photodiode, RC filter derivations
├── 04_ESP32/                          ← ESP32/ESP32-S3 peripheral & memory map
├── 05_EMBEDDED/                       ← Non-blocking FSM & esp_timer patterns
├── 06_SIGNAL_PROCESSING/              ← Sampling theory & DFT fundamentals
├── 07_FFT/                            ← FFT algorithms & Edge Impulse pipeline
├── 08_GOERTZEL/                       ← Mathematical derivation of Goertzel DFT
├── 09_TINYML/                         ← Neural network architecture & inference
├── 10_VLC/                            ← Visible Light Communication theory
├── 11_SECURITY/                       ← Threat model & 2FA dual-verdict gate
├── 12_DOCKING/                        ← Two-phase optical alignment & stepper control
├── 13_DEBUGGING/                      ← Serial logic analyzer & oscilloscopes
├── 14_FAILURE_ANALYSIS/               ← FMEA & noise mitigation
├── 15_EXPERIMENT_LOG/                 ← Lab notebook & historical prototype logs
├── 16_CHEATSHEETS/                    ← Quick-reference one-pagers
├── 17_GLOSSARY/                       ← Technical glossary of terms
├── 18_INTERVIEW_QA/                   ← Multi-tier technical interview bank
├── 19_VIVA_QA/                        ← Academic viva defense Q&A
├── 20_PRESENTATION/                   ← Presentation slide outlines & scripts
├── 21_PATENT_NOTES/                   ← Intellectual property & novelty notes
└── 22_STUDY_ROADMAP/                  ← Learning roadmap for engineering students
```

---

## 3. Quick-Start Checklists

### Before a Technical Interview
* Read [`PROJECT_STORY.md`](PROJECT_STORY.md) for the 60-second narrative pitch.
* Review [`DESIGN_DECISIONS.md`](DESIGN_DECISIONS.md) to defend why two microcontrollers were used instead of one.
* Study [`18_INTERVIEW_QA/interview_bank.md`](18_INTERVIEW_QA/interview_bank.md) for Goertzel vs. FFT complexity trade-offs ($O(N)$ vs. $O(N \log N)$).

### Before an Academic Viva / Defense
* Review [`03_ELECTRONICS/`](03_ELECTRONICS/) for the LM358 TIA gain derivation ($V_{\text{out}} = I_{\text{pd}} \cdot R_f$).
* Review [`08_GOERTZEL/`](08_GOERTZEL/) for why zero spectral leakage occurs at $N=100, f_s=100\text{ Hz}$.
* Review [`19_VIVA_QA/`](19_VIVA_QA/) for faculty defense strategy.
