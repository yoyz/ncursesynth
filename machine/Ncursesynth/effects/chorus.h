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

    int getParamCount() const override { return 3; }
    const char* getParamName(int index) const override {
        return index == 0 ? "DEPTH" : (index == 1 ? "RATE" : "MIX");
    }
    float getParam(int index) const override {
        return index == 0 ? depth
             : (index == 1 ? ((rate - 0.1f) / 4.9f) : mix);
    }
    void setParam(int index, float normalized) override {
        float v = normalized < 0.0f ? 0.0f : (normalized > 1.0f ? 1.0f : normalized);
        if (index == 0) setDepth(v);
        else if (index == 1) setRate(0.1f + v * 4.9f);
        else setMix(v);
    }
};

#endif
