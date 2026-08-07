"""
test_goertzel.py
Automated Python verification script for N.O.V.A. Goertzel DSP Engine.
Tests the exact Goertzel algorithm recurrence against synthetic optical signals.
"""

import math

SAMPLE_COUNT = 100
SAMPLING_FREQ_HZ = 100.0
MIN_DOMINANCE_RATIO = 2.0
TARGET_FREQS = [10.0, 20.0, 30.0]

class TestVectorSet:
    """Structured container for automated test vectors (AUD-014)."""
    @staticmethod
    def generate_sine(freq_hz, amplitude=1000.0, dc_bias=2000.0, sample_count=SAMPLE_COUNT, fs=SAMPLING_FREQ_HZ):
        return [dc_bias + amplitude * math.sin(2.0 * math.pi * freq_hz * i / fs) for i in range(sample_count)]

    @staticmethod
    def generate_square(freq_hz, low=1000.0, high=3000.0, sample_count=SAMPLE_COUNT, fs=SAMPLING_FREQ_HZ):
        samples = []
        for i in range(sample_count):
            t = i / fs
            period = 1.0 / freq_hz
            phase = (t % period) / period
            val = high if phase < 0.5 else low
            samples.append(val)
        return samples

def goertzel_magnitude(samples, sample_rate, target_freq):
    n = len(samples)
    k = target_freq / sample_rate * n
    omega = 2.0 * math.pi * k / n
    coeff = 2.0 * math.cos(omega)

    s1 = 0.0
    s2 = 0.0
    for x in samples:
        s0 = x + coeff * s1 - s2
        s2 = s1
        s1 = s0

    mag_sq = s1 * s1 + s2 * s2 - coeff * s1 * s2
    return mag_sq

def goertzel_analyze(samples, sample_rate=SAMPLING_FREQ_HZ):
    mags = [goertzel_magnitude(samples, sample_rate, f) for f in TARGET_FREQS]
    m_10, m_20, m_30 = mags

    max_idx = 0
    for i in range(1, 3):
        if mags[i] > mags[max_idx]:
            max_idx = i

    second_max = 0.0
    for i in range(3):
        if i != max_idx and mags[i] > second_max:
            second_max = mags[i]

    dominant_found = False
    dominant_freq = 0

    if mags[max_idx] > 0.0 and second_max > 0.0:
        ratio = mags[max_idx] / second_max
        if ratio >= MIN_DOMINANCE_RATIO:
            dominant_found = True
            dominant_freq = int(TARGET_FREQS[max_idx])
    elif mags[max_idx] > 0.0 and second_max == 0.0:
        dominant_found = True
        dominant_freq = int(TARGET_FREQS[max_idx])

    return {
        'mag_10': m_10,
        'mag_20': m_20,
        'mag_30': m_30,
        'dominant_found': dominant_found,
        'dominant_freq': dominant_freq
    }

def generate_sine(freq, amplitude=500.0, offset=2048.0):
    return [
        offset + amplitude * math.sin(2.0 * math.pi * freq * (i / SAMPLING_FREQ_HZ))
        for i in range(SAMPLE_COUNT)
    ]

def generate_square(freq, amplitude=500.0, offset=2048.0):
    """Square wave optical modulation (LED 50% duty cycle ON/OFF)."""
    return [
        offset + amplitude * (1.0 if math.sin(2.0 * math.pi * freq * (i / SAMPLING_FREQ_HZ)) >= 0 else -1.0)
        for i in range(SAMPLE_COUNT)
    ]

def generate_signal_with_mains(freq, mains_freq=50.0, mains_amp=150.0, amplitude=500.0, offset=2048.0):
    """Optical signal corrupted by 50 Hz AC mains lighting flicker."""
    return [
        offset + amplitude * math.sin(2.0 * math.pi * freq * (i / SAMPLING_FREQ_HZ))
        + mains_amp * math.sin(2.0 * math.pi * mains_freq * (i / SAMPLING_FREQ_HZ))
        for i in range(SAMPLE_COUNT)
    ]

