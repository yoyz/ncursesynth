#ifndef TEST_ANALYSIS_H
#define TEST_ANALYSIS_H

#include <vector>
#include <string>
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

double frequencyToMidi(double frequency);
double midiToFrequency(double midiNote);

#endif
