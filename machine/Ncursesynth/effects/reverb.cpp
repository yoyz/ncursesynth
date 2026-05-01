#include "reverb.h"
#include <algorithm>
#include <cmath>

ReverbEffect::ReverbEffect() 
    : writePosition(0), decay(0.5f), mix(0.3f), enabled(true), 
      sampleRate(48000.0f), reverbSize(0) {
    setSampleRate(48000.0f);
}

ReverbEffect::~ReverbEffect() {}

void ReverbEffect::setSampleRate(float rate) {
    sampleRate = rate;
    reverbSize = static_cast<int>(sampleRate * 0.5f); // 500ms reverb
    buffer.resize(reverbSize, 0.0f);
    reset();
}

void ReverbEffect::setDecay(float d) {
    decay = std::max(0.0f, std::min(0.98f, d));
}

void ReverbEffect::setMix(float m) {
    mix = std::max(0.0f, std::min(1.0f, m));
}

float ReverbEffect::process(float input) {
    if (!enabled) return input;
    
    // Simple feedback delay network reverb
    float output = buffer[writePosition];
    buffer[writePosition] = input + (output * decay);
    writePosition = (writePosition + 1) % reverbSize;
    
    // Mix dry/wet
    return input * (1.0f - mix) + output * mix;
}

void ReverbEffect::reset() {
    std::fill(buffer.begin(), buffer.end(), 0.0f);
    writePosition = 0;
}
