# Changelog

All notable changes to N.O.V.A. will be documented in this file.

Format follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).
This project uses [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased]

### Added
- Full Phase 2 implementation & verification: Subsystem 1 (HAL, Goertzel DSP, TinyML, 2FA FSM) and Subsystem 2 (Docking Stepper & Alignment Controller) (Phases 2.1–2.7)
- Automated verification test suite: `test_goertzel.py` & `test_auth_fsm.py`
- Validation test harnesses: `test_ss1_hal.ino`, `test_goertzel.ino`, `test_classifier.ino`, `test_auth_fsm.ino`, `test_docking_stepper.ino`
- Hardware, Signal Processing, Optical, Firmware, Security, and Docking validation matrices in `validation/results/`
- Implementation Specification v1.0
- System Design Freeze Specification v1.0
- Engineering Feasibility Review (Phase 0)
- Repository Blueprint v1.0

---

## [0.0.1] — 2026-08-07

### Added
- Initial repository created from `nova_final.ino` prototype
- Original monolithic firmware archived at root level
- Edge Impulse generated library `NOVA_Secure_Lock_inferencing` integrated

### Notes
- Original firmware is a working prototype; refactored modular firmware begins at v0.1.0
- First Prize — College Project Expo (prototype demonstrated on this firmware version)
