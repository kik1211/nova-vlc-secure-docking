# N.O.V.A. — Project Story & Engineering Conception

> **Document Status:** Official Project Origin & Engineering Background v1.0  
> **Author:** Kiruthik R S  
> **Source of Truth:** System Architecture Specification & Design History

---

## 1. Concept & Background

Physical access security systems rely heavily on Radio Frequency (RF) technologies like RFID, Bluetooth, and Wi-Fi. However, RF signals penetrate physical barriers, making them vulnerable to distant eavesdropping, relay attacks, and RF jamming. 

**N.O.V.A.** (Navigational Optical Verification & Authentication) was created to demonstrate a physically localized alternative by pairing **Near Field Communication (NFC)** with directional **Visible Light Communication (VLC)**.

---

## 2. Key Engineering Achievements

1. **Physical Line-of-Sight Security:** Modulated optical signals require line-of-sight propagation, confining access credentials within physical boundaries and eliminating RF relay/eavesdropping vectors.
2. **Dual-Verdict Access Security:** To prevent ambient light flicker or artificial light interference from causing false unlocks, a dual-verdict verification pipeline was developed. An **Edge Impulse 1D-CNN TinyML model** operates in parallel with a deterministic **Goertzel DFT algorithm**—access is granted only when both algorithms independently confirm the optical key frequency matches the NFC cardholder's role.
3. **Autonomous Docking Alignment:** Subsystem 2 provides autonomous optical alignment driven by a 28BYJ-48 stepper motor, executing a two-phase coarse/fine peak intensity search.
4. **Recognition:** Awarded **First Prize at College Project Expo** for embedded system innovation, signal processing rigor, and TinyML integration.
