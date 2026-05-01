#ifndef DELAY_H
#define DELAY_H

#include "effect_base.h"
#include <vector>

class DelayEffect : public EffectBase {
private:
    std::vector<float> buffer;
    int writePosition;
    int readPosition;
    float delayTimeMs;
    float feedback;
    float mix;
    bool enabled;
    float sampleRate;
    int delaySamples;
    
public:
    DelayEffect();
    ~DelayEffect() override;
    float getDelayTime() const { return delayTimeMs; }
    float getFeedback() const { return feedback; }
    float getMix() const { return mix; }    
    void setSampleRate(float rate) override;
    void setDelayTime(float ms);  // 0-1000ms
    void setFeedback(float fb);    // 0-0.95
    void setMix(float m);          // 0-1
    float process(float input) override;
    void reset() override;
    bool isEnabled() const override { return enabled; }
    void setEnabled(bool e) override { enabled = e; }
    const char* getName() const override { return "DELAY"; }
};

#endif
