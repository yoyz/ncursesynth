#ifndef KORGMS20_FILTER_H
#define KORGMS20_FILTER_H

#include "filter_base.h"

class KorgMS20Filter : public FilterBase {
private:
    float sampleRate;
    float cutoff, peak, highpassFreq;
    float stage1, stage2, stage3, highpassState;
    float g, g_hp, resonance;
    
    inline float ms20Saturate(float x);
    inline float ms20Clip(float x);
    inline float resonantSaturate(float x, float resAmount);
    void updateCoefficients();
    
public:
    KorgMS20Filter(float rate = 48000.0f);
    ~KorgMS20Filter() override;
    
    void setCutoff(float freq) override;
    void setResonance(float resonanceAmount) override;
    void setSampleRate(float rate) override;
    void setHighpassFreq(float freq);
    float process(float input) override;
    void reset() override;
};

#endif
