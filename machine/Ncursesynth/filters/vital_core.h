#ifndef VITAL_CORE_H
#define VITAL_CORE_H

#include <cmath>
#include <algorithm>

inline float vitalPassThrough(float x) { return x; }
inline float vitalTanh(float x) {
    if (x > 10.0f) return 1.0f;
    if (x < -10.0f) return -1.0f;
    float x2 = x * x;
    return x * (27.0f + x2) / (27.0f + 9.0f * x2);
}
inline float vitalHardTanh(float x) {
    return std::max(-1.0f, std::min(1.0f, x));
}
inline float vitalQuickTanh(float x) {
    if (x > 3.0f) return 1.0f;
    if (x < -3.0f) return -1.0f;
    return x * (27.0f + x * x) / (27.0f + 9.0f * x * x);
}

inline float onePoleLookup(float freq_hz, float sample_rate) {
    float scaled = freq_hz * float(M_PI) / sample_rate;
    float max_rads = 0.499f * float(M_PI);
    return tanf(std::min(max_rads, scaled / (scaled + 1.0f)));
}

inline float svfLookup(float freq_hz, float sample_rate) {
    float ratio = freq_hz / sample_rate;
    return tanf(std::min(0.499f, ratio) * float(M_PI));
}

#endif
