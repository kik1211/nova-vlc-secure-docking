# N.O.V.A. Security & Threat Model Specification

## 1. Physical Security Architecture & 2FA Model

N.O.V.A. enforces physical Two-Factor Authentication (2FA) by requiring credential agreement across two physically independent modalities:

$$\text{Access Granted} = L_{\text{NFC}} \land L_{\text{ML}} \land L_{\text{DSP}}$$

```text
[NFC Card Presented] ──► nfc_pollCard() ──► nfc_resolveRole()
                                                │
                                                ▼ Valid Card Role
[Modulated LED Light] ──► optical_sampler ──► 100-Sample Buffer (100 Hz)
                                                │
                     ┌──────────────────────────┴──────────────────────────┐
                     ▼                                                     ▼
           classifier_run() (TinyML)                             goertzel_analyze() (DSP)
                     │                                                     │
                     ▼                                                     ▼
           ML Label == Expected Role?                             Dominant Freq == Expected Freq?
           ML Confidence >= 0.85?                                 Dominance Ratio >= 2.0?
                     │                                                     │
                     └──────────────────────────┬──────────────────────────┘
                                                ▼
                                      {ALL CONDITIONS TRUE}
                                                │
                                      [Assert Relay Unlock]
```

### 1.1 Dual-Verdict Verification Logic

1. **Layer 1 (Possession):** The user taps a 13.56 MHz MIFARE Classic NFC card. The PN532 reader retrieves the 4-byte UID. The firmware compares the UID against the role table in [`subsystem1_secure_access/config.h`](../subsystem1_secure_access/config.h) to resolve the role (`ADMIN`, `STAFF`, or `GUEST`). An unregistered UID immediately locks out the system for $2.0\text{ s}$ (`NFC_DENY_LOCKOUT_MS`).
2. **Layer 2a (TinyML Inference):** The optical sampler captures 100 samples of incident light at $100.0\text{ Hz}$. The Edge Impulse neural network computes class probabilities. The winning class must match the role's expected frequency label (`freq_30hz` for Admin, `freq_20hz` for Staff, `freq_10hz` for Guest) with confidence $\ge 0.85$.
3. **Layer 2b (Goertzel DSP Verification):** In parallel, the Goertzel algorithm calculates magnitude squared values for $k=10, 20, 30$. The dominant frequency must match the expected role frequency, and the Dominance Ratio ($\frac{M_{\text{dominant}}}{\max(M_{\text{other}})}$) must be $\ge 2.0$.
4. **Relay Actuation:** Only when all three conditions evaluate to `true` does the state machine transition to `STATE_UNLOCK`, asserting GPIO 4 HIGH for $5.0\text{ s}$ (`DOOR_OPEN_DURATION_MS`).

---

## 2. Threat Matrix & Countermeasure Analysis

| Threat Vector | Attack Scenario | Countermeasure & Defensive Behavior | System Outcome |
|:---|:---|:---|:---:|
| **Card Cloning / Replay** | Attacker clones 4-byte NFC UID to an unauthorized tag. | Layer 2 optical key is required; NFC alone cannot trigger solenoid unlock. | **Blocked** |
| **Mismatched Optical Key** | Valid NFC card presented with wrong frequency key (e.g., Admin card + 10 Hz LED). | Dual-verdict gate detects frequency/role mismatch (`AUTH_REASON_GOERTZEL_MISMATCH`). | **Blocked** |
| **High-Intensity DC Flashlight** | Attacker shines an unmodulated high-intensity flashlight into photodiode. | Goertzel AC energy is near zero; dominance ratio $< 2.0$ triggers `AUTH_REASON_GOERTZEL_AMBIGUOUS`. | **Blocked** |
| **Ambient Light Flicker ($100\text{ Hz}$)** | Artificial fluorescent lighting flickers into receiver. | Passive RC filter attenuates $100\text{ Hz}$ flicker by $>14\text{ dB}$; TinyML classifies ambient flicker as `noise` ($>99\%$ confidence). | **Blocked** |
| **Photodiode Saturation Attack** | Laser or focused beam aimed at receiver to trigger overvoltage. | LM358 powered from $3.3\text{ V}$ rail caps maximum output swing at $\approx 1.8\text{ V}$. ADC saturation threshold ($>3890\text{ counts}$) triggers safety veto. | **Blocked** |
| **RF Sniffing / Jamming** | Attacker sniffs or jams RF signals from a distance. | VLC signals require direct optical line-of-sight and do not penetrate opaque walls or doors. | **Blocked** |

---

## 3. Implementation Constraints & Production Hardening

* **Static Credential Storage:** In the research prototype, registered card UIDs are compiled as constants in [`subsystem1_secure_access/config.h`](../subsystem1_secure_access/config.h). Production deployments should store credentials in ESP32-S3 Flash encrypted via NVS Flash Encryption (AES-XTS-256).
* **Cryptographic NFC Protocols:** The prototype uses 4-byte UIDs from MIFARE Classic cards. Production security can be hardened by upgrading to MIFARE DESFire EV3 with 128-bit AES challenge-response authentication.
* **Rolling Optical Keys:** Future revisions can replace static frequency tones ($10, 20, 30\text{ Hz}$) with pseudo-random rolling Frequency-Shift Keying (FSK) optical tokens synchronized between a smartphone app and lock.
