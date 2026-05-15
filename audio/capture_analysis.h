#ifndef CAPTURE_ANALYSIS_H
#define CAPTURE_ANALYSIS_H

#include <string>
#include <vector>
#include <cstdio>
#include <cstdint>
#include "../test/fft_analyzer.h"

class CaptureAnalyzer {
public:
    CaptureAnalyzer(const std::string& path, int sampleRate = 48000);
    ~CaptureAnalyzer();

    void addSample(float sample);
    uint64_t getTotalSamples() const { return totalSamples; }
    const std::string& getRawPath() const { return rawPath; }
    const std::string& getAnalysisPath() const { return analysisPath; }
    bool failed() const { return writeFailed; }
    void finalize();

    static CaptureAnalyzer* instance;
    static void setInstance(CaptureAnalyzer* a) { instance = a; }
    static CaptureAnalyzer* getInstance() { return instance; }
    static void writeSample(float s) {
        if (instance) instance->addSample(s);
    }

private:
    FILE* rawFile;
    std::string rawPath;
    std::string analysisPath;
    int sampleRate;
    int chunkSize;
    std::vector<float> chunkBuffer;
    int chunkIndex;
    int totalSeconds;
    uint64_t totalSamples;
    bool writeFailed;

    void analyzeChunk(const std::vector<float>& chunk, int second);
};

#endif
