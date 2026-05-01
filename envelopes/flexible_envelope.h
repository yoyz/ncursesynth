#ifndef FLEXIBLE_ENVELOPE_H
#define FLEXIBLE_ENVELOPE_H

#include "envelope_types.h"

class FlexibleEnvelope {
private:
    float sampleRate;
    
    // Parameters
    float attack;      // 0-1 (0=1ms, 1=2s)
    float decay;       // 0-1
    float sustain;     // 0-1
    float release;     // 0-1
    
    EnvelopeCurve attackCurve;
    EnvelopeCurve decayCurve;
    EnvelopeCurve releaseCurve;
    
    // State
    EnvelopeStage currentStage;
    float currentLevel;
    
    // Calculated values
    float attackStep;
    float decayStep;
    float releaseStep;
    float targetLevel;
    
    // Curve-specific lookup tables or precomputed values
    void updateAttackStep();
    void updateDecayStep();
    void updateReleaseStep();
    
    // Curve processing functions
    float applyCurve(float t, EnvelopeCurve curve);
    float getCurveFactor(float t, EnvelopeCurve curve);
    
public:
    FlexibleEnvelope(float rate = 48000.0f);
    ~FlexibleEnvelope();
    
    void setSampleRate(float rate);
    
    // Parameter setters (0.0 to 1.0)
    void setAttack(float value);
    void setDecay(float value);
    void setSustain(float value);
    void setRelease(float value);
    
    // Curve setters
    void setAttackCurve(EnvelopeCurve curve);
    void setDecayCurve(EnvelopeCurve curve);
    void setReleaseCurve(EnvelopeCurve curve);
    
    // Event triggers
    void noteOn();
    void noteOff();
    void reset();
    
    // Process one sample
    float process();
    
    // Getters
    bool isActive() const { return currentStage != EnvelopeStage::IDLE; }
    float getCurrentLevel() const { return currentLevel; }
    
    // Parameter getters
    float getAttack() const { return attack; }
    float getDecay() const { return decay; }
    float getSustain() const { return sustain; }
    float getRelease() const { return release; }
    
    EnvelopeCurve getAttackCurve() const { return attackCurve; }
    EnvelopeCurve getDecayCurve() const { return decayCurve; }
    EnvelopeCurve getReleaseCurve() const { return releaseCurve; }
};

#endif
