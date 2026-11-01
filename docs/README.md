# N.O.V.A. Technical Documentation

This directory contains the engineering documentation for the N.O.V.A. (Navigational Optical Verification & Authentication) prototype.

---

## Documentation Index

### 1. System Architecture & Security
* [**ARCHITECTURE.md**](ARCHITECTURE.md) — System block diagrams, subsystem boundaries, interface specifications, signal flow, and Architecture Decision Records (ADR-001 through ADR-008).
* [**SECURITY_MODEL.md**](SECURITY_MODEL.md) — Threat model, physical 2FA gate logic ($L_{\text{NFC}} \land L_{\text{ML}} \land L_{\text{DSP}}$), attack vectors, and countermeasure analysis.

### 2. Hardware & Electronics
* [**HARDWARE.md**](HARDWARE.md) — Bill of Materials (BOM), pinout mapping tables, analog Transimpedance Amplifier (TIA) schematic, passive RC filter design, and electrical operating limits.

### 3. Firmware, DSP & Edge Machine Learning
* [**SIGNAL_PROCESSING.md**](SIGNAL_PROCESSING.md) — Sampling theory, Goertzel algorithm mathematical derivation, coherent integer-bin alignment ($k=10, 20, 30$), and spectral leakage bounds.
* [**TINYML.md**](TINYML.md) — Edge Impulse spectral analysis pipeline (13 features), 534-parameter Dense classifier, on-device inference budget, and C++ wrapper API.
* [**DOCKING.md**](DOCKING.md) — Autonomous optical tracking algorithm, two-phase coarse/fine angular sweep, 8-phase half-stepping, backlash compensation, and coil de-energization.

### 4. Validation, Operations & Diagnostics
* [**VALIDATION_RESULTS.md**](VALIDATION_RESULTS.md) — 30-test verification matrix with strict distinction between host simulation test harnesses and physical hardware bench measurements.
* [**KNOWN_LIMITATIONS.md**](KNOWN_LIMITATIONS.md) — Empirical operating limits, optical range boundaries, open-loop tracking constraints, and planned engineering improvements.
* [**TROUBLESHOOTING.md**](TROUBLESHOOTING.md) — Symptom-based diagnostic checklist, bench test points, common failure modes, and recovery procedures.

---

## Repository Cross-References
* Subsystem 1 Firmware: [`subsystem1_secure_access/`](../subsystem1_secure_access/)
* Subsystem 2 Firmware: [`subsystem2_docking/`](../subsystem2_docking/)
* Edge Impulse Inference SDK: [`NOVA_Secure_Lock_inferencing/`](../NOVA_Secure_Lock_inferencing/)
* Validation Suites & Test Harnesses: [`validation/`](../validation/)
* Hardware Bill of Materials: [`hardware/bom/`](../hardware/bom/)
