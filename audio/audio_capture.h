#ifndef AUDIO_CAPTURE_H
#define AUDIO_CAPTURE_H

#include <atomic>
#include <thread>
#include <vector>
#include <string>
#include <cstdint>
#include "../machine/Machine.h"
#include "audio_limiter.h"

class AudioCaptureDriver {
public:
    AudioCaptureDriver(int tcpPort, int sampleRate = 48000);
    ~AudioCaptureDriver();

    bool start(Machine* initialMachine);
    void stop();
    void setMachine(Machine* m);
    uint64_t getTotalSamples() const { return totalSamples.load(); }

private:
    int tcpPort;
    int sampleRate;
    std::atomic<bool> running;
    std::thread captureThread;
    std::thread serverThread;
    std::atomic<Machine*> machine;
    std::atomic<uint64_t> totalSamples;

    static constexpr size_t RING_CAPACITY = 48000 * 30;
    std::vector<float> ringBuffer;
    std::atomic<size_t> ringWrite;
    std::atomic<uint64_t> ringOverwriteCount;

    void captureLoop();
    void serverLoop();
    void handleClient(int clientFd);
    size_t getReadableSamples() const;

public:
    AudioLimiter limiter;
};

#endif
