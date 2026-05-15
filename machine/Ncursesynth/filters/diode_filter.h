#ifndef DIODE_FILTER_H
#define DIODE_FILTER_H

/*
** Diode ladder filter
** Based on Odin 2 Synthesizer Plugin (GPLv3)
** Copyright (C) 2020 - 2021 TheWaveWarden
** Algorithm from Will Pirkle's "Designing Software Synthesizer Plug-Ins in C++"
*/

#include "filter_base.h"
#include <cmath>

class DiodeFilter : public FilterBase {
public:
    DiodeFilter(float rate = 48000.0f);
    ~DiodeFilter() override;

    void setCutoff(float freq) override;
    void setResonance(float res) override;
    void setSampleRate(float rate) override;
    float process(float input) override;
    void reset() override;

private:
    struct OnePole {
        double z1 = 0, alpha = 1.0, beta = 0, gamma = 1.0;
        double delta = 0, epsilon = 0, a0 = 1.0, feedback = 0;
        double processLP(double xn) {
            double vn = (a0 * xn + feedback * delta - z1) * alpha;
            double lpf = vn + z1;
            z1 = vn + lpf;
            return lpf;
        }
        double fbOut() { return beta * (z1 + feedback * delta); }
        void reset() { z1 = 0; feedback = 0; }
    };

    float sampleRate;
    double fc, res_k;
    double gamma, sg1, sg2, sg3, sg4;
    OnePole lpf1, lpf2, lpf3, lpf4;
    double last_freq;
    bool dirty;
};

#endif
