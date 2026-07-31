#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

#include <atomic>
#include <portaudio.h>
#include "machine/Ncursesynth/synth/synth_architecture.h"
#include "machine/Machine.h"
#include "audio_limiter.h"
#include "master_effects.h"

class AudioEngine {
private:
    PaStream* stream;
    SynthArchitecture* synth;
    std::atomic<Machine*> machine;
    std::atomic<bool> isRunning;
    std::atomic<bool> switching;
    int sampleRate;
    int framesPerBuffer;
    double latencyMs;
    AudioLimiter limiter;
    MasterEffects masterEffects;

    static int audioCallback(const void* inputBuffer, void* outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void* userData);

public:
    AudioEngine(int rate = 48000, int frames = 256, double latencyMs = 20.0, float limiterThreshold = 0.85f);
    ~AudioEngine();

    bool initialize();
    bool start();
    void stop();
    void shutdown();
    SynthArchitecture* getSynth() { return synth; }
    Machine* getMachine() { return machine.load(); }
    void setMachine(Machine* m);
    bool isActive() const { return isRunning.load(); }
    AudioLimiter* getLimiter() { return &limiter; }
    MasterEffects* getMasterEffects() { return &masterEffects; }
};

#endif
