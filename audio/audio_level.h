#ifndef AUDIO_LEVEL_H
#define AUDIO_LEVEL_H

#include <atomic>
#include <cmath>

struct AudioLevel {
    static std::atomic<float> peak;
    static std::atomic<float> rms;

    static void update(float sample) {
        float absVal = fabsf(sample);
        float oldPeak = peak.load(std::memory_order_relaxed);
        if (absVal > oldPeak)
            peak.store(absVal, std::memory_order_relaxed);
        else
            peak.store(oldPeak * 0.9995f, std::memory_order_relaxed);
    }

    static float getPeak() { return peak.load(std::memory_order_relaxed); }
    static float getRMS() {
        float p = getPeak();
        return p * 0.707f;
    }
};

#endif
