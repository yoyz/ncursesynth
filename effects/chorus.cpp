#include "chorus.h"
#include <algorithm>

ChorusEffect::ChorusEffect() 
    : writePosition(0), depth(0.5f), rate(0.5f), mix(0.3f), 
      enabled(true), sampleRate(48000.0f), phase(0.0f), maxDelaySamples(0) {
    setSampleRate(48000.0f);
}

ChorusEffect::~ChorusEffect() {}

void ChorusEffect::setSampleRate(float rate) {
    sampleRate = rate;
    maxDelaySamples = static_cast<int>(sampleRate * 0.05f); // 50ms max delay
    buffer.resize(maxDelaySamples * 2, 0.0f);
    reset();
}

void ChorusEffect::setDepth(float d) {
    depth = std::max(0.0f, std::min(1.0f, d));
}

void ChorusEffect::setRate(float r) {
    rate = std::max(0.1f, std::min(5.0f, r));
}

void ChorusEffect::setMix(float m) {
    mix = std::max(0.0f, std::min(1.0f, m));
}

float ChorusEffect::process(float input) {
    if (!enabled) return input;
    
    // LFO for modulation
    phase += rate / sampleRate;
    if (phase >= 1.0f) phase -= 1.0f;
    
    float lfoValue = sinf(phase * 2.0f * M_PI);
    int delaySamples = static_cast<int>((depth * 0.03f * sampleRate) * (0.5f + lfoValue * 0.5f));
    delaySamples = std::max(1, std::min(maxDelaySamples - 1, delaySamples));
    
    // Write to buffer
    buffer[writePosition] = input;
    
    // Read delayed sample
    int readPosition = (writePosition - delaySamples + maxDelaySamples * 2) % (maxDelaySamples * 2);
    float delayed = buffer[readPosition];
    
    // Update position
    writePosition = (writePosition + 1) % (maxDelaySamples * 2);
    
    // Mix
    return input * (1.0f - mix) + delayed * mix;
}

void ChorusEffect::reset() {
    std::fill(buffer.begin(), buffer.end(), 0.0f);
    writePosition = 0;
    phase = 0.0f;
}