def run_tests():
    print("==================================================")
    print("   GOERTZEL DSP AUTOMATED MATHEMATICAL VERIFICATION")
    print("==================================================")

    all_pass = True

    # 1. Ideal Sine Wave Tests
    print("\n--- TEST GROUP 1: Ideal Sine Wave Signals ---")
    for target_f in [10.0, 20.0, 30.0]:
        sig = generate_sine(target_f)
        res = goertzel_analyze(sig)

        print(f"Testing Input Signal: {target_f} Hz Sine")
        print(f"  Mag 10Hz: {res['mag_10']:12.1f} | Mag 20Hz: {res['mag_20']:12.1f} | Mag 30Hz: {res['mag_30']:12.1f}")
        print(f"  Dominant: {res['dominant_freq']} Hz (Found: {res['dominant_found']})")

        other_mags = [res[k] for k in ['mag_10', 'mag_20', 'mag_30'] if k != f"mag_{int(target_f)}"]
        leakage_clean = all(m < 1e-4 for m in other_mags)
        correct_freq = res['dominant_found'] and res['dominant_freq'] == int(target_f)
        passed = leakage_clean and correct_freq

        print(f"  Coherent Integer-Bin Check: {'PASS' if leakage_clean else 'FAIL'}")
        print(f"  Overall Status:             {'PASS' if passed else 'FAIL'}")
        if not passed:
            all_pass = False

    # 2. Realistic Square-Wave Optical Modulation Tests
    print("\n--- TEST GROUP 2: Square-Wave Optical Modulation (LED ON/OFF) ---")
    for target_f in [10.0, 20.0, 30.0]:
        sig = generate_square(target_f)
        res = goertzel_analyze(sig)

        print(f"Testing Square-Wave Optical Signal: {target_f} Hz")
        print(f"  Mag 10Hz: {res['mag_10']:12.1f} | Mag 20Hz: {res['mag_20']:12.1f} | Mag 30Hz: {res['mag_30']:12.1f}")
        print(f"  Dominant: {res['dominant_freq']} Hz (Found: {res['dominant_found']})")

        correct_freq = res['dominant_found'] and res['dominant_freq'] == int(target_f)
        print(f"  Square Wave Detection Status: {'PASS' if correct_freq else 'FAIL'}")
        if not correct_freq:
            all_pass = False

    # 3. Frequency Offset Tolerance Tests (±0.5% to ±2.5% Oscillator Tolerance)
    print("\n--- TEST GROUP 3: Frequency Offset Tolerance Tests (±0.5% to ±2.5%) ---")
    for target_f in [10.0, 20.0, 30.0]:
        for offset_pct in [-2.5, -2.0, -1.0, -0.5, 0.5, 1.0, 2.0, 2.5]:
            actual_f = target_f * (1.0 + offset_pct / 100.0)
            sig = generate_square(actual_f)
            res = goertzel_analyze(sig)
            detected = res['dominant_found'] and res['dominant_freq'] == int(target_f)
            print(f"  Target {target_f:2.0f} Hz with Offset {offset_pct:+4.1f}% ({actual_f:5.2f} Hz): Detected={res['dominant_freq']} Hz | {'PASS' if detected else 'FAIL'}")
            if not detected:
                all_pass = False

    # 4. DC Offset & Ambient Light Shift Tests
    print("\n--- TEST GROUP 4: DC Offset & Ambient Light Bias Tests ---")
    for dc_bias in [500.0, 1500.0, 3000.0]:
        sig = generate_square(20.0, amplitude=400.0, offset=dc_bias)
        res = goertzel_analyze(sig)
        detected = res['dominant_found'] and res['dominant_freq'] == 20
        print(f"  20 Hz Square Signal with DC Offset {dc_bias:.0f} counts: Detected={res['dominant_freq']} Hz | {'PASS' if detected else 'FAIL'}")
        if not detected:
            all_pass = False

    # 5. 50 Hz AC Mains Lighting Interference Tests
    print("\n--- TEST GROUP 5: 50 Hz AC Mains Lighting Interference ---")
    for target_f in [10.0, 20.0, 30.0]:
        sig = generate_signal_with_mains(target_f, mains_freq=50.0, mains_amp=200.0)
        res = goertzel_analyze(sig)
        detected = res['dominant_found'] and res['dominant_freq'] == int(target_f)
        print(f"  Target {target_f:2.0f} Hz with 50 Hz Mains Flicker (200ct amp): Detected={res['dominant_freq']} Hz | {'PASS' if detected else 'FAIL'}")
        if not detected:
            all_pass = False

    # 6. Noise Rejection & Low-SNR Tests
    print("\n--- TEST GROUP 6: Broadband Noise Rejection & Low SNR ---")
    import random
    random.seed(42)
    noise_sig = [2048.0 + random.uniform(-100, 100) for _ in range(SAMPLE_COUNT)]
    noise_res = goertzel_analyze(noise_sig)

    print(f"  Broadband Noise (±100ct): Dominant Found={noise_res['dominant_found']} | {'PASS' if not noise_res['dominant_found'] else 'FAIL'}")
    if noise_res['dominant_found']:
        all_pass = False

    print("\n==================================================")
    print(f"   FINAL SUITE RESULT: {'ALL IMPLEMENTED TEST HARNESS CASES PASSED' if all_pass else 'TEST SUITE FAILED'}")
    print("==================================================")
    return all_pass

if __name__ == "__main__":
    success = run_tests()
    exit(0 if success else 1)

