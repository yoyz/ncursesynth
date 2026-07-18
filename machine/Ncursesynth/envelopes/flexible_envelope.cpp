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
      releaseStartLevel(0.0f),
      lastOutput(0.0f),
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
    // Exponential mapping: 3ms at 0, 15s at 1.0
    // This gives fine control over short times while allowing long sweeps
    float attackTime = 0.003f * powf(5000.0f, attack);
    attackStep = 1.0f / (attackTime * sampleRate);
}

void FlexibleEnvelope::updateDecayStep() {
    float decayTime = 0.003f * powf(5000.0f, decay);
    decayStep = (1.0f - sustain) / (decayTime * sampleRate);
}

void FlexibleEnvelope::updateReleaseStep() {
    float releaseTime = 0.003f * powf(5000.0f, release);
    releaseStep = 1.0f / (releaseTime * sampleRate);
}

float FlexibleEnvelope::getCurveFactor(float t, EnvelopeCurve curve) {
    // t is normalized 0-1 progress through the stage
    // All curves must map [0,1] → [0,1] exactly to avoid discontinuities.
    switch (curve) {
        case EnvelopeCurve::LINEAR:
            return t;
            
        case EnvelopeCurve::EXPONENTIAL:
            // Exponential curve: starts slow, accelerates
            // Normalized so f(0)=0, f(1)=1
            return (1.0f - expf(-t * 5.0f)) / 0.993262053f;
            
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
    lastOutput = 0.0f;
    targetLevel = 1.0f;
}

void FlexibleEnvelope::noteOff() {
    if (currentStage != EnvelopeStage::IDLE) {
        releaseStartLevel = (lastOutput > 0.001f) ? lastOutput : 0.0f;
        currentStage = EnvelopeStage::RELEASE;
        targetLevel = 0.0f;
    }
}

void FlexibleEnvelope::reset() {
    currentStage = EnvelopeStage::IDLE;
    currentLevel = 0.0f;
    releaseStartLevel = 0.0f;
    lastOutput = 0.0f;
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
            lastOutput = shaped;
            return shaped;
        }
        
        case EnvelopeStage::DECAY: {
            currentLevel -= decayStep;
            
            if (currentLevel <= sustain) {
                currentLevel = sustain;
                currentStage = EnvelopeStage::SUSTAIN;
            }
            
            // Map currentLevel [1.0..sustain] to progress [0..1], output goes from 1.0 down to sustain
            if (sustain >= 1.0f) {
                lastOutput = 1.0f;
                return 1.0f;
            }
            float progress = (1.0f - currentLevel) / (1.0f - sustain);
            float shaped = applyCurve(progress, decayCurve);
            lastOutput = 1.0f - (1.0f - sustain) * shaped;
            return lastOutput;
        }
        
        case EnvelopeStage::SUSTAIN:
            lastOutput = sustain;
            return sustain;
            
        case EnvelopeStage::RELEASE: {
            if (releaseStartLevel <= 0.0f) {
                currentStage = EnvelopeStage::IDLE;
                currentLevel = 0.0f;
                lastOutput = 0.0f;
                return 0.0f;
            }
            
            currentLevel -= releaseStep;
            
            if (currentLevel <= 0.0f) {
                currentLevel = 0.0f;
                currentStage = EnvelopeStage::IDLE;
                lastOutput = 0.0f;
                return 0.0f;
            }
            
            float progress = 1.0f - (currentLevel / (releaseStartLevel + 0.0001f));
            if (progress < 0.0f) progress = 0.0f;
            if (progress > 1.0f) progress = 1.0f;
            
            float shaped = applyCurve(progress, releaseCurve);
            lastOutput = releaseStartLevel * (1.0f - shaped);
            return lastOutput;
        }
        
        case EnvelopeStage::IDLE:
        default:
            lastOutput = 0.0f;
            return 0.0f;
    }
}
