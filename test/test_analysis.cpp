#include "test_analysis.h"
#include <algorithm>
#include <complex>
#include <limits>

// Bit-reverse permutation for FFT
static void bitReverse(std::vector<std::complex<float>>& buffer, int logSize) {
    int n = 1 << logSize;
    std::vector<int> bitRev(n);
    
    for (int i = 0; i < n; i++) {
        int rev = 0;
        for (int j = 0; j < logSize; j++) {
            rev = (rev << 1) | (i & 1);
            i >>= 1;
        }
        bitRev[i] = rev;
    }
    
    for (int i = 0; i < n; i++) {
        if (i < bitRev[i]) {
            std::swap(buffer[i], buffer[bitRev[i]]);
        }
    }
}

// In-place FFT using Cooley-Tukey algorithm
static void fft(std::vector<std::complex<float>>& buffer, bool invert) {
    int n = buffer.size();
    int logSize = 0;
    
    // Find log2(n)
    while ((1 << logSize) < n) logSize++;
    
    bitReverse(buffer, logSize);
    
    // Butterfly operations
    for (int size = 2; size <= n; size *= 2) {
        int half = size / 2;
        float angle = (2.0f * (float)M_PI) / static_cast<float>(size) * (invert ? -1.0f : 1.0f);
        float wenr = cosf(angle);
        float weni = sinf(angle);
        
        std::complex<float> w(1.0f, 0.0f);
        
        for (int i = 0; i < n; i += size) {
            std::complex<float> wcur(1.0f, 0.0f);
            for (int j = 0; j < half; j++) {
                std::complex<float> u = buffer[i + j];
                std::complex<float> v = buffer[i + j + half] * wcur;
                buffer[i + j] = u + v;
                buffer[i + j + half] = u - v;
                wcur *= std::complex<float>(wenr, weni);
            }
        }
    }
}

// Compute FFT on float32 buffer
void TestAnalysis::compute(const float* buffer, int size, std::vector<float>& magnitudes) {
    // Allocate complex buffer
    std::vector<std::complex<float>> complexBuffer(size / 2 + 1);
    
    // Pad to power of 2 if necessary
    int bufferSize = 1;
    while (bufferSize < size) bufferSize *= 2;
    bufferSize = bufferSize / 2 + 1;
    
    // Create complex buffer from real samples
    for (int i = 0; i < bufferSize; i++) {
        complexBuffer[i] = std::complex<float>(buffer[i], 0.0f);
    }
    
    // Apply FFT
    fft(complexBuffer, false);
    
    // Compute magnitudes and normalize
    int numBins = bufferSize / 2;
    for (int i = 0; i < numBins; i++) {
        float mag = std::abs(complexBuffer[i]);
        magnitudes.push_back(mag / numBins);
    }
}

// Find fundamental frequency using peak detection
float TestAnalysis::findFundamentalFrequency(const std::vector<float>& magnitudes, float sampleRate) {
    if (magnitudes.empty()) return 0.0f;
    
    // Find peak in non-DC bins
    int peakBin = 1;
    float peakMag = magnitudes[1];
    
    for (int i = 2; i < static_cast<int>(magnitudes.size()); i++) {
        if (magnitudes[i] > peakMag) {
            peakMag = magnitudes[i];
            peakBin = i;
        }
    }
    
    float frequency = getBinFrequency(magnitudes, peakBin, sampleRate);
    
    // Check harmonics
    float threshold = peakMag * 0.3f;
    for (int i = peakBin - 1; i >= 2; i--) {
        if (magnitudes[i] > threshold) {
            return getBinFrequency(magnitudes, i, sampleRate);
        }
    }
    
    return frequency;
}

// Convert frequency to MIDI note number
float TestAnalysis::frequencyToMidi(float frequency) {
    if (frequency <= 0) return -1.0f;
    return 69.0f + 12.0f * log2f(frequency / 440.0f);
}

// Convert MIDI note number to frequency
float TestAnalysis::midiToFrequency(int midiNote) {
    return 440.0f * std::pow(2.0f, (static_cast<float>(midiNote) - 69.0f) / 12.0f);
}

// Check if buffer is silent
bool TestAnalysis::isSilent(const std::vector<float>& magnitudes, float threshold) {
    if (magnitudes.empty()) return true;
    
    float total = 0.0f;
    for (size_t i = 1; i < magnitudes.size(); i++) {
        total += magnitudes[i];
    }
    
    return total < threshold;
}

// Get magnitude at specific bin
float TestAnalysis::getMagnitudeAtBin(const std::vector<float>& magnitudes, int bin) {
    if (bin < 0 || bin >= static_cast<int>(magnitudes.size())) return 0.0f;
    return magnitudes[bin];
}

// Get frequency of specific bin
float TestAnalysis::getBinFrequency(const std::vector<float>& magnitudes, int bin, float sampleRate) {
    if (bin < 0 || bin >= static_cast<int>(magnitudes.size())) return 0.0f;
    return (static_cast<float>(bin) * sampleRate) / static_cast<float>(magnitudes.size());
}

// Compute RMS
float TestAnalysis::computeRMS(const std::vector<float>& data) {
    if (data.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (size_t i = 0; i < data.size(); i++) {
        sum += data[i] * data[i];
    }
    
    return std::sqrt(sum / data.size());
}

// Find max sample
float TestAnalysis::findMaxSample(const std::vector<float>& data) {
    if (data.empty()) return 0.0f;
    
    float maxSample = 0.0f;
    for (size_t i = 0; i < data.size(); i++) {
        float absVal = std::abs(data[i]);
        if (absVal > maxSample) {
            maxSample = absVal;
        }
    }
    
    return maxSample;
}

// Check for clipping
bool TestAnalysis::isClipped(const std::vector<float>& data, float threshold) {
    if (data.empty()) return false;
    
    for (size_t i = 0; i < data.size(); i++) {
        if (std::abs(data[i]) >= threshold) {
            return true;
        }
    }
    
    return false;
}
