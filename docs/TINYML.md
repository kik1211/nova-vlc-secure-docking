# N.O.V.A. TinyML Model & Edge Impulse Integration

> **Document Status:** Official TinyML Specification v1.0  
> **Source of Truth:** Design Freeze Specification §6, `ml/classifier_wrapper.cpp`, and `validation/results/optical_test_results.md`

---

## 1. Overview & Machine Learning Pipeline

Subsystem 1 uses an on-device Edge Impulse neural network to perform real-time optical frequency classification directly on the ESP32-S3 microcontroller.

```text
100 ADC Samples (1.000 s @ 100 Hz)
            │
            ▼
┌───────────────────────┐
│   Signal Normalization│  Scale raw 12-bit ADC (0-4095) to float
└───────────┬───────────┘
            │
            ▼
┌───────────────────────┐
│   1D-CNN Model        │  Conv1D (8 filters) -> ReLU -> MaxPooling -> Dense (4 units)
└───────────┬───────────┘
            │
            ▼
┌───────────────────────┐  Class Probabilities:
│   Softmax Output      │  [freq_10hz, freq_20hz, freq_30hz, noise]
└───────────┬───────────┘
            │
            ▼
┌───────────────────────┐
│  Confidence Gate      │  Require Winning Class Confidence >= 0.85 (85%)
└───────────────────────┘
```

---

## 2. Neural Network Architecture

| Layer | Type | Configuration / Output Shape | Activation | Parameters |
|:---|:---|:---|:---|:---:|
| **Input** | Raw Time Series | $100\text{ samples} \times 1\text{ channel}$ | — | 0 |
| **Conv1D** | 1D Convolution | 8 filters, kernel size 3, stride 1 | ReLU | 32 |
| **MaxPool1D** | Max Pooling | Pool size 2, stride 2 | — | 0 |
| **Conv1D-2** | 1D Convolution | 16 filters, kernel size 3, stride 1 | ReLU | 400 |
| **Flatten** | Reshape | 784 units | — | 0 |
| **Dense** | Fully Connected | 16 neurons | ReLU | 12,560 |
| **Output** | Dense Softmax | 4 classes (`freq_10hz`, `freq_20hz`, `freq_30hz`, `noise`) | Softmax | 68 |
| **Total** | | | | **13,060** |

---

## 3. Classifier Integration API (`classifier_wrapper.cpp`)

The application layer interacts with the model exclusively through `classifier_run()`:

```cpp
ClassifierResult_t res;
bool ok = classifier_run(sampleBuffer, 100, &res);
```

### Performance Characteristics on ESP32-S3
* **Inference Latency:** $2,140\ \mu\text{s}$ ($2.14\text{ ms}$) leveraging ESP32-S3 Vector Extension SIMD instructions.
* **RAM Footprint:** $4.2\text{ KB}$ tensor arena RAM.
* **Flash Footprint:** $42.8\text{ KB}$ compiled weights and CMSIS-NN kernels.
* **Confidence Gate Threshold:** $\ge 85.0\%$ required for 2FA pass.
