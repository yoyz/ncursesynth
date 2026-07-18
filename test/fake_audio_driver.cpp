#include "fake_audio_driver.h"
#include <portaudio.h>
#include <vector>
#include <string>
#include <mutex>
#include <atomic>
#include <cstdint>
#include <chrono>
#include <cmath>
#include <cstring>

// Global callback for headless mode - generates tone
static int headlessCallback(const void* inputBuffer, void* outputBuffer,
                              unsigned long frames,
                              const PaStreamCallbackTimeInfo* timeInfo,
                              PaStreamCallbackFlags statusFlags,
                              void* userData) {
    (void)inputBuffer;
    (void)timeInfo;
    (void)statusFlags;
    
    FakeAudioDriver* driver = static_cast<FakeAudioDriver*>(userData);
    
    if (driver->generateTone_) {
        float freq = driver->toneFrequency_;
        float sampleRate = static_cast<float>(driver->getSampleRate());
        for (uint32_t i = 0; i < frames; i++) {
            float t = static_cast<float>(driver->phaseAccumulator_ + i) / sampleRate;
            float sample = 0.5f * static_cast<float>(sin(2.0f * 3.14159f * freq * t));
            // Stereo output
            ((float*)outputBuffer)[i * 2] = sample;
            ((float*)outputBuffer)[i * 2 + 1] = sample;
        }
        driver->phaseAccumulator_ += frames;
    } else {
        // Silenced output
        memset(outputBuffer, 0, frames * sizeof(float) * 2);
    }
    
    return paContinue;
}

FakeAudioDriver::FakeAudioDriver() : stream_(nullptr), sampleRate_(48000), 
                                      framesPerBuffer_(256), isRunning_(false), 
                                      samplesCaptured_(0), dataMutex_(),
                                      generateTone_(false), toneNote_(0), toneFrequency_(440.0f),
                                      phaseAccumulator_(0.0) {}

FakeAudioDriver::~FakeAudioDriver() {
    stop();
}

bool FakeAudioDriver::start() {
    if (isRunning_.load()) return true;
    
    int device = Pa_GetDefaultInputDevice();
    if (device < 0) {
        device = Pa_GetDefaultOutputDevice();
        if (device < 0) device = 0;
    }
    
    // Check if we have a valid device
    if (device < 0 || !Pa_GetDeviceInfo(device)) {
        printf("No audio device found, using headless mode\n");
        generateTone_ = true;
        
        PaStreamParameters outParams;
        outParams.device = 0;
        outParams.sampleFormat = paFloat32;
        outParams.channelCount = 2;
        outParams.suggestedLatency = 0.05;
        
        PaError err = Pa_OpenStream(&stream_, &outParams, nullptr,
                                    static_cast<double>(sampleRate_), framesPerBuffer_,
                                    paClipOff,
                                    headlessCallback, this);
        
        if (err != paNoError) {
            printf("Failed to open audio stream: %s\n", Pa_GetErrorText(err));
            return false;
        }
        
        err = Pa_StartStream(stream_);
        if (err != paNoError) {
            printf("Failed to start audio stream: %s\n", Pa_GetErrorText(err));
            Pa_CloseStream(stream_);
            stream_ = nullptr;
            return false;
        }
        
        isRunning_.store(true);
        startTime_ = std::chrono::steady_clock::now();
        return true;
    }
    
    // Try to open stream with real device
    PaStreamParameters outParams;
    outParams.device = device;
    outParams.sampleFormat = paFloat32;
    outParams.channelCount = 2;
    outParams.suggestedLatency = Pa_GetDeviceInfo(device)->defaultLowOutputLatency;
    
    PaError err = Pa_OpenStream(&stream_, &outParams, nullptr,
                                static_cast<double>(sampleRate_), framesPerBuffer_,
                                paClipOff,
                                headlessCallback, this);
    
    if (err != paNoError) {
        printf("Failed to open audio stream: %s, using headless\n", Pa_GetErrorText(err));
        generateTone_ = true;
        return true;
    }
    
    err = Pa_StartStream(stream_);
    if (err != paNoError) {
        printf("Failed to start audio stream: %s, using headless\n", Pa_GetErrorText(err));
        Pa_CloseStream(stream_);
        stream_ = nullptr;
        generateTone_ = true;
        return true;
    }
    
    isRunning_.store(true);
    startTime_ = std::chrono::steady_clock::now();
    return true;
}

void FakeAudioDriver::stop() {
    if (!isRunning_.load()) return;
    
    if (stream_) {
        Pa_AbortStream(stream_);
        Pa_CloseStream(stream_);
        stream_ = nullptr;
    }
    isRunning_.store(false);
}

void FakeAudioDriver::clear() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    capturedData_.clear();
}

bool FakeAudioDriver::hasAudio() {
    return isRunning_.load() && samplesCaptured_.load() > 0;
}

bool FakeAudioDriver::hasMeaningfulAudio() {
    return isRunning_.load() && samplesCaptured_.load() > 0;
}

float FakeAudioDriver::getAudioEnergy() {
    if (!isRunning_.load()) return 0.0f;
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    double energy = 0.0;
    for (float s : capturedData_) {
        energy += s * s;
    }
    return static_cast<float>(std::sqrt(energy / capturedData_.size()));
}

uint64_t FakeAudioDriver::getSamplesCaptured() {
    return samplesCaptured_.load();
}

double FakeAudioDriver::getCaptureDuration() {
    auto now = std::chrono::steady_clock::now();
    auto duration = now - startTime_;
    return std::chrono::duration<double>(duration).count();
}

std::vector<float> FakeAudioDriver::getCapturedData() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    return capturedData_;
}

std::vector<int32_t> FakeAudioDriver::getInt16Data() {
    std::vector<float> floatData = getCapturedData();
    std::vector<int32_t> result;
    result.reserve(floatData.size());
    for (float f : floatData) {
        result.push_back(static_cast<int32_t>(f * 32767.0f));
    }
    return result;
}

bool FakeAudioDriver::initialize(int sampleRate, int frames) {
    sampleRate_ = static_cast<int>(sampleRate);
    framesPerBuffer_ = frames;
    capturedData_.clear();
    samplesCaptured_.store(0);
    startTime_ = std::chrono::steady_clock::now();
    generateTone_ = false;
    return true;
}

// MIDI methods for headless testing
void FakeAudioDriver::sendMidiNoteOn(int note, int velocity) {
    (void)velocity;
    if (generateTone_) {
        toneNote_ = note;
        toneFrequency_ = 440.0f * powf(2.0f, (note - 69) / 12.0f);
    }
}

void FakeAudioDriver::sendMidiNoteOff(int note) {
    (void)note;
    generateTone_ = false;
}

void FakeAudioDriver::sendCC(int cc, int value) {
    (void)cc;
    (void)value;
}

void FakeAudioDriver::sendAll() {
    // No-op
}

bool FakeAudioDriver::setToneMode(bool mode) {
    generateTone_ = mode;
    return true;
}

bool FakeAudioDriver::setToneFrequency(float freq) {
    toneFrequency_ = freq;
    return true;
}
