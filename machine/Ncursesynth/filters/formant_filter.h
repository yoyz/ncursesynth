#ifndef FORMANT_FILTER_H
#define FORMANT_FILTER_H

/*
** Formant (vowel) filter
** Based on Odin 2 Synthesizer Plugin (GPLv3)
** Copyright (C) 2020 - 2021 TheWaveWarden
*/

#include "filter_base.h"
#include <cmath>

class FormantFilter : public FilterBase {
public:
    FormantFilter(float rate = 48000.0f);
    ~FormantFilter() override;

    void setCutoff(float freq) override;
    void setResonance(float res) override;
    void setSampleRate(float rate) override;
    float process(float input) override;
    void reset() override;

private:
    struct Resonator2Pole {
        float sr, freq, radius, z1, z2, a1, a2, b0, b1, b2;
        Resonator2Pole() : sr(48000), freq(1000), radius(0.9f), z1(0), z2(0),
            a1(0), a2(0), b0(0.03f), b1(0), b2(-0.03f) {}
        void setSampleRate(float r) { sr = r; calc(); }
        void setFrequency(float f) { freq = f; calc(); }
        void setQ(float q) { radius = 0.85f + q * 0.14f; calc(); }
        void calc() {
            float w0 = 2.0f * M_PI * freq / sr;
            a1 = -2.0f * radius * std::cos(w0);
            a2 = radius * radius;
        }
        float process(float x) {
            float y = b0 * x + b1 * z1 + b2 * z2 - a1 * z1 - a2 * z2;
            z2 = z1; z1 = y;
            return y * 0.5f;
        }
        void reset() { z1 = z2 = 0; }
    };

    float sampleRate;
    float morph;
    float vowelData[8][2];
    Resonator2Pole res1, res2;
};

#endif
