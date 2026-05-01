#ifndef EFFECT_BASE_H
#define EFFECT_BASE_H

class EffectBase {
public:
    virtual ~EffectBase() {}
    virtual void setSampleRate(float rate) = 0;
    virtual float process(float input) = 0;
    virtual void reset() = 0;
    virtual bool isEnabled() const = 0;
    virtual void setEnabled(bool enabled) = 0;
    virtual const char* getName() const = 0;
};

#endif
