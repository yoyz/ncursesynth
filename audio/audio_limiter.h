#ifndef AUDIO_LIMITER_H
#define AUDIO_LIMITER_H

#include <cmath>
#include <algorithm>

class AudioLimiter {
public:
    AudioLimiter(float threshold = 0.85f, float attackMs = 1.0f, float releaseMs = 50.0f, int sampleRate = 48000)
        : threshold(threshold), envelope(0.0f), sampleRate(sampleRate),
          attackMs(attackMs), releaseMs(releaseMs) {
        updateCoeffs();
    }

    void reset() {
        envelope = 0.0f;
    }

    float process(float sample) {
        float absInput = fabsf(sample);

        if (absInput > envelope)
            envelope = attackCoeff * envelope + (1.0f - attackCoeff) * absInput;
        else
            envelope = releaseCoeff * envelope + (1.0f - releaseCoeff) * absInput;

        if (envelope > threshold) {
            float gainReduction = threshold / envelope;
            return sample * gainReduction;
        }
        return sample;
    }

    void setThreshold(float t) {
        threshold = t;
    }

    float getThreshold() const { return threshold; }

    void setSampleRate(int rate) {
        sampleRate = rate;
        updateCoeffs();
    }

    void setAttackMs(float ms) {
        attackMs = ms;
        updateCoeffs();
    }

    void setReleaseMs(float ms) {
        releaseMs = ms;
        updateCoeffs();
    }

    float getEnvelope() const { return envelope; }

private:
    float threshold;
    float attackCoeff;
    float releaseCoeff;
    float envelope;
    int sampleRate;
    float attackMs;
    float releaseMs;

    void updateCoeffs() {
        if (sampleRate <= 0) return;
        float at = std::max(attackMs / 1000.0f, 0.00001f);
        float rt = std::max(releaseMs / 1000.0f, 0.00001f);
        attackCoeff = expf(-1.0f / (static_cast<float>(sampleRate) * at));
        releaseCoeff = expf(-1.0f / (static_cast<float>(sampleRate) * rt));
    }
};

#endif
