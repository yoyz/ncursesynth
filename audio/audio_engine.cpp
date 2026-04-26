#include "audio_engine.h"
#include <iostream>
#include <cstring>
#include <algorithm>

AudioEngine::AudioEngine(int rate, int frames) 
    : stream(nullptr), isRunning(false), sampleRate(rate), framesPerBuffer(frames) {
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
    
    // Process audio
    for (unsigned int i = 0; i < framesPerBuffer; i++) {
        float sample = engine->synth->process();
        // Soft clipping to prevent harsh distortion
        out[i] = std::max(-0.95f, std::min(0.95f, sample));
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

void AudioEngine::shutdown() {
    stop();
    if (stream) {
        Pa_CloseStream(stream);
        stream = nullptr;
    }
    Pa_Terminate();
}
