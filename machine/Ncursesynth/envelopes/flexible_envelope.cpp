#include "flexible_envelope.h"
#include <cmath>
#include <algorithm>

FlexibleEnvelope::FlexibleEnvelope(float rate) 
    : sampleRate(rate),
      attack(0.1f), decay(0.1f), sustain(0.7f), release(0.1f),
      attackCurve(EnvelopeCurve::EXPONENTIAL),
      decayCurve(EnvelopeCurve::EXPONENTIAL),
      releaseCurve(EnvelopeCurve::EXPONENTIAL),
      currentStage(EnvelopeStage::IDLE),
      currentLevel(0.0f),
      targetLevel(0.0f) {
    updateAttackStep();
    updateDecayStep();
    updateReleaseStep();
}

FlexibleEnvelope::~FlexibleEnvelope() {}

void FlexibleEnvelope::setSampleRate(float rate) {
    sampleRate = rate;
    updateAttackStep();
    updateDecayStep();
    updateReleaseStep();
}

void FlexibleEnvelope::updateAttackStep() {
    float attackTime = 0.001f + attack * 1.999f; // 1ms to 2 seconds
    attackStep = 1.0f / (attackTime * sampleRate);
}

void FlexibleEnvelope::updateDecayStep() {
    float decayTime = 0.001f + decay * 1.999f;
    decayStep = (1.0f - sustain) / (decayTime * sampleRate);
}

void FlexibleEnvelope::updateReleaseStep() {
    float releaseTime = 0.001f + release * 1.999f;
    releaseStep = 1.0f / (releaseTime * sampleRate);
}

float FlexibleEnvelope::getCurveFactor(float t, EnvelopeCurve curve) {
    // t is normalized 0-1 progress through the stage
    switch (curve) {
        case EnvelopeCurve::LINEAR:
            return t;
            
        case EnvelopeCurve::EXPONENTIAL:
            // Exponential curve: starts slow, accelerates
            return 1.0f - expf(-t * 5.0f);
            
        case EnvelopeCurve::LOGARITHMIC:
            // Logarithmic curve: starts fast, slows down
            return logf(1.0f + t * 9.0f) / logf(10.0f);
            
        case EnvelopeCurve::SINE:
            // Sine curve: smooth S-shape
            return sinf(t * M_PI / 2.0f);
            
        case EnvelopeCurve::TENT:
            // Fast attack, exponential decay shape
            if (t < 0.1f) return t * 10.0f;  // Sharp initial rise
            return 1.0f - expf(-(t - 0.1f) * 3.0f);
            
        case EnvelopeCurve::REVERSE_TENT:
            // Slow attack, fast decay shape
            if (t < 0.3f) return t * t * 11.11f;  // Slow start
            return 0.7f + (t - 0.3f) * 0.428f;   // Then linear
            
        default:
            return t;
    }
}

float FlexibleEnvelope::applyCurve(float t, EnvelopeCurve curve) {
    // Apply curve shaping to the progress t (0-1)
    return getCurveFactor(t, curve);
}

void FlexibleEnvelope::setAttack(float value) {
    attack = std::max(0.0f, std::min(1.0f, value));
    updateAttackStep();
}

void FlexibleEnvelope::setDecay(float value) {
    decay = std::max(0.0f, std::min(1.0f, value));
    updateDecayStep();
}

void FlexibleEnvelope::setSustain(float value) {
    sustain = std::max(0.0f, std::min(1.0f, value));
    updateDecayStep();
}

void FlexibleEnvelope::setRelease(float value) {
    release = std::max(0.0f, std::min(1.0f, value));
    updateReleaseStep();
}

void FlexibleEnvelope::setAttackCurve(EnvelopeCurve curve) {
    attackCurve = curve;
}

void FlexibleEnvelope::setDecayCurve(EnvelopeCurve curve) {
    decayCurve = curve;
}

void FlexibleEnvelope::setReleaseCurve(EnvelopeCurve curve) {
    releaseCurve = curve;
}

void FlexibleEnvelope::noteOn() {
    currentStage = EnvelopeStage::ATTACK;
    currentLevel = 0.0f;
    targetLevel = 1.0f;
}

void FlexibleEnvelope::noteOff() {
    if (currentStage != EnvelopeStage::IDLE) {
        currentStage = EnvelopeStage::RELEASE;
        targetLevel = 0.0f;
    }
}

void FlexibleEnvelope::reset() {
    currentStage = EnvelopeStage::IDLE;
    currentLevel = 0.0f;
}

float FlexibleEnvelope::process() {
    switch (currentStage) {
        case EnvelopeStage::ATTACK: {
            currentLevel += attackStep;
            
            if (currentLevel >= 1.0f) {
                currentLevel = 1.0f;
                currentStage = EnvelopeStage::DECAY;
                targetLevel = sustain;
            }
            
            // Apply curve shaping
            float shaped = applyCurve(currentLevel, attackCurve);
            return shaped;
        }
        
        case EnvelopeStage::DECAY: {
            currentLevel -= decayStep;
            
            if (currentLevel <= sustain) {
                currentLevel = sustain;
                currentStage = EnvelopeStage::SUSTAIN;
            }
            
            // Apply curve shaping
            float t = (currentLevel - sustain) / (1.0f - sustain);
            if (sustain >= 1.0f) t = 0.0f;
            float shaped = applyCurve(1.0f - t, decayCurve);
            shaped = sustain + (1.0f - sustain) * shaped;
            return shaped;
        }
        
        case EnvelopeStage::SUSTAIN:
            return sustain;
            
        case EnvelopeStage::RELEASE: {
            currentLevel -= releaseStep;
            
            if (currentLevel <= 0.0f) {
                currentLevel = 0.0f;
                currentStage = EnvelopeStage::IDLE;
            }
            
            // Apply curve shaping
            float shaped = applyCurve(currentLevel, releaseCurve);
            return shaped;
        }
        
        case EnvelopeStage::IDLE:
        default:
            return 0.0f;
    }
}
