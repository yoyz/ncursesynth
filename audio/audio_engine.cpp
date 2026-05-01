#include "audio_engine.h"
#include <iostream>
#include <cstring>
#include <algorithm>
#include <thread>

AudioEngine::AudioEngine(int rate, int frames)
    : stream(nullptr), synth(nullptr), machine(nullptr), isRunning(false), switching(false), sampleRate(rate), framesPerBuffer(frames) {
    machine.store(nullptr);
    switching.store(false);
    synth = new SynthArchitecture(8, sampleRate);  // Start with 8 voices
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
        for (unsigned int i = 0; i < framesPerBuffer; i++) {
            int32_t sample = 0;
            try {
                sample = machine->tick();
            } catch (...) {
                // Silently ignore errors to prevent crash
                sample = 0;
            }
            out[i] = std::max(-0.95f, std::min(0.95f, sample / 1280.0f));
        }
    } else if (engine->synth) {
        for (unsigned int i = 0; i < framesPerBuffer; i++) {
            float sample = engine->synth->process();
            out[i] = std::max(-0.95f, std::min(0.95f, sample));
        }
    } else {
        // No machine or synth - output silence
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
    
    err = Pa_OpenDefaultStream(&stream,
                               0,          // No input
                               1,          // Mono output
                               paFloat32,
                               sampleRate,
                               framesPerBuffer,
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
    // Signal that we're switching - audio thread will output silence
    switching.store(true, std::memory_order_release);
    
    // Small delay to let audio thread notice the flag
    std::this_thread::sleep_for(std::chrono::microseconds(100));
    
    Machine* oldMachine = machine.load(std::memory_order_acquire);
    // Reset the old machine before switching
    if (oldMachine && oldMachine != m) {
        oldMachine->reset();
    }
    machine.store(m, std::memory_order_release);
    if (m) {
        m->init();
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
