/*
** Formant (vowel) filter
** Based on Odin 2 Synthesizer Plugin (GPLv3)
** Copyright (C) 2020 - 2021 TheWaveWarden
*/

#include "formant_filter.h"
#include <algorithm>

FormantFilter::FormantFilter(float rate)
    : sampleRate(rate), morph(0.0f) {
    vowelData[0][0] = 1000.f; vowelData[0][1] = 1400.f; // A
    vowelData[1][0] = 500.f;  vowelData[1][1] = 2300.f; // E
    vowelData[2][0] = 320.f;  vowelData[2][1] = 3200.f; // I
    vowelData[3][0] = 500.f;  vowelData[3][1] = 1000.f; // O
    vowelData[4][0] = 320.f;  vowelData[4][1] = 800.f;  // U
    vowelData[5][0] = 700.f;  vowelData[5][1] = 1800.f; // AE
    vowelData[6][0] = 500.f;  vowelData[6][1] = 1500.f; // OE
    vowelData[7][0] = 320.f;  vowelData[7][1] = 1650.f; // UE
    res1.setSampleRate(rate);
    res2.setSampleRate(rate);
    setCutoff(1000.0f);
}

FormantFilter::~FormantFilter() {}

void FormantFilter::setSampleRate(float rate) {
    sampleRate = rate;
    res1.setSampleRate(rate);
    res2.setSampleRate(rate);
}

void FormantFilter::setCutoff(float freq) {
    float t = freq / 8000.0f;
    if (t < 0) t = 0;
    if (t > 1) t = 1;
    morph = t * 7.0f;
    int idx = (int)morph;
    float frac = morph - idx;
    if (idx > 6) { idx = 6; frac = 1; }

    float f0 = vowelData[idx][0] * (1 - frac) + vowelData[idx + 1][0] * frac;
    float f1 = vowelData[idx][1] * (1 - frac) + vowelData[idx + 1][1] * frac;
    res1.setFrequency(f0);
    res2.setFrequency(f1);
}

void FormantFilter::setResonance(float res) {
    float q = std::max(0.01f, std::min(1.0f, res));
    float r = 0.95f + q * 0.04f;
    res1.radius = r;
    res2.radius = r;
    res1.calc();
    res2.calc();
}

void FormantFilter::reset() {
    res1.reset();
    res2.reset();
}

float FormantFilter::process(float input) {
    return res1.process(res2.process(input)) * 0.25f;
}
