#include "audio_capture.h"
#include "capture_analysis.h"
#include "audio_level.h"
#include <iostream>
#include <cmath>
#include <chrono>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>
#include <fstream>

AudioCaptureDriver::AudioCaptureDriver(int tcpPort, int sampleRate)
    : tcpPort(tcpPort), sampleRate(sampleRate), running(false),
      machine(nullptr), totalSamples(0),
      ringBuffer(RING_CAPACITY, 0.0f), ringWrite(0), ringOverwriteCount(0) {}

AudioCaptureDriver::~AudioCaptureDriver() {
    stop();
}

bool AudioCaptureDriver::start(Machine* initialMachine) {
    if (running.load()) return true;

    machine.store(initialMachine);
    running.store(true);
    totalSamples.store(0);
    ringWrite.store(0);
    ringOverwriteCount.store(0);

    captureThread = std::thread(&AudioCaptureDriver::captureLoop, this);
    serverThread = std::thread(&AudioCaptureDriver::serverLoop, this);

    return true;
}

void AudioCaptureDriver::stop() {
    running.store(false);
    if (captureThread.joinable()) captureThread.join();
    if (serverThread.joinable()) serverThread.join();
}

void AudioCaptureDriver::setMachine(Machine* m) {
    machine.store(m);
}

size_t AudioCaptureDriver::getReadableSamples() const {
    uint64_t total = totalSamples.load();
    uint64_t overwrites = ringOverwriteCount.load();
    size_t cap = RING_CAPACITY;
    if (total >= cap) {
        return cap;
    }
    return static_cast<size_t>(total);
}

void AudioCaptureDriver::captureLoop() {
    auto nextTick = std::chrono::steady_clock::now();
    uint64_t tickCount = 0;

    while (running.load()) {
        Machine* m = machine.load();
        if (m) {
            int32_t sample = m->tick();
            float f = sample / 8192.0f;
            float absF = fabsf(f);
            if (absF > 0.85f) {
                float over = (absF - 0.85f) / (1.0f - 0.85f);
                f = (f > 0 ? 1.0f : -1.0f) * (0.85f + 0.10f * (1.0f - 1.0f / (1.0f + over)));
            }
            CaptureAnalyzer::writeSample(f);
            AudioLevel::update(f);

            size_t idx = ringWrite.load();
            ringBuffer[idx] = f;
            ringWrite.store((idx + 1) % RING_CAPACITY);
            if (totalSamples.load() >= RING_CAPACITY) {
                ringOverwriteCount.fetch_add(1);
            }
            totalSamples.fetch_add(1);
        }

        tickCount++;
        nextTick += std::chrono::microseconds(1000000 / sampleRate);
        std::this_thread::sleep_until(nextTick);
    }
}

void AudioCaptureDriver::serverLoop() {
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0) {
        std::cerr << "[capture] Failed to create socket" << std::endl;
        return;
    }

    int opt = 1;
    setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(tcpPort);

    if (bind(serverFd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "[capture] Failed to bind on port " << tcpPort << std::endl;
        close(serverFd);
        return;
    }

    if (listen(serverFd, 5) < 0) {
        std::cerr << "[capture] Failed to listen" << std::endl;
        close(serverFd);
        return;
    }

    std::cout << "[capture] Audio capture server on port " << tcpPort << std::endl;

    while (running.load()) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int clientFd = accept(serverFd, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientFd < 0) {
            if (!running.load()) break;
            continue;
        }
        handleClient(clientFd);
        close(clientFd);
    }

    close(serverFd);
}

void AudioCaptureDriver::handleClient(int clientFd) {
    char buf[1024];
    ssize_t n = read(clientFd, buf, sizeof(buf) - 1);
    if (n <= 0) return;

    buf[n] = '\0';
    std::string cmd(buf);

    if (cmd.find("snapshot") == 0) {
        size_t count = getReadableSamples();
        uint32_t beCount = htonl(static_cast<uint32_t>(count));
        write(clientFd, &beCount, sizeof(beCount));

        size_t start = 0;
        uint64_t total = totalSamples.load();
        size_t cap = RING_CAPACITY;
        if (total > cap) {
            start = (ringWrite.load() + cap - count) % cap;
        }

        size_t sent = 0;
        while (sent < count && running.load()) {
            size_t idx = (start + sent) % cap;
            float sample = ringBuffer[idx];
            ssize_t w = write(clientFd, &sample, sizeof(float));
            if (w <= 0) break;
            sent++;
        }
    } else if (cmd.find("save ") == 0) {
        std::string path = cmd.substr(5);
        while (!path.empty() && (path.back() == '\n' || path.back() == '\r'))
            path.pop_back();

        size_t count = getReadableSamples();
        std::ofstream out(path, std::ios::binary);
        if (out.is_open()) {
            size_t start = 0;
            uint64_t total = totalSamples.load();
            size_t cap = RING_CAPACITY;
            if (total > cap) {
                start = (ringWrite.load() + cap - count) % cap;
            }
            for (size_t i = 0; i < count; i++) {
                size_t idx = (start + i) % cap;
                float sample = ringBuffer[idx];
                out.write(reinterpret_cast<const char*>(&sample), sizeof(float));
            }
            out.close();
            std::string ok = "OK " + std::to_string(count) + " samples written\n";
            write(clientFd, ok.c_str(), ok.size());
        } else {
            std::string err = "ERR cannot open file\n";
            write(clientFd, err.c_str(), err.size());
        }
    } else if (cmd.find("status") == 0) {
        std::string status = "samples=" + std::to_string(totalSamples.load())
                           + " ring=" + std::to_string(getReadableSamples()) + "\n";
        write(clientFd, status.c_str(), status.size());
    }
}
