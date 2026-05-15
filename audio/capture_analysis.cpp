#include "capture_analysis.h"
#include <cstdio>
#include <cmath>
#include <algorithm>

CaptureAnalyzer* CaptureAnalyzer::instance = nullptr;

CaptureAnalyzer::CaptureAnalyzer(const std::string& path, int sampleRate)
    : rawFile(nullptr), rawPath(path), analysisPath(path + ".analysis.txt"),
      sampleRate(sampleRate), chunkSize(sampleRate),
      chunkIndex(0), totalSeconds(0), totalSamples(0), writeFailed(false) {
    chunkBuffer.reserve(chunkSize);
    std::remove(analysisPath.c_str());
    rawFile = fopen(path.c_str(), "wb");
    if (!rawFile) {
        std::string fallback = "tmp2/";
        size_t slash = path.rfind('/');
        fallback += (slash != std::string::npos) ? path.substr(slash + 1) : path;
        rawFile = fopen(fallback.c_str(), "wb");
        if (rawFile) {
            rawPath = fallback;
            analysisPath = fallback + ".analysis.txt";
            std::remove(analysisPath.c_str());
        } else {
            writeFailed = true;
            fprintf(stderr, "[capture-analysis] Cannot write to %s or tmp2/\n", path.c_str());
        }
    }
}

CaptureAnalyzer::~CaptureAnalyzer() {
    if (rawFile) fclose(rawFile);
    if (instance == this) instance = nullptr;
}

void CaptureAnalyzer::addSample(float sample) {
    if (writeFailed) return;

    if (rawFile) {
        fwrite(&sample, sizeof(float), 1, rawFile);
    }

    chunkBuffer.push_back(sample);
    totalSamples++;

    if ((int)chunkBuffer.size() >= chunkSize) {
        analyzeChunk(chunkBuffer, totalSeconds);
        totalSeconds++;
        chunkBuffer.clear();
    }
}

void CaptureAnalyzer::finalize() {
    if (writeFailed) return;
    if (!chunkBuffer.empty()) {
        analyzeChunk(chunkBuffer, totalSeconds);
        totalSeconds++;
        chunkBuffer.clear();
    }
    if (rawFile) {
        fclose(rawFile);
        rawFile = nullptr;
    }
}

void CaptureAnalyzer::analyzeChunk(const std::vector<float>& chunk, int second) {
    if (chunk.empty()) return;

    double sumSq = 0.0;
    for (float s : chunk) sumSq += (double)s * s;
    float rms = sqrtf((float)(sumSq / chunk.size()));

    std::vector<float> magnitudes;
    int fftSize = 1;
    while (fftSize * 2 <= (int)chunk.size()) fftSize *= 2;
    if (fftSize < 32) fftSize = 32;

    FFTAnalyzer::compute(chunk.data(), fftSize, magnitudes);
    float freq = FFTAnalyzer::findFundamentalFrequency(magnitudes, (float)sampleRate);
    float midiNote = FFTAnalyzer::frequencyToMidi(freq);

    FILE* af = fopen(analysisPath.c_str(), "a");
    if (af) {
        if (freq > 0.0f) {
            fprintf(af, "t=%ds  fft=%.2fHz  midi=%.1f  rms=%.6f\n",
                    second, freq, midiNote, rms);
        } else {
            fprintf(af, "t=%ds  fft=--  midi=--  rms=%.6f\n",
                    second, rms);
        }
        fclose(af);
    } else {
        fprintf(stderr, "[capture-analysis] Failed to write analysis to %s\n", analysisPath.c_str());
        writeFailed = true;
    }
}
