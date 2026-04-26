#include "high_pass_filter.h"
#include <cmath>

HighPassFilter::HighPassFilter(float rate)
    : sampleRate(rate), cutoff(1000.0f), res(0.0f), z1(0.0f), z2(0.0f) {}

void HighPassFilter::setCutoff(float freq) {
    cutoff = freq;
}

void HighPassFilter::setResonance(float r) {
    res = r;
}

void HighPassFilter::setSampleRate(float rate) {
    sampleRate = rate;
}

float HighPassFilter::process(float input) {
    float f = cutoff / sampleRate;
    float q = res;
    
    float s1 = z1;
    float s2 = z2;
    
    float hp = input - s1 - q * s1;
    s1 += hp * f;
    z1 = s1;
    
    return hp;
}

void HighPassFilter::reset() {
    z1 = 0.0f;
    z2 = 0.0f;
}