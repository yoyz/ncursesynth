#include "audio_engine.h"
#include "capture_analysis.h"
#include "audio_level.h"
#include <iostream>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <thread>

AudioEngine::AudioEngine(int rate, int frames, double latencyMs, float limiterThreshold)
    : stream(nullptr), synth(nullptr), machine(nullptr), isRunning(false), switching(false),
      sampleRate(rate), framesPerBuffer(frames), latencyMs(latencyMs),
      limiter(limiterThreshold, 1.0f, 50.0f, rate) {
    machine.store(nullptr);
    switching.store(false);
    synth = new SynthArchitecture(8, sampleRate);  // Start with 8 voices
    masterEffects.setSampleRate((float)sampleRate);
}

AudioEngine::~AudioEngine() {
    delete synth;
}

int AudioEngine::audioCallback(const void* inputBuffer, void* outputBuffer,
                              unsigned long framesPerBuffer,
                              const PaStreamCallbackTimeInfo* timeInfo,
                              PaStreamCallbackFlags statusFlags,
                              void* userData) {

    AudioEngine* engine = static_cast<AudioEngine*>(userData);
    float* out = static_cast<float*>(outputBuffer);

    // Skip if machine is being switched
    if (engine->switching.load(std::memory_order_acquire)) {
        for (unsigned int i = 0; i < framesPerBuffer; i++) {
            out[i] = 0.0f;
        }
        return paContinue;
    }

    Machine* machine = engine->machine.load(std::memory_order_acquire);
    if (machine) {
        // try_lock: non-blocking, skip this buffer if MIDI/UI holds the lock
        bool locked = machine->tryLock();
        for (unsigned int i = 0; i < framesPerBuffer; i++) {
            int32_t sample = 0;
            if (locked) {
                try {
                    sample = machine->tick();
                } catch (...) {
                    sample = 0;
                }
            }
            // Scale to float, apply shared master FX, then limit
            float f = sample / 8192.0f;
            f = engine->masterEffects.process(f);
            f = engine->limiter.process(f);
            CaptureAnalyzer::writeSample(f);
            AudioLevel::update(f);
            out[i] = f;
        }
        if (locked) machine->unlock();
    } else if (engine->synth) {
        for (unsigned int i = 0; i < framesPerBuffer; i++) {
            float sample = engine->synth->process();
            sample = engine->masterEffects.process(sample);
            out[i] = std::max(-0.95f, std::min(0.95f, sample));
        }
    } else {
        for (unsigned int i = 0; i < framesPerBuffer; i++) {
            out[i] = 0.0f;
        }
    }

    return paContinue;
}

bool AudioEngine::initialize() {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }
    
    PaStreamParameters outParams;
    outParams.device = Pa_GetDefaultOutputDevice();
    if (outParams.device == paNoDevice) {
        std::cerr << "No default output device found." << std::endl;
        Pa_Terminate();
        return false;
    }
    outParams.channelCount = 1;
    outParams.sampleFormat = paFloat32;
    outParams.suggestedLatency = latencyMs / 1000.0;
    outParams.hostApiSpecificStreamInfo = nullptr;
    
    err = Pa_OpenStream(&stream,
                        nullptr,       // No input
                        &outParams,
                        sampleRate,
                        framesPerBuffer,
                        paClipOff,
                        audioCallback,
                        this);
    
    if (err != paNoError) {
        std::cerr << "Failed to open audio stream: " << Pa_GetErrorText(err) << std::endl;
        Pa_Terminate();
        return false;
    }
    
    return true;
}

bool AudioEngine::start() {
    PaError err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "Failed to start audio stream: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }
    isRunning = true;
    return true;
}

void AudioEngine::stop() {
    if (stream) {
        Pa_StopStream(stream);
        isRunning = false;
    }
}

void AudioEngine::setMachine(Machine* m) {
    // Signal that we're switching — audio thread will output silence
    switching.store(true, std::memory_order_release);
    
    // Lock old machine so audio callback sees the lock and skips
    Machine* oldMachine = machine.load(std::memory_order_acquire);
    if (oldMachine && oldMachine != m) {
        oldMachine->lock();
        oldMachine->reset();
        oldMachine->unlock();
    }
    machine.store(m, std::memory_order_release);
    if (m) {
        m->lock();
        m->init();
        m->unlock();
    }
    
    // Clear switching flag
    switching.store(false, std::memory_order_release);
}

void AudioEngine::shutdown() {
    stop();
    if (stream) {
        Pa_CloseStream(stream);
        stream = nullptr;
    }
    Pa_Terminate();
}
