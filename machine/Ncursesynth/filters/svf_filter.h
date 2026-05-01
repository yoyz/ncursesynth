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
    float fc, res;
    float ic1eq, ic2eq;
    float in_0_, in_1_, in_2_;
    float out_0_, out_1_;
    float past_in_1_, past_in_2_, past_out_1_, past_out_2_;
    float current_cutoff_, current_resonance_;
    float PI = 3.14159265358979323846f;

    Type currentType;

public:
    StateVariableFilter(float rate = 48000.0f);
    ~StateVariableFilter() override;

    void setCutoff(float freq) override;
    void setResonance(float res) override;
    void setSampleRate(float rate) override;
    void setType(Type type);
    void computeCoefficients(Type type, float cutoff, float resonance);

    float process(float input) override;
    void reset() override;

    float processLP(float input);
    float processHP(float input);
    float processBP(float input);
    float processAP(float input);
};

#endif