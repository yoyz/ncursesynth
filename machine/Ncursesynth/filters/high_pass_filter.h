#ifndef HIGH_PASS_FILTER_H
#define HIGH_PASS_FILTER_H

#include "filter_base.h"

class HighPassFilter : public FilterBase {
private:
    float sampleRate;
    float cutoff;
    float res;
    float z1, z2;
    
public:
    HighPassFilter(float rate = 48000.0f);
    virtual ~HighPassFilter() {}
    
    void setCutoff(float freq) override;
    void setResonance(float res) override;
    void setSampleRate(float rate) override;
    float process(float input) override;
    void reset() override;
};

#endif