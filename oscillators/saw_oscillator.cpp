#include "saw_oscillator.h"
#include <cmath>

SawtoothOscillator::SawtoothOscillator(float rate) 
    : sampleRate(rate), phase(0.0f), freq(440.0f), waveform(Waveform::SAWTOOTH) {}

SawtoothOscillator::~SawtoothOscillator() {}

void SawtoothOscillator::setFrequency(float frequency) {
    freq = frequency;
}

void SawtoothOscillator::setSampleRate(float rate) {
    sampleRate = rate;
}

void SawtoothOscillator::setWaveform(Waveform wav) {
    waveform = wav;
}

float SawtoothOscillator::generateSawtooth() {
    return (phase * 2.0f) - 1.0f;
}

float SawtoothOscillator::generateSquare() {
    return phase < 0.5f ? 1.0f : -1.0f;
}

float SawtoothOscillator::generateTriangle() {
    if (phase < 0.25f) return 4.0f * phase;
    if (phase < 0.75f) return 2.0f - (4.0f * phase);
    return (4.0f * phase) - 4.0f;
}

float SawtoothOscillator::process() {
    float output = 0.0f;
    
    switch (waveform) {
        case Waveform::SAWTOOTH:
            output = generateSawtooth();
            break;
        case Waveform::SQUARE:
            output = generateSquare();
            break;
        case Waveform::TRIANGLE:
            output = generateTriangle();
            break;
    }
    
    phase += freq / sampleRate;
    if (phase >= 1.0f) phase -= 1.0f;
    
    return output;
}

void SawtoothOscillator::reset() {
    phase = 0.0f;
}
