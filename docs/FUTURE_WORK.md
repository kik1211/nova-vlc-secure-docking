# Future Work

> Planned improvements and research directions for future versions of N.O.V.A.

<!-- STATUS: Placeholder — to be completed in Phase 3. -->

---

## Production Security Hardening

- MIFARE DESFire EV2 mutual authentication (replaces UID-only NFC)
- TOTP-based rolling optical frequency keys (eliminates replay attacks)
- NVS encrypted key storage (replaces plaintext UIDs in firmware)
- Exponential backoff on failed authentication attempts

## Signal Processing Improvements

- Retrain Edge Impulse model with 64-point or 128-point FFT
- Implement windowing function (Hann) to reduce spectral leakage
- ADC calibration using `esp_adc_cal_characterize()` for improved linearity
- Higher sampling rate (200–500 Hz) for wider frequency space

## Docking System Enhancements

- Closed-loop stepper control with optical encoder feedback
- Multi-axis alignment (azimuth + elevation)
- FSK-modulated beacon identification
- Real-time tracking (continuous alignment, not single-scan)

## Platform Improvements

- Quantized (int8) NN model for faster inference
- FreeRTOS task-based architecture with proper priorities
- BLE or Wi-Fi audit log export
- OTA firmware update capability

---

## Document Revision

| Version | Date | Change |
|:---|:---|:---|
| 0.1 | 2026-08-07 | Placeholder created (Phase 2.1) |
