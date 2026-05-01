#ifndef OBERHEIM_SEM_FILTER_H
#define OBERHEIM_SEM_FILTER_H

#include "filter_base.h"

class OberheimSEMFilter : public FilterBase {
private:
    float sampleRate;
    float fc, res;
    float lp, bp, hp;
    float f, q, drive;
    
    void updateCoefficients();
    
public:
    OberheimSEMFilter(float rate = 48000.0f);
    ~OberheimSEMFilter() override;
    
    void setCutoff(float cutoff) override;
    void setResonance(float resonance) override;
    void setSampleRate(float rate) override;
    void setDrive(float driveAmount);
    float process(float input) override;
    void reset() override;
};

#endif
