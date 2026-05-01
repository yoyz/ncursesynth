#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

#include <atomic>
#include <portaudio.h>
#include "synth/synth_architecture.h"
#include "machine/Machine.h"

class AudioEngine {
private:
    PaStream* stream;
    SynthArchitecture* synth;
    Machine* machine;
    std::atomic<bool> isRunning;
    int sampleRate;
    int framesPerBuffer;

    static int audioCallback(const void* inputBuffer, void* outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void* userData);

public:
    AudioEngine(int rate = 48000, int frames = 256);
    ~AudioEngine();

    bool initialize();
    bool start();
    void stop();
    void shutdown();
    SynthArchitecture* getSynth() { return synth; }
    Machine* getMachine() { return machine; }
    void setMachine(Machine* m);
    bool isActive() const { return isRunning.load(); }
};

#endif
