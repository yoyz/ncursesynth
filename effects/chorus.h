#ifndef CHORUS_H
#define CHORUS_H

#include "effect_base.h"
#include <vector>
#include <cmath>

class ChorusEffect : public EffectBase {
private:
    std::vector<float> buffer;
    int writePosition;
    float depth;
    float rate;
    float mix;
    bool enabled;
    float sampleRate;
    float phase;
    int maxDelaySamples;
    
public:
    ChorusEffect();
    ~ChorusEffect() override;
    
    // Add these public methods to ChorusEffect class
    float getDepth() const { return depth; }
    float getRate() const { return rate; }
    float getMix() const { return mix; }
    void setSampleRate(float rate) override;
    void setDepth(float d);   // 0-1
    void setRate(float r);    // 0.1-5 Hz
    void setMix(float m);     // 0-1
    float process(float input) override;
    void reset() override;
    bool isEnabled() const override { return enabled; }
    void setEnabled(bool e) override { enabled = e; }
    const char* getName() const override { return "CHORUS"; }
};

#endif
