#ifndef FFT_ANALYZER_H
#define FFT_ANALYZER_H

#include <vector>
#include <cmath>
#include <complex>

class FFTAnalyzer {
public:
    // Compute FFT on float32 buffer
    // buffer: audio samples
    // size: number of samples (must be power of 2)
    // magnitudes: output magnitudes (N/2 + 1 bins)
    static void compute(const float* buffer, int size, std::vector<float>& magnitudes);

    // Find fundamental frequency from magnitudes
    // magnitudes: FFT magnitudes (bins 0 to N/2)
    // sampleRate: audio sample rate
    // Returns: fundamental frequency in Hz
    static float findFundamentalFrequency(const std::vector<float>& magnitudes, float sampleRate);

    // Convert frequency to MIDI note number
    // frequency: frequency in Hz
    // Returns: MIDI note number (69 = A4 = 440 Hz)
    static float frequencyToMidi(float frequency);

    // Convert MIDI note number to frequency
    // midiNote: MIDI note number
    // Returns: frequency in Hz
    static float midiToFrequency(int midiNote);

    // Check if buffer is silent
    // magnitudes: FFT magnitudes
    // threshold: silence threshold
    static bool isSilent(const std::vector<float>& magnitudes, float threshold = 0.01f);

    // Get magnitude at specific bin
    static float getMagnitudeAtBin(const std::vector<float>& magnitudes, int bin);

    // Get frequency of specific bin
    static float getBinFrequency(const std::vector<float>& magnitudes, int bin, float sampleRate);

    // Get bin number at specific frequency
    static int getBinAtFrequency(float frequency, float sampleRate, int bufferSize);

private:
    // In-place FFT using Cooley-Tukey algorithm
    // buffer: complex array (passed by reference)
    static void fft(std::vector<std::complex<float>>& buffer, bool invert = false);

    // Bit-reverse permutation
    static void bitReverse(std::vector<std::complex<float>>& buffer, int logSize);
};

#endif
