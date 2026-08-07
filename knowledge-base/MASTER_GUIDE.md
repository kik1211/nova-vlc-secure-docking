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

## 2. Knowledge Base Modules & Documentation Index

| Module | Description & Focus | Target Audience |
|:---|:---|:---|
| [`PROJECT_STORY.md`](PROJECT_STORY.md) | Origin story, motivation, and 60-second elevator pitch | Recruiters & Hiring Managers |
| [`DESIGN_DECISIONS.md`](DESIGN_DECISIONS.md) | Architecture Decision Records (ADR-001 through ADR-008) capturing key technical trade-offs | Embedded Engineers & Systems Architects |
| [`INTERVIEW_BANK.md`](INTERVIEW_BANK.md) | Structured technical Q&A across Recruiter, Firmware, DSP/ML, and Security categories | Technical Interviewers & Candidates |
| [`docs/ARCHITECTURE.md`](../docs/ARCHITECTURE.md) | System boundaries, interface tables, and subsystem independence specs | Systems Engineers |
| [`docs/HARDWARE.md`](../docs/HARDWARE.md) | Component BOM, pinout tables, TIA schematic, and electrical operating limits | Hardware & Electronics Engineers |
| [`docs/SIGNAL_PROCESSING.md`](../docs/SIGNAL_PROCESSING.md) | 100 Hz sampling theory, Goertzel IIR DFT mathematical derivation, and spectral leakage proofs | DSP & Signal Processing Engineers |
| [`docs/TINYML.md`](../docs/TINYML.md) | 1D-CNN network architecture, parameter footprint, and Edge Impulse SDK integration | TinyML & Edge AI Engineers |
| [`docs/DOCKING.md`](../docs/DOCKING.md) | Two-phase coarse/fine optical alignment algorithm, motor kinematics, and backlash math | Robotics & Control Engineers |
| [`docs/SECURITY_MODEL.md`](../docs/SECURITY_MODEL.md) | Threat model, attack vectors, and dual-verdict veto authentication state machine | Embedded Security Engineers |

---

## 3. Quick-Start Checklists

### Before a Technical Interview
* Read [`PROJECT_STORY.md`](PROJECT_STORY.md) for the 60-second narrative pitch.
* Review [`DESIGN_DECISIONS.md`](DESIGN_DECISIONS.md) to defend why two microcontrollers were used instead of one (ADR-001).
* Study [`INTERVIEW_BANK.md`](INTERVIEW_BANK.md) for Goertzel vs. FFT complexity trade-offs ($O(N)$ vs. $O(N \log N)$).

### Before an Architecture Review
* Review [`docs/HARDWARE.md`](../docs/HARDWARE.md) for the LM358 TIA gain derivation ($V_{\text{out}} = I_{\text{pd}} \cdot R_f$).
* Review [`docs/SIGNAL_PROCESSING.md`](../docs/SIGNAL_PROCESSING.md) for why zero spectral leakage occurs at $N=100, f_s=100\text{ Hz}$.
* Review [`docs/SECURITY_MODEL.md`](../docs/SECURITY_MODEL.md) for the dual-verdict authentication gate logic.
