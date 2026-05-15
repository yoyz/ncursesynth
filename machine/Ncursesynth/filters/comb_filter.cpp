/*
** Comb filter with feedback delay
** Based on Odin 2 Synthesizer Plugin (GPLv3)
** Copyright (C) 2020 - 2021 TheWaveWarden
*/

#include "comb_filter.h"
#include <algorithm>
#include <cstring>

CombFilter::CombFilter(float rate)
    : sampleRate(rate), delayTime(0.01f), feedback(0.5f),
      writePos(0), delaySamples((int)(rate * 0.01f)) {
    buffer.resize(rate * 0.5f, 0);
}

CombFilter::~CombFilter() {}

void CombFilter::setSampleRate(float rate) {
    sampleRate = rate;
    delaySamples = (int)(rate * delayTime);
    if (delaySamples < 1) delaySamples = 1;
    buffer.resize(rate * 0.5f, 0);
}

void CombFilter::setCutoff(float freq) {
    delayTime = std::max(0.001f, std::min(0.5f, 1.0f / (freq + 20.0f)));
    delaySamples = (int)(sampleRate * delayTime);
    if (delaySamples < 1) delaySamples = 1;
}

void CombFilter::setResonance(float res) {
    feedback = std::max(0.0f, std::min(0.95f, res));
}

void CombFilter::reset() {
    std::fill(buffer.begin(), buffer.end(), 0);
    writePos = 0;
}

float CombFilter::process(float input) {
    int readPos = writePos - delaySamples;
    if (readPos < 0) readPos += buffer.size();
    float delayed = buffer[readPos];
    float out = input + delayed * feedback;
    buffer[writePos] = out;
    writePos = (writePos + 1) % buffer.size();
    return out * 0.5f;
}
