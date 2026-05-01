#ifndef MOOG_FILTER_H
#define MOOG_FILTER_H

#include "filter_base.h"

class MoogLadderFilter : public FilterBase {
private:
    float sampleRate;
    float fc, res, drive;
    float stage[4];
    float g;
    
    inline float saturate(float x);
    inline float clip(float x);
    void updateCoefficients();
    
public:
    MoogLadderFilter(float rate = 48000.0f);
    ~MoogLadderFilter() override;
    
    void setCutoff(float freq) override;
    void setResonance(float res) override;
    void setSampleRate(float rate) override;
    void setDrive(float driveAmount);
    float process(float input) override;
    void reset() override;
};

#endif
