#include "korgms20_filter.h"
#include <cmath>

KorgMS20Filter::KorgMS20Filter(float rate) 
    : sampleRate(rate), cutoff(1000.0f), peak(0.5f),
      highpassFreq(30.0f), stage1(0), stage2(0), 
      stage3(0), highpassState(0) {
    updateCoefficients();
}

KorgMS20Filter::~KorgMS20Filter() {}

inline float KorgMS20Filter::ms20Saturate(float x) {
    if (x > 0.0f) return 1.0f - expf(-x * 1.5f);
    else return -1.0f + expf(x * 2.0f);
}

inline float KorgMS20Filter::ms20Clip(float x) {
    const float hardClip = 0.95f;
    if (x > hardClip) return hardClip;
    if (x < -hardClip) return -hardClip;
    return x;
}

inline float KorgMS20Filter::resonantSaturate(float x, float resAmount) {
    float drive = 1.0f + resAmount * 2.0f;
    float saturated = tanhf(x * drive);
    if (resAmount > 0.7f) saturated += 0.2f * resAmount * saturated * saturated;
    return ms20Clip(saturated);
}

void KorgMS20Filter::updateCoefficients() {
    float fc = cutoff;
    if (fc < 10.0f) fc = 10.0f;
    if (fc > sampleRate * 0.4f) fc = sampleRate * 0.4f;
    
    float wd = 2.0f * M_PI * fc;
    float t = 1.0f / sampleRate;
    float wa = (2.0f / t) * tanf(wd * t / 2.0f);
    g = wa * t / 1.3f;
    
    float wd_hp = 2.0f * M_PI * highpassFreq;
    float wa_hp = (2.0f / t) * tanf(wd_hp * t / 2.0f);
    g_hp = wa_hp * t / 2.0f;
    
    resonance = 0.1f + peak * 0.89f;
    if (resonance > 0.99f) resonance = 0.99f;
}

void KorgMS20Filter::setCutoff(float freq) {
    cutoff = freq;
    updateCoefficients();
}

void KorgMS20Filter::setResonance(float resonanceAmount) {
    peak = resonanceAmount;
    updateCoefficients();
}

void KorgMS20Filter::setSampleRate(float rate) {
    sampleRate = rate;
    updateCoefficients();
}

void KorgMS20Filter::setHighpassFreq(float freq) {
    highpassFreq = freq;
    updateCoefficients();
}

float KorgMS20Filter::process(float input) {
    float highpassed = g_hp * (input - highpassState) + highpassState;
    highpassState = highpassed;
    float saturated = ms20Saturate(highpassed * 1.5f);
    float feedback = resonance * stage3;
    
    float stage1_in = saturated - feedback + (saturated * saturated * 0.1f);
    stage1 = g * (ms20Saturate(stage1_in) - stage1) + stage1;
    
    float stage2_in = stage1 - stage2;
    stage2 = g * (ms20Saturate(stage2_in * 1.2f) - stage2) + stage2;
    
    float stage3_in = stage2;
    stage3 = g * (resonantSaturate(stage3_in, peak) - stage3) + stage3;
    
    float output = stage3 * 0.95f + saturated * 0.05f;
    output = ms20Saturate(output * (1.0f + peak * 0.5f));
    return output;
}

void KorgMS20Filter::reset() {
    stage1 = stage2 = stage3 = highpassState = 0.0f;
}
