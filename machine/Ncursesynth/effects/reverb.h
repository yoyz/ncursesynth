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

    int getParamCount() const override { return 2; }
    const char* getParamName(int index) const override {
        return index == 0 ? "DECAY" : "MIX";
    }
    float getParam(int index) const override {
        return index == 0 ? (decay / 0.98f) : mix;
    }
    void setParam(int index, float normalized) override {
        float v = normalized < 0.0f ? 0.0f : (normalized > 1.0f ? 1.0f : normalized);
        if (index == 0) setDecay(v * 0.98f);
        else setMix(v);
    }
};

#endif
