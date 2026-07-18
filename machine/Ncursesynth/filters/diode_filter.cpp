/*
** Diode ladder filter
** Based on Odin 2 Synthesizer Plugin (GPLv3)
** Copyright (C) 2020 - 2021 TheWaveWarden
** Algorithm from Will Pirkle's "Designing Software Synthesizer Plug-Ins in C++"
*/

#include "diode_filter.h"
#include <algorithm>

DiodeFilter::DiodeFilter(float rate)
    : sampleRate(rate), fc(1000.0), res_k(0), gamma(0),
      sg1(0), sg2(0), sg3(0), sg4(0), last_freq(-1), dirty(true) {
    reset();
}

DiodeFilter::~DiodeFilter() {}

void DiodeFilter::setSampleRate(float rate) {
    sampleRate = rate;
    last_freq = -1;
    dirty = true;
}

void DiodeFilter::setCutoff(float freq) {
    fc = freq;
    dirty = true;
}

void DiodeFilter::setResonance(float res) {
    res_k = 0.5 + 15.5 * res;
    dirty = true;
}

void DiodeFilter::reset() {
    lpf1.reset(); lpf2.reset(); lpf3.reset(); lpf4.reset();
    last_freq = -1;
    dirty = true;
}

float DiodeFilter::process(float input) {
    if (dirty) {
        double sr = sampleRate;
        double wd = 2 * M_PI * fc;
        double wa = (2.0 * sr) * std::tan(wd / sr * 0.5);
        double g = wa / sr / 2.0;

        double G4 = 0.5 * g / (1.0 + g);
        double G3 = 0.5 * g / (1.0 + g - 0.5 * g * G4);
        double G2 = 0.5 * g / (1.0 + g - 0.5 * g * G3);
        double G1 = g / (1.0 + g - g * G2);
        gamma = G4 * G3 * G2 * G1;

        sg1 = G4 * G3 * G2;
        sg2 = G4 * G3;
        sg3 = G4;
        sg4 = 1.0;

        double G = g / (1.0 + g);
        lpf1.alpha = G; lpf2.alpha = G; lpf3.alpha = G; lpf4.alpha = G;
        lpf1.beta = 1.0 / (1.0 + g - g * G2);
        lpf2.beta = 1.0 / (1.0 + g - 0.5 * g * G3);
        lpf3.beta = 1.0 / (1.0 + g - 0.5 * g * G4);
        lpf4.beta = 1.0 / (1.0 + g);

        lpf1.delta = g; lpf2.delta = 0.5 * g; lpf3.delta = 0.5 * g; lpf4.delta = 0.0;
        lpf1.gamma = 1.0 + G1 * G2; lpf2.gamma = 1.0 + G2 * G3;
        lpf3.gamma = 1.0 + G3 * G4; lpf4.gamma = 1.0;
        lpf1.epsilon = G2; lpf2.epsilon = G3; lpf3.epsilon = G4; lpf4.epsilon = 0.0;
        lpf1.a0 = 1.0; lpf2.a0 = 0.5; lpf3.a0 = 0.5; lpf4.a0 = 0.5;

        dirty = false;
    }

    double xn = input;
    double k = std::max(0.0, std::min(16.0, res_k));
    xn *= 1.0 + 0.3 * k;

    lpf4.feedback = 0.0;
    lpf3.feedback = lpf4.fbOut();
    lpf2.feedback = lpf3.fbOut();
    lpf1.feedback = lpf2.fbOut();

    double sigma = sg1 * lpf1.fbOut() + sg2 * lpf2.fbOut() + sg3 * lpf3.fbOut() + sg4 * lpf4.fbOut();
    double u = (xn - k * sigma) / (1.0 + k * gamma);

    double out = lpf4.processLP(lpf3.processLP(lpf2.processLP(lpf1.processLP(u))));

    float gain = 3.0f;
    if (fc < 150.0) gain = 8.0f;
    else if (fc < 300.0) gain = 6.0f;
    else if (fc < 600.0) gain = 4.0f;
    else if (fc > 6000.0) gain = 2.0f;

    return (float)(out * gain);
}
