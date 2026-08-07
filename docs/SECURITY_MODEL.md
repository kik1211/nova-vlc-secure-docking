# N.O.V.A. Security & Threat Model Specification

> **Document Status:** Official Security Model v1.0  
> **Source of Truth:** Design Freeze Specification §8, `auth/auth_state_machine.cpp`, and `validation/results/security_test_results.md`

---

## 1. Physical Security Architecture & 2FA Model

N.O.V.A. implements a physical **Two-Factor Authentication (2FA)** gate combining two independent security modalities:

$$\text{Access Granted} = \text{Layer 1 (NFC Possession)} \land \text{Layer 2a (ML Optical Classification)} \land \text{Layer 2b (Goertzel DSP Magnitude)}$$

```text
[NFC Card Present] ──► nfc_pollCard() ──► nfc_resolveRole()
                                              │
                                              ▼ Valid Card Role
[Modulated LED Light] ──► optical_sampler ──► 100 Hz Float Buffer
                                              │
                   ┌──────────────────────────┴──────────────────────────┐
                   ▼                                                     ▼
        classifier_run() (TinyML)                              goertzel_analyze() (DSP)
                   │                                                     │
                   ▼                                                     ▼
        ML Label == Expected Role?                             Dominant Freq == Expected Freq?
        ML Confidence >= 0.85?                                 Dominance Ratio >= 2.0?
                   │                                                     │
                   └──────────────────────────┬──────────────────────────┘
                                              ▼
                                    {BOTH MUST BE TRUE}
                                              │
                                     [ASSERT RELAY UNLOCK]
```

---

## 2. Threat Matrix & Countermeasure Analysis

| Threat Vector | Attack Description | Countermeasure / Security Defense | Result |
|:---|:---|:---|:---:|
| **Card Cloning / Replay** | Attacker clones 4-byte NFC UID to unauthorized card | Layer 2 optical key required; NFC alone cannot trigger solenoid | **BLOCKED** |
| **Wrong Optical Key** | Valid NFC holder presents wrong frequency (e.g., Admin card + 10 Hz LED) | Dual-verdict gate detects label/frequency mismatch (`AUTH_REASON_GOERTZEL_MISMATCH`) | **BLOCKED** |
| **High-Intensity DC Flashlight** | Attacker shines high-intensity unmodulated flashlight at photodiode | Goertzel AC energy is zero; dominance ratio $<2.0$ yields `GOERTZEL_AMBIGUOUS` veto | **BLOCKED** |
| **Ambient Light Flicker ($100\text{ Hz}$)** | Artificial fluorescent lighting flickers into receiver | Edge Impulse model classifies AC ambient flicker as `noise` ($>99\%$ confidence) | **BLOCKED** |
| **Photodiode Saturation Attack** | Laser blinded photodiode aiming for overvoltage lock trigger | LM358 powered from $3.3\text{ V}$ caps output at $3.1\text{ V}$; ADC saturation flag triggers deny | **BLOCKED** |

---

## 3. Prototype Limitations & Future Cryptographic Roadmap

### Prototype Credentials (Static Memory)
For the prototype implementation, registered NFC UIDs are compiled as static byte arrays in `subsystem1_secure_access/config.h`.

### Production Security Enhancements (Future Roadmap)
1. **NVS Encrypted Credential Store:** Store UIDs in ESP32-S3 Flash encrypted via NVS Flash Encryption (AES-XTS-256).
2. **MIFARE AES Cryptographic Authentication:** Upgrade from 4-byte UID MIFARE Classic to MIFARE DESFire EV3 with 128-bit AES challenge-response mutual authentication.
3. **Dynamic VLC Challenge-Response:** Replace static optical frequencies ($10\text{ Hz}, 20\text{ Hz}, 30\text{ Hz}$) with rolling pseudo-random frequency shift keying (FSK) optical tokens synchronized between smartphone transmitter app and lock.
