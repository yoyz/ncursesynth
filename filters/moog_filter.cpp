#include "moog_filter.h"
#include <cmath>
#include <algorithm>

MoogLadderFilter::MoogLadderFilter(float rate) 
    : sampleRate(rate), fc(1000.0f), res(0.5f), drive(1.0f) {
    for (int i = 0; i < 4; i++) stage[i] = 0.0f;
    updateCoefficients();
}

MoogLadderFilter::~MoogLadderFilter() {}

inline float MoogLadderFilter::saturate(float x) {
    return 1.5f * x - 0.5f * x * x * x;
}

inline float MoogLadderFilter::clip(float x) {
    if (x > 1.0f) return 1.0f;
    if (x < -1.0f) return -1.0f;
    return x;
}

void MoogLadderFilter::updateCoefficients() {
    float wd = 2.0f * M_PI * fc;
    float t = 1.0f / sampleRate;
    float wa = (2.0f / t) * tanf(wd * t / 2.0f);
    g = wa * t / 2.0f;
    if (g < 0.0001f) g = 0.0001f;
    if (g > 1.0f) g = 1.0f;
}

void MoogLadderFilter::setCutoff(float freq) {
    fc = freq;
    if (fc < 10.0f) fc = 10.0f;
    if (fc > sampleRate * 0.45f) fc = sampleRate * 0.45f;
    updateCoefficients();
}

void MoogLadderFilter::setResonance(float resonance) {
    res = resonance;
}

void MoogLadderFilter::setSampleRate(float rate) {
    sampleRate = rate;
    updateCoefficients();
}

void MoogLadderFilter::setDrive(float driveAmount) {
    drive = driveAmount;
}

float MoogLadderFilter::process(float input) {
    float in = input * (1.0f + 1.5f * drive);
    float feedback = res * 4.0f * (stage[3] - input * 0.5f);
    
    for (int i = 0; i < 4; i++) {
        float nonlinear;
        if (i == 0) nonlinear = saturate(in + feedback - stage[0] * 0.5f);
        else nonlinear = saturate(stage[i-1] - stage[i] * 0.5f);
        stage[i] = g * (nonlinear - stage[i]) + stage[i];
        stage[i] = clip(stage[i]);
    }
    return saturate(stage[3]);
}

void MoogLadderFilter::reset() {
    for (int i = 0; i < 4; i++) stage[i] = 0.0f;
}
