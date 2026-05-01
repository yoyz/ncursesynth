#ifndef ADSR_ENVELOPE_H
#define ADSR_ENVELOPE_H

class ADSREnvelope {
private:
    float sampleRate;
    
    // Parameters (0-1 range)
    float attack;
    float decay;
    float sustain;
    float release;
    
    // State
    enum State { IDLE, ATTACK, DECAY, SUSTAIN, RELEASE };
    State currentState;
    
    float currentLevel;
    float attackStep;
    float decayStep;
    float releaseStep;
    
    void updateAttackStep();
    void updateDecayStep();
    void updateReleaseStep();
    
public:
    ADSREnvelope(float rate = 48000.0f);
    ~ADSREnvelope();
    
    void setSampleRate(float rate);
    
    // Parameter setters (0.0 to 1.0)
    void setAttack(float value);    // 0 = fastest, 1 = slowest
    void setDecay(float value);
    void setSustain(float value);
    void setRelease(float value);
    
    // Event triggers
    void noteOn();
    void noteOff();
    void reset();
    
    // Process one sample
    float process();
    
    // Get current state
    bool isActive() const { return currentState != IDLE; }
    float getCurrentLevel() const { return currentLevel; }
    
    // Parameter getters
    float getAttack() const { return attack; }
    float getDecay() const { return decay; }
    float getSustain() const { return sustain; }
    float getRelease() const { return release; }
};

#endif
