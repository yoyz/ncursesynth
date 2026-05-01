#include "effect_chain.h"

EffectChain::EffectChain() : sampleRate(48000.0f) {
    // Add effects in order
    effects.push_back(std::make_unique<DelayEffect>());
    effects.push_back(std::make_unique<ReverbEffect>());
    effects.push_back(std::make_unique<ChorusEffect>());
    effects.push_back(std::make_unique<DistortionEffect>());
    
    for (auto& effect : effects) {
        effect->setSampleRate(sampleRate);
    }
}

EffectChain::~EffectChain() {}

void EffectChain::setSampleRate(float rate) {
    sampleRate = rate;
    for (auto& effect : effects) {
        effect->setSampleRate(rate);
    }
}

float EffectChain::process(float input) {
    float output = input;
    for (auto& effect : effects) {
        output = effect->process(output);
    }
    return output;
}

void EffectChain::reset() {
    for (auto& effect : effects) {
        effect->reset();
    }
}
