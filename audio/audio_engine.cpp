#include "audio_engine.h"
#include <iostream>
#include <cstring>
#include <algorithm>

AudioEngine::AudioEngine(int rate, int frames)
    : stream(nullptr), synth(nullptr), machine(nullptr), isRunning(false), sampleRate(rate), framesPerBuffer(frames) {
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

    Machine* machine = engine->machine;
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
    // Reset the old machine before switching
    if (machine && machine != m) {
        machine->reset();
    }
    machine = m;
    if (m) {
        m->init();
    }
}

void AudioEngine::shutdown() {
    stop();
    if (stream) {
        Pa_CloseStream(stream);
        stream = nullptr;
    }
    Pa_Terminate();
}
