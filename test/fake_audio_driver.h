#ifndef FAKE_AUDIO_DRIVER_H
#define FAKE_AUDIO_DRIVER_H

#include <portaudio.h>
#include <vector>
#include <string>
#include <mutex>
#include <atomic>
#include <cstdint>
#include <chrono>

class FakeAudioDriver {
public:
    FakeAudioDriver();
    ~FakeAudioDriver();

    // Initialize fake device
    bool initialize(int sampleRate = 48000, int frames = 256);
    
    // Start/stop capture
    bool start();
    void stop();
    
    // Get captured audio data
    std::vector<float> getCapturedData();
    std::vector<int32_t> getInt16Data();
    
    // Verify sound production
    bool hasAudio();
    bool hasMeaningfulAudio();
    float getAudioEnergy();
    
    // Get metrics
    uint64_t getSamplesCaptured();
    double getCaptureDuration();
    int32_t getSampleRate() const { return sampleRate_; }
    void clear();
    
    // MIDI methods for testing (no-ops for headless mode)
    void sendMidiNoteOn(int note, int velocity);
    void sendMidiNoteOff(int note);
    void sendCC(int cc, int value);
    void sendAll();
    
    // Tone control methods
    bool setToneMode(bool mode);
    bool setToneFrequency(float freq);

private:
    void captureAudio(float* output, unsigned long frames);

    PaStream* stream_;
    int sampleRate_;
    int framesPerBuffer_;
    std::vector<float> capturedData_;
    std::mutex dataMutex_;
    std::atomic<bool> isRunning_;
    std::atomic<uint64_t> samplesCaptured_;
    std::chrono::steady_clock::time_point startTime_;
    
    // Headless mode: generate simple tone
public:
    bool generateTone_;
    int toneNote_;
    float toneFrequency_;
private:
};

// Global callback function
int audioCallback_wrapper(const void* inputBuffer, void* outputBuffer,
                          unsigned long framesPerBuffer,
                          const PaStreamCallbackTimeInfo* timeInfo,
                          PaStreamCallbackFlags statusFlags,
                          void* userData);

#endif

