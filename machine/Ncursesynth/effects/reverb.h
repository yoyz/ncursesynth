#ifndef REVERB_H
#define REVERB_H

#include "effect_base.h"
#include <vector>

class ReverbEffect : public EffectBase {
private:
    std::vector<float> buffer;
    int writePosition;
    float decay;
    float mix;
    bool enabled;
    float sampleRate;
    int reverbSize;
    
public:
    ReverbEffect();
    ~ReverbEffect() override;
    
    float getDecay() const { return decay; }
    float getMix() const { return mix; }
    void setSampleRate(float rate) override;
    void setDecay(float d);  // 0-1
    void setMix(float m);     // 0-1
    float process(float input) override;
    void reset() override;
    bool isEnabled() const override { return enabled; }
    void setEnabled(bool e) override { enabled = e; }
    const char* getName() const override { return "REVERB"; }
};

#endif
