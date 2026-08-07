# Security Policy

## Scope

N.O.V.A. is a **proof-of-concept prototype** built for educational and portfolio purposes.
It is **not a production security system**. Please read the limitations below before
using any part of this system in a real access control deployment.

---

## Known Prototype Limitations

The following are **documented, understood limitations** of the current prototype:

| Limitation | Severity | Production Mitigation |
|:---|:---|:---|
| NFC UID cloning trivially bypasses NFC factor | High | MIFARE DeSFire EV2 mutual authentication |
| Optical channel is vulnerable to replay attacks | High | TOTP-based rolling frequency keys |
| NFC UIDs stored in firmware plaintext | Medium | NVS encrypted key storage |
| Only 3 optical frequencies (brute-force surface) | Medium | Expanded frequency space with encoding |
| No failed-attempt rate limiting beyond a fixed delay | Low | Exponential backoff with alarm |

For a detailed threat model, see [docs/SECURITY_MODEL.md](docs/SECURITY_MODEL.md).

---

## Reporting a Security Issue

If you identify a security issue in this repository's code:

1. **Do not open a public GitHub issue.** Use private disclosure.
2. Email the repository owner directly (see GitHub profile).
3. Provide: description, steps to reproduce, and potential impact.
4. Allow 14 days for a response before public disclosure.

Security issues in the **Edge Impulse generated library** should be reported to
[Edge Impulse](https://edgeimpulse.com/security) directly.

---

## Supported Versions

| Version | Supported |
|:---|:---|
| v1.0.x (when released) | ✅ Yes |
| v0.x.x (pre-release) | ❌ No — prototype only |
