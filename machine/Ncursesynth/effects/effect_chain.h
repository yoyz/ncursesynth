#ifndef EFFECT_CHAIN_H
#define EFFECT_CHAIN_H

#include "effect_base.h"
#include "delay.h"
#include "reverb.h"
#include "chorus.h"
#include "distortion.h"
#include <vector>
#include <memory>

class EffectChain {
private:
    std::vector<std::unique_ptr<EffectBase>> effects;
    float sampleRate;
    
public:
    EffectChain();
    ~EffectChain();
    
    void setSampleRate(float rate);
    void addEffect(EffectBase* effect);
    float process(float input);
    void reset();
    
    // Individual effect controls
    DelayEffect* getDelay() { return static_cast<DelayEffect*>(effects[0].get()); }
    ReverbEffect* getReverb() { return static_cast<ReverbEffect*>(effects[1].get()); }
    ChorusEffect* getChorus() { return static_cast<ChorusEffect*>(effects[2].get()); }
    DistortionEffect* getDistortion() { return static_cast<DistortionEffect*>(effects[3].get()); }
    
    int getEffectCount() const { return effects.size(); }
};

#endif
