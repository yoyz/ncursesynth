#include "midi_capture.h"
#include <chrono>
#include <cstdio>
#include <iostream>
#include <algorithm>

MidiCapture* MidiCapture::instance = nullptr;

MidiCapture::MidiCapture(const std::string& path)
    : logPath(path + ".midi.txt"), rawPath(path), writeFailed(false), eventCount(0) {
    startTime = std::chrono::duration<double>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();
    FILE* f = fopen(logPath.c_str(), "w");
    if (!f) {
        std::string fallback = "tmp2/";
        size_t slash = path.rfind('/');
        fallback += (slash != std::string::npos) ? path.substr(slash + 1) : path;
        fallback += ".midi.txt";
        f = fopen(fallback.c_str(), "w");
        if (f) {
            logPath = fallback;
            std::cout << "[midi-capture] Falling back to " << fallback << std::endl;
        } else {
            writeFailed = true;
            fprintf(stderr, "[midi-capture] Cannot write to %s or tmp2/\n", path.c_str());
        }
    }
    if (f) fclose(f);
}

MidiCapture::~MidiCapture() {
    if (instance == this) instance = nullptr;
}

void MidiCapture::noteOn(int note, int vel) {
    double now = std::chrono::duration<double>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count() - startTime;
    events.push_back({now, 'N', note, vel});
    eventCount++;
}

void MidiCapture::noteOff(int note) {
    double now = std::chrono::duration<double>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count() - startTime;
    events.push_back({now, 'F', note, 0});
    eventCount++;
}

void MidiCapture::cc(int cc, int value) {
    double now = std::chrono::duration<double>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count() - startTime;
    events.push_back({now, 'C', cc, value});
    eventCount++;
}

void MidiCapture::finalize() {
    FILE* f = fopen(logPath.c_str(), "w");
    if (!f) {
        writeFailed = true;
        fprintf(stderr, "[midi-capture] Failed to write %s\n", logPath.c_str());
        return;
    }

    for (const auto& e : events) {
        switch (e.type) {
            case 'N':
                fprintf(f, "t=%.3fs  note_on  %d %d\n", e.time, e.p1, e.p2);
                break;
            case 'F':
                fprintf(f, "t=%.3fs  note_off %d\n", e.time, e.p1);
                break;
            case 'C':
                fprintf(f, "t=%.3fs  cc       %d %d\n", e.time, e.p1, e.p2);
                break;
        }
    }
    fclose(f);
}
