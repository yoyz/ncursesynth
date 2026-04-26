#include "adsr_envelope.h"
#include <cmath>
#include <algorithm>

ADSREnvelope::ADSREnvelope(float rate) 
    : sampleRate(rate), 
      attack(0.1f), decay(0.1f), sustain(0.7f), release(0.1f),
      currentState(IDLE), currentLevel(0.0f) {
    updateAttackStep();
    updateDecayStep();
    updateReleaseStep();
}

ADSREnvelope::~ADSREnvelope() {}

void ADSREnvelope::setSampleRate(float rate) {
    sampleRate = rate;
    updateAttackStep();
    updateDecayStep();
    updateReleaseStep();
}

void ADSREnvelope::updateAttackStep() {
    float attackTime = 0.001f + attack * 1.999f;
    attackStep = 1.0f / (attackTime * sampleRate);
}

void ADSREnvelope::updateDecayStep() {
    float decayTime = 0.001f + decay * 1.999f;
    decayStep = (1.0f - sustain) / (decayTime * sampleRate);
}

void ADSREnvelope::updateReleaseStep() {
    float releaseTime = 0.001f + release * 1.999f;
    releaseStep = 1.0f / (releaseTime * sampleRate);
}

void ADSREnvelope::setAttack(float value) {
    attack = std::max(0.0f, std::min(1.0f, value));
    updateAttackStep();
}

void ADSREnvelope::setDecay(float value) {
    decay = std::max(0.0f, std::min(1.0f, value));
    updateDecayStep();
}

void ADSREnvelope::setSustain(float value) {
    sustain = std::max(0.0f, std::min(1.0f, value));
    updateDecayStep();
}

void ADSREnvelope::setRelease(float value) {
    release = std::max(0.0f, std::min(1.0f, value));
    updateReleaseStep();
}

void ADSREnvelope::noteOn() {
    currentState = ATTACK;
    currentLevel = 0.0f;
}

void ADSREnvelope::noteOff() {
    if (currentState != IDLE) {
        currentState = RELEASE;
    }
}

void ADSREnvelope::reset() {
    currentState = IDLE;
    currentLevel = 0.0f;
}

float ADSREnvelope::process() {
    switch (currentState) {
        case ATTACK:
            currentLevel += attackStep;
            if (currentLevel >= 1.0f) {
                currentLevel = 1.0f;
                currentState = DECAY;
            }
            break;
            
        case DECAY:
            currentLevel -= decayStep;
            if (currentLevel <= sustain) {
                currentLevel = sustain;
                currentState = SUSTAIN;
            }
            break;
            
        case SUSTAIN:
            // Level stays at sustain
            currentLevel = sustain;
            break;
            
        case RELEASE:
            currentLevel -= releaseStep;
            if (currentLevel <= 0.0f) {
                currentLevel = 0.0f;
                currentState = IDLE;
            }
            break;
            
        case IDLE:
            currentLevel = 0.0f;
            break;
    }
    
    return currentLevel;
}
