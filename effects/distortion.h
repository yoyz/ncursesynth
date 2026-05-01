#ifndef DISTORTION_H
#define DISTORTION_H

#include "effect_base.h"
#include <cmath>

class DistortionEffect : public EffectBase {
private:
    float drive;
    float mix;
    bool enabled;
    
public:
    DistortionEffect();
    ~DistortionEffect() override;
    
    float getDrive() const { return drive; }
    float getMix() const { return mix; }
    void setSampleRate(float rate) override;
    void setDrive(float d);   // 0-1
    void setMix(float m);     // 0-1
    float process(float input) override;
    void reset() override;
    bool isEnabled() const override { return enabled; }
    void setEnabled(bool e) override { enabled = e; }
    const char* getName() const override { return "DISTORTION"; }
};

#endif
