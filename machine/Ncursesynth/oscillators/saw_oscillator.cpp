#include "saw_oscillator.h"
#include <cmath>
#include <algorithm>

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

static float polyBlep(float t) {
    // 2-point 2nd-order polyBLEP residual for a unit step
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    float dt = 1.0f - t;
    return dt * dt;
}

float SawtoothOscillator::process() {
    float inc = freq / sampleRate;
    if (inc <= 0.0f || inc >= 0.5f)
        inc = 0.0f; // Nyquist safety

    float output = 0.0f;
    float raw = (phase * 2.0f) - 1.0f;

    if (waveform == Waveform::SAWTOOTH) {
        // Step of -2 at phase=0 (from +1 to -1)
        // Post-correction: we just passed the discontinuity
        float t = phase / inc;
        if (t < 1.0f)
            raw -= polyBlep(t); // step = -2, so -2 * 0.5 * (1-t)^2 = -(1-t)^2

        // Pre-correction: about to hit the discontinuity
        float nextPhase = phase + inc;
        if (nextPhase >= 1.0f) {
            float t2 = (nextPhase - 1.0f) / inc;
            raw += polyBlep(t2);
        }
        output = raw;
    } else if (waveform == Waveform::SQUARE) {
        raw = phase < 0.5f ? 1.0f : -1.0f;

        // Discontinuity at phase=0: step -2 (from +1 to -1)
        float t0 = phase / inc;
        if (t0 < 1.0f) raw -= polyBlep(t0);
        float nextPhase = phase + inc;
        if (nextPhase >= 1.0f) {
            float t2 = (nextPhase - 1.0f) / inc;
            raw += polyBlep(t2);
        }

        // Discontinuity at phase=0.5: step +2 (from -1 to +1)
        float dist = phase - 0.5f;
        float t1 = dist / inc + 0.5f;
        if (dist < 0.0f) {
            // We haven't passed it yet this cycle
            float t1_post = 1.0f + dist / inc;
            if (t1_post < 1.0f) raw += polyBlep(t1_post);
            // Pre-correction for next sample
            float nextPos = nextPhase;
            if (nextPos >= 0.5f && nextPos < 1.0f + inc) {
                float t2 = (nextPos - 0.5f) / inc;
                if (t2 < 1.0f) raw -= polyBlep(t2);
            }
        } else {
            // Already passed the 0.5 discontinuity
            if (t1 < 1.0f) raw += polyBlep(t1);
            // Check if next sample passes 0.5
            if (phase < 0.5f && nextPhase >= 0.5f) {
                float t2 = (nextPhase - 0.5f) / inc;
                if (t2 < 1.0f) raw -= polyBlep(t2);
            }
        }
        output = raw;
    } else {
        // Triangle — slope changes at phase 0.25 and 0.75
        if (phase < 0.25f) output = 4.0f * phase;
        else if (phase < 0.75f) output = 2.0f - (4.0f * phase);
        else output = (4.0f * phase) - 4.0f;

        // Smooth slope-change discontinuities with a 2-point correction
        float t = phase / inc;
        // Peak at phase 0.25 (slope +4 → -4)
        float dist = phase - 0.25f;
        if (fabsf(dist) < inc) {
            float d = dist / inc;
            output -= d * d * 2.0f;
        }
        // Valley at phase 0.75 (slope -4 → +4)
        dist = phase - 0.75f;
        if (fabsf(dist) < inc) {
            float d = dist / inc;
            output += d * d * 2.0f;
        }
    }

    phase += inc;
    if (phase >= 1.0f) phase -= 1.0f;

    return output;
}

void SawtoothOscillator::reset() {
    phase = 0.0f;
}

void SawtoothOscillator::addPhaseOffset(float offset) {
    phase += offset;
    if (phase >= 1.0f) phase -= 1.0f;
    if (phase < 0.0f) phase += 1.0f;
}
