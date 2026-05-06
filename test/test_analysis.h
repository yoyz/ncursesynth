#ifndef TEST_ANALYSIS_H
#define TEST_ANALYSIS_H

#include <vector>
#include <string>
#include <complex>
#include <cmath>

class FftAnalyzer {
public:
    FftAnalyzer();
    ~FftAnalyzer();

    double analyze(const float* buffer, int bufferSize);

private:
    void fft(float* input, int size, float* output);
    double frequencyToMidi(double frequency);
};

class TestAnalysis {
public:
    void compute(const float* buffer, int size, std::vector<float>& magnitudes);
    float findFundamentalFrequency(const std::vector<float>& magnitudes, float sampleRate);
    float frequencyToMidi(float frequency);
    float midiToFrequency(int midiNote);
    bool isSilent(const std::vector<float>& magnitudes, float threshold);
    float getMagnitudeAtBin(const std::vector<float>& magnitudes, int bin);
    float getBinFrequency(const std::vector<float>& magnitudes, int bin, float sampleRate);
    float computeRMS(const std::vector<float>& data);
    float findMaxSample(const std::vector<float>& data);
    bool isClipped(const std::vector<float>& data, float threshold);
};

double frequencyToMidi(double frequency);
double midiToFrequency(double midiNote);

#endif
