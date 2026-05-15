#ifndef COMB_FILTER_H
#define COMB_FILTER_H

/*
** Comb filter with feedback delay
** Based on Odin 2 Synthesizer Plugin (GPLv3)
** Copyright (C) 2020 - 2021 TheWaveWarden
*/

#include "filter_base.h"
#include <cmath>
#include <vector>

class CombFilter : public FilterBase {
public:
    CombFilter(float rate = 48000.0f);
    ~CombFilter() override;

    void setCutoff(float freq) override;
    void setResonance(float res) override;
    void setSampleRate(float rate) override;
    float process(float input) override;
    void reset() override;

private:
    float sampleRate;
    float delayTime;    // in seconds
    float feedback;
    std::vector<float> buffer;
    int writePos;
    int delaySamples;
};

#endif
