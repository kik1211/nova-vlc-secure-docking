"""
test_auth_fsm.py
Automated Python verification script for N.O.V.A. Two-Factor Authentication FSM & Dual-Verdict Gate.
"""

import sys

# Registered Credentials (from config.h)
ADMIN_UID = [0xAE, 0x9A, 0xF9, 0x06]
STAFF_UID = [0x03, 0x5F, 0x1A, 0x32]
GUEST_UID = [0xDE, 0xB0, 0x29, 0x07]

ML_CONFIDENCE_THRESHOLD = 0.85

ROLES = {
    'ADMIN': {'uid': ADMIN_UID, 'expected_label': 'freq_30hz', 'expected_freq': 30},
    'STAFF': {'uid': STAFF_UID, 'expected_label': 'freq_20hz', 'expected_freq': 20},
    'GUEST': {'uid': GUEST_UID, 'expected_label': 'freq_10hz', 'expected_freq': 10},
}

class AuthFSM:
    def __init__(self):
        self.state = "IDLE"
        self.last_reason = "NONE"
        self.scanned_role = None
        self.unlocked = False

    def resolve_role(self, uid):
        for role, data in ROLES.items():
            if uid == data['uid']:
                return role
        return "UNKNOWN"

    def evaluate_2fa_gate(self, role, ml_label, ml_confidence, goertzel_freq, goertzel_found):
        expected_label = ROLES[role]['expected_label']
        expected_freq = ROLES[role]['expected_freq']

        ml_label_match = (ml_label == expected_label)
        ml_conf_pass = (ml_confidence >= ML_CONFIDENCE_THRESHOLD)
        goertzel_match = goertzel_found and (goertzel_freq == expected_freq)

        gate_pass = ml_label_match and ml_conf_pass and goertzel_match

        if gate_pass:
            return "SUCCESS", True
        else:
            if not ml_label_match:
                return "ML_WRONG_LABEL", False
            elif not ml_conf_pass:
                return "ML_LOW_CONFIDENCE", False
            elif not goertzel_found:
                return "GOERTZEL_AMBIGUOUS", False
            else:
                return "GOERTZEL_MISMATCH", False

def run_fsm_tests():
    print("==================================================")
    print("   AUTH FSM & DUAL-VERDICT GATE VERIFICATION")
    print("==================================================")

    fsm = AuthFSM()
    all_pass = True

    tests = [
        # 1. Admin valid 2FA pass
        {
            'name': 'SEC-01a: Admin Valid 2FA (30 Hz ML + 30 Hz Goertzel)',
            'uid': ADMIN_UID, 'ml_label': 'freq_30hz', 'ml_conf': 0.95, 'g_freq': 30, 'g_found': True,
            'expected_reason': 'SUCCESS', 'expected_unlock': True
        },
        # 2. Staff valid 2FA pass
        {
            'name': 'SEC-01b: Staff Valid 2FA (20 Hz ML + 20 Hz Goertzel)',
            'uid': STAFF_UID, 'ml_label': 'freq_20hz', 'ml_conf': 0.91, 'g_freq': 20, 'g_found': True,
            'expected_reason': 'SUCCESS', 'expected_unlock': True
        },
        # 3. Guest valid 2FA pass
        {
            'name': 'SEC-01c: Guest Valid 2FA (10 Hz ML + 10 Hz Goertzel)',
            'uid': GUEST_UID, 'ml_label': 'freq_10hz', 'ml_conf': 0.88, 'g_freq': 10, 'g_found': True,
            'expected_reason': 'SUCCESS', 'expected_unlock': True
        },
        # 4. Unknown NFC Card Rejection
        {
            'name': 'SEC-02: Unregistered NFC UID Rejection',
            'uid': [0x11, 0x22, 0x33, 0x44], 'ml_label': 'freq_30hz', 'ml_conf': 0.99, 'g_freq': 30, 'g_found': True,
            'expected_reason': 'UNKNOWN_CARD', 'expected_unlock': False
        },
        # 5. ML Low Confidence Veto
        {
            'name': 'SEC-03: ML Low Confidence Veto (0.75 < 0.85 Threshold)',
            'uid': ADMIN_UID, 'ml_label': 'freq_30hz', 'ml_conf': 0.75, 'g_freq': 30, 'g_found': True,
            'expected_reason': 'ML_LOW_CONFIDENCE', 'expected_unlock': False
        },
        # 6. Goertzel Mismatch Veto
        {
            'name': 'SEC-04: Goertzel Mismatch Veto (Admin card with 10 Hz optical key)',
            'uid': ADMIN_UID, 'ml_label': 'freq_30hz', 'ml_conf': 0.95, 'g_freq': 10, 'g_found': True,
            'expected_reason': 'GOERTZEL_MISMATCH', 'expected_unlock': False
        },
        # 7. Goertzel Ambiguous Veto
        {
            'name': 'SEC-05: Goertzel Ambiguous Signal Veto',
            'uid': STAFF_UID, 'ml_label': 'freq_20hz', 'ml_conf': 0.92, 'g_freq': 0, 'g_found': False,
            'expected_reason': 'GOERTZEL_AMBIGUOUS', 'expected_unlock': False
        },
    ]

    for t in tests:
        print(f"\nTest: {t['name']}")
        role = fsm.resolve_role(t['uid'])
        if role == "UNKNOWN":
            reason, unlock = "UNKNOWN_CARD", False
        else:
            reason, unlock = fsm.evaluate_2fa_gate(role, t['ml_label'], t['ml_conf'], t['g_freq'], t['g_found'])

        pass_reason = (reason == t['expected_reason'])
        pass_unlock = (unlock == t['expected_unlock'])
        passed = pass_reason and pass_unlock

        print(f"  Role Resolved:   {role}")
        print(f"  Outcome Reason:  {reason} (Expected: {t['expected_reason']})")
        print(f"  Door State:      {'UNLOCKED' if unlock else 'LOCKED'} (Expected: {'UNLOCKED' if t['expected_unlock'] else 'LOCKED'})")
        print(f"  Test Result:     {'PASS' if passed else 'FAIL'}")

        if not passed:
            all_pass = False

    print("\n==================================================")
    print(f"   FINAL SUITE RESULT: {'ALL IMPLEMENTED HARNESS TESTS PASSED' if all_pass else 'TEST SUITE FAILED'}")
    print("==================================================")
    return all_pass

if __name__ == "__main__":
    success = run_fsm_tests()
    sys.exit(0 if success else 1)
