#include "fft_analyzer.h"
#include <algorithm>
#include <limits>
#include <cmath>

// Bit-reverse permutation for FFT
void FFTAnalyzer::bitReverse(std::vector<std::complex<float>>& buffer, int logSize) {
    int n = 1 << logSize;
    std::vector<int> bitRev(n);
    
    for (int i = 0; i < n; i++) {
        int rev = 0;
        int temp = i;
        for (int j = 0; j < logSize; j++) {
            rev = (rev << 1) | (temp & 1);
            temp >>= 1;
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
void FFTAnalyzer::fft(std::vector<std::complex<float>>& buffer, bool invert) {
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
void FFTAnalyzer::compute(const float* buffer, int size, std::vector<float>& magnitudes) {
    // Round up to power of 2
    int n = 1;
    while (n < size) n *= 2;
    
    // Create complex buffer padded to power of 2
    std::vector<std::complex<float>> complexBuffer(n);
    
    // Copy real samples, zero-pad if needed
    for (int i = 0; i < size && i < n; i++) {
        complexBuffer[i] = std::complex<float>(buffer[i], 0.0f);
    }
    for (int i = size; i < n; i++) {
        complexBuffer[i] = std::complex<float>(0.0f, 0.0f);
    }
    
    // Apply FFT
    fft(complexBuffer, false);
    
    // Compute magnitudes (only first size/2 bins are meaningful for real input)
    int numBins = size / 2;
    for (int i = 0; i < numBins; i++) {
        float mag = std::abs(complexBuffer[i]);
        magnitudes.push_back(mag / numBins);
    }
}

// Find fundamental frequency using autocorrelation-like approach
float FFTAnalyzer::findFundamentalFrequency(const std::vector<float>& magnitudes, float sampleRate) {
    if (magnitudes.empty()) return 0.0f;
    
    int n = magnitudes.size();
    float binWidth = sampleRate / (2.0f * n);
    
    // Find top peaks
    struct Peak { int bin; float mag; };
    std::vector<Peak> peaks;
    
    for (int i = 3; i < n - 3; i++) {
        if (magnitudes[i] > magnitudes[i-1] && 
            magnitudes[i] > magnitudes[i-2] &&
            magnitudes[i] > magnitudes[i+1] && 
            magnitudes[i] > magnitudes[i+2]) {
            if (magnitudes[i] > 0.001f) { // Noise threshold
                peaks.push_back({i, magnitudes[i]});
            }
        }
    }
    
    if (peaks.empty()) return 0.0f;
    
    // Sort by magnitude (strongest first)
    std::sort(peaks.begin(), peaks.end(), [](const Peak& a, const Peak& b) {
        return a.mag > b.mag;
    });
    
    // Find fundamental by checking if peaks are harmonically related
    for (const auto& peak : peaks) {
        float peakFreq = peak.bin * binWidth;
        
        // Try this peak as fundamental
        for (const auto& other : peaks) {
            if (&peak == &other) continue;
            
            float ratio = other.bin / static_cast<float>(peak.bin);
            float expectedRatio = std::round(ratio);
            
            // If this peak is a harmonic of the first peak
            if (std::abs(ratio - expectedRatio) < 0.1f) {
                return peakFreq; // This is likely the fundamental
            }
        }
        
        // Try this peak as harmonic of a lower fundamental
        for (const auto& candidate : peaks) {
            if (candidate.bin >= peak.bin) continue;
            
            float ratio = peak.bin / static_cast<float>(candidate.bin);
            
            // Check if ratio is close to an integer (harmonic relationship)
            float nearestInt = std::round(ratio);
            if (ratio > 0 && std::abs(ratio - nearestInt) < 0.1f) {
                // Check if this fundamental has harmonics
                for (const auto& check : peaks) {
                    if (check.bin <= candidate.bin) continue;
                    float checkRatio = check.bin / static_cast<float>(candidate.bin);
                    if (std::abs(checkRatio - std::round(checkRatio)) < 0.15f) {
                        // This looks like a valid fundamental
                        return candidate.bin * binWidth;
                    }
                }
            }
        }
    }
    
    // Fallback to strongest peak
    return peaks[0].bin * binWidth;
}

// Convert frequency to MIDI note number
float FFTAnalyzer::frequencyToMidi(float frequency) {
    if (frequency <= 0) return -1.0f;
    return 69.0f + 12.0f * log2f(frequency / 440.0f);
}

// Convert MIDI note number to frequency
float FFTAnalyzer::midiToFrequency(int midiNote) {
    return 440.0f * std::pow(2.0f, (static_cast<float>(midiNote) - 69.0f) / 12.0f);
}

// Check if buffer is silent
bool FFTAnalyzer::isSilent(const std::vector<float>& magnitudes, float threshold) {
    if (magnitudes.empty()) return true;
    
    // Sum magnitudes (excluding DC)
    float total = 0.0f;
    for (size_t i = 1; i < magnitudes.size(); i++) {
        total += magnitudes[i];
    }
    
    return total < threshold;
}

// Get magnitude at specific bin
float FFTAnalyzer::getMagnitudeAtBin(const std::vector<float>& magnitudes, int bin) {
    if (bin < 0 || bin >= static_cast<int>(magnitudes.size())) return 0.0f;
    return magnitudes[bin];
}

// Get frequency of specific bin
float FFTAnalyzer::getBinFrequency(const std::vector<float>& magnitudes, int bin, float sampleRate) {
    if (bin < 0 || bin >= static_cast<int>(magnitudes.size())) return 0.0f;
    return (static_cast<float>(bin) * sampleRate) / static_cast<float>(magnitudes.size());
}

// Get bin number at specific frequency
int FFTAnalyzer::getBinAtFrequency(float frequency, float sampleRate, int bufferSize) {
    if (frequency <= 0) return 0;
    return static_cast<int>((frequency * bufferSize) / sampleRate);
}
