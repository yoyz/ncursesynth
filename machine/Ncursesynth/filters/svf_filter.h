#ifndef SVF_FILTER_H
#define SVF_FILTER_H

#include "filter_base.h"

class StateVariableFilter : public FilterBase {
public:
    enum class Type {
        LP12,
        HP12,
        BP12,
        AP12
    };

private:
    float sampleRate;
    float fc, resQ;
    float lp, bp, hp;
    float f, q;

    Type currentType;

public:
    StateVariableFilter(float rate = 48000.0f);
    ~StateVariableFilter() override;

    void setCutoff(float freq) override;
    void setResonance(float res) override;
    void setSampleRate(float rate) override;
    void setType(Type type);
    void computeCoefficients();

    float process(float input) override;
    void reset() override;
};

#endif
