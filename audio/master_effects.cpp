#include "master_effects.h"
#include "machine/Ncursesynth/effects/delay.h"
#include "machine/Ncursesynth/effects/reverb.h"
#include "machine/Ncursesynth/effects/chorus.h"
#include "machine/Ncursesynth/effects/distortion.h"

MasterEffects::MasterEffects() : sampleRate(48000.0f) {
    addEffect(EffectType::DELAY);
    addEffect(EffectType::REVERB);
    addEffect(EffectType::CHORUS);
    addEffect(EffectType::DISTORTION);
}

MasterEffects::~MasterEffects() {}

void MasterEffects::setSampleRate(float rate) {
    sampleRate = rate;
    for (auto& effect : chain) {
        effect->setSampleRate(rate);
    }
}

void MasterEffects::reset() {
    for (auto& effect : chain) {
        effect->reset();
    }
}

float MasterEffects::process(float input) {
    float output = input;
    for (auto& effect : chain) {
        output = effect->process(output);
    }
    return output;
}

const char* MasterEffects::effectTypeName(EffectType type) {
    switch (type) {
        case EffectType::DELAY:      return "DELAY";
        case EffectType::REVERB:     return "REVERB";
        case EffectType::CHORUS:     return "CHORUS";
        case EffectType::DISTORTION: return "DISTORTION";
    }
    return "UNKNOWN";
}

EffectBase* MasterEffects::getEffect(int index) {
    if (index < 0 || index >= (int)chain.size()) return nullptr;
    return chain[index].get();
}

const EffectBase* MasterEffects::getEffect(int index) const {
    if (index < 0 || index >= (int)chain.size()) return nullptr;
    return chain[index].get();
}

bool MasterEffects::addEffect(EffectType type) {
    EffectBase* effect = nullptr;
    switch (type) {
        case EffectType::DELAY:      effect = new DelayEffect(); break;
        case EffectType::REVERB:     effect = new ReverbEffect(); break;
        case EffectType::CHORUS:     effect = new ChorusEffect(); break;
        case EffectType::DISTORTION: effect = new DistortionEffect(); break;
    }
    if (!effect) return false;
    effect->setSampleRate(sampleRate);
    chain.push_back(std::unique_ptr<EffectBase>(effect));
    return true;
}

bool MasterEffects::insertEffect(int afterIndex, EffectType type) {
    if (afterIndex < -1 || afterIndex >= (int)chain.size()) return false;
    EffectBase* effect = nullptr;
    switch (type) {
        case EffectType::DELAY:      effect = new DelayEffect(); break;
        case EffectType::REVERB:     effect = new ReverbEffect(); break;
        case EffectType::CHORUS:     effect = new ChorusEffect(); break;
        case EffectType::DISTORTION: effect = new DistortionEffect(); break;
    }
    if (!effect) return false;
    effect->setSampleRate(sampleRate);
    chain.insert(chain.begin() + afterIndex + 1, std::unique_ptr<EffectBase>(effect));
    return true;
}

bool MasterEffects::removeEffect(int index) {
    if (index < 0 || index >= (int)chain.size()) return false;
    chain.erase(chain.begin() + index);
    return true;
}

bool MasterEffects::moveEffect(int from, int to) {
    if (from < 0 || from >= (int)chain.size()) return false;
    if (to < 0 || to >= (int)chain.size()) return false;
    if (from == to) return true;
    auto item = std::move(chain[from]);
    chain.erase(chain.begin() + from);
    chain.insert(chain.begin() + to, std::move(item));
    return true;
}

bool MasterEffects::setEffectEnabled(int index, bool enabled) {
    EffectBase* effect = getEffect(index);
    if (!effect) return false;
    effect->setEnabled(enabled);
    return true;
}

bool MasterEffects::isEffectEnabled(int index) const {
    const EffectBase* effect = getEffect(index);
    return effect ? effect->isEnabled() : false;
}

const char* MasterEffects::getEffectName(int index) const {
    const EffectBase* effect = getEffect(index);
    return effect ? effect->getName() : "NONE";
}

int MasterEffects::getEffectParamCount(int index) const {
    const EffectBase* effect = getEffect(index);
    return effect ? effect->getParamCount() : 0;
}

const char* MasterEffects::getEffectParamName(int index, int param) const {
    const EffectBase* effect = getEffect(index);
    return effect ? effect->getParamName(param) : "?";
}

float MasterEffects::getEffectParam(int index, int param) const {
    const EffectBase* effect = getEffect(index);
    return effect ? effect->getParam(param) : 0.0f;
}

void MasterEffects::setEffectParam(int index, int param, float normalized) {
    EffectBase* effect = getEffect(index);
    if (effect) effect->setParam(param, normalized);
}
