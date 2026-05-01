#include "delay.h"
#include <algorithm>

DelayEffect::DelayEffect() 
    : writePosition(0), readPosition(0), delayTimeMs(250.0f), 
      feedback(0.4f), mix(0.3f), enabled(true), sampleRate(48000.0f), delaySamples(0) {
    setSampleRate(48000.0f);
}

DelayEffect::~DelayEffect() {}

void DelayEffect::setSampleRate(float rate) {
    sampleRate = rate;
    delaySamples = static_cast<int>((delayTimeMs / 1000.0f) * sampleRate);
    buffer.resize(delaySamples + 1, 0.0f);
    reset();
}

void DelayEffect::setDelayTime(float ms) {
    delayTimeMs = std::max(1.0f, std::min(1000.0f, ms));
    delaySamples = static_cast<int>((delayTimeMs / 1000.0f) * sampleRate);
    buffer.resize(delaySamples + 1, 0.0f);
    reset();
}

void DelayEffect::setFeedback(float fb) {
    feedback = std::max(0.0f, std::min(0.95f, fb));
}

void DelayEffect::setMix(float m) {
    mix = std::max(0.0f, std::min(1.0f, m));
}

float DelayEffect::process(float input) {
    if (!enabled || delaySamples == 0) return input;
    
    // Read from buffer
    float delayed = buffer[writePosition];
    
    // Write to buffer with feedback
    buffer[writePosition] = input + (delayed * feedback);
    
    // Update position
    writePosition = (writePosition + 1) % (delaySamples + 1);
    
    // Mix dry/wet
    return input * (1.0f - mix) + delayed * mix;
}

void DelayEffect::reset() {
    std::fill(buffer.begin(), buffer.end(), 0.0f);
    writePosition = 0;
}
