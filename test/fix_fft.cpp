#include "test_runner.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <algorithm>

void printUsage() {
    std::cout << "ncursesynth Test Runner\n\n";
    std::cout << "Usage: test_runner [OPTIONS]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --engine NAME     Run tests on specific engine\n";
    std::cout << "  --fft             Enable FFT analysis\n";
    std::cout << "  --verbose         Verbose output\n";
    std::cout << "  --help            Show this help\n\n";
}

int main(int argc, char** argv) {
    bool verbose = false;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--fft") {
            verbose = true;
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg == "--help") {
            printUsage();
            return 0;
        }
    }
    
    std::cout << "\n=== Testing FFT Analyzer ===" << std::endl;
    
    // Test with known power of 2 buffer
    std::vector<float> audio(1024, 0.0f);
    audio[0] = 1.0f;
    
    std::vector<float> magnitudes;
    FFTAnalyzer::compute(audio.data(), audio.size(), magnitudes);
    
    std::cout << "FFT computed successfully!" << std::endl;
    std::cout << "Magnitudes size: " << magnitudes.size() << std::endl;
    
    if (verbose) {
        float freq = FFTAnalyzer::findFundamentalFrequency(magnitudes, 48000.0f);
        std::cout << "Detected frequency: " << freq << " Hz" << std::endl;
    }
    
    return 0;
}
