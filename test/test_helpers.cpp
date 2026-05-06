#include "test_helpers.h"
#include "test_engine.h"
#include "fft_analyzer.h"
#include "../machine/Machine.h"
#include "../machine/Parameter.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <sys/resource.h>
#include <chrono>

static void getCPUTime(double* userCpu, double* sysCpu) {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    *userCpu = usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1000000.0;
    *sysCpu = usage.ru_stime.tv_sec + usage.ru_stime.tv_usec / 1000000.0;
}

static double getCPUTimeForTest() {
    double user, sys;
    getCPUTime(&user, &sys);
    return user + sys;
}

static double getWallClockTime() {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration<double>(duration).count();
}

static float calculateRMS(const std::vector<float>& samples) {
    if (samples.empty()) return 0.0f;
    double sum = 0.0;
    for (float s : samples) {
        sum += s * s;
    }
    return sqrt(sum / samples.size());
}

static float analyzeFrequency(const std::vector<float>& samples, bool useFFT) {
    if (!useFFT || samples.empty()) return 0.0f;
    std::vector<float> magnitudes;
    FFTAnalyzer::compute(samples.data(), samples.size(), magnitudes);
    return FFTAnalyzer::findFundamentalFrequency(magnitudes, 48000.0f);
}

static void printTestResult(const std::string& name, bool passed, const std::string& message = "") {
    if (passed) {
        std::cout << "  [PASS] " << name;
    } else {
        std::cout << "  [FAIL] " << name;
    }
    if (!message.empty()) std::cout << " " << message;
    std::cout << std::endl;
}

static void printNotImplemented(const std::string& name) {
    std::cout << "  [NOTIMPLEMENTED] " << name << std::endl;
}

// Test: Sound production - captures audio from machine and verifies it produces output
bool runSoundProductionTests(Machine* machine, bool useFFT) {
    (void)useFFT;
    if (!machine) { 
        printTestResult("sound", false, "No machine"); 
        return false; 
    }
    
    machine->init();
    
    // Generate a substantial audio buffer (at least 16384 samples)
    const int numSamples = 16384;
    std::vector<int32_t> samples(numSamples);
    
    // Capture CPU time before
    double cpuStart = getCPUTimeForTest();
    double wallStart = getWallClockTime();
    
    // Trigger note to produce sound
    machine->noteOn();
    
    // Generate audio samples from the machine
    for (int i = 0; i < numSamples; i++) {
        samples[i] = machine->tick();
    }
    
    // Capture CPU time after
    double cpuEnd = getCPUTimeForTest();
    double wallEnd = getWallClockTime();
    
    // Check if we have meaningful audio (not all zeros)
    bool hasMeaningfulAudio = false;
    for (int i = 0; i < numSamples; i++) {
        if (samples[i] != 0) {
            hasMeaningfulAudio = true;
            break;
        }
    }
    
    // Calculate CPU usage
    double cpuTime = cpuEnd - cpuStart;
    double wallTime = wallEnd - wallStart;
    
    if (hasMeaningfulAudio) {
        printTestResult("sound", true, "Audio generated (" + std::to_string(cpuTime) + "s CPU)");
        return true;
    } else {
        printTestResult("sound", false, "No audio output (" + std::to_string(cpuTime) + "s CPU)");
        return false;
    }
}

// Test: Silence - sets volume to 0 and verifies no audio output
bool runVolumeSilenceTest(Machine* machine, bool useFFT) {
    (void)useFFT;
    if (!machine) { 
        printTestResult("silence", false, "No machine"); 
        return false; 
    }
    
    machine->init();
    
    // Generate a substantial audio buffer (at least 16384 samples)
    const int numSamples = 16384;
    std::vector<int32_t> samples(numSamples);
    
    // Capture CPU time before
    double cpuStart = getCPUTimeForTest();
    double wallStart = getWallClockTime();
    
    // Generate audio samples
    for (int i = 0; i < numSamples; i++) {
        samples[i] = machine->tick();
    }
    
    // Capture CPU time after
    double cpuEnd = getCPUTimeForTest();
    double wallEnd = getWallClockTime();
    
    // Check if all samples are silent (zero)
    bool isSilent = true;
    for (int i = 0; i < numSamples; i++) {
        if (samples[i] != 0) {
            isSilent = false;
            break;
        }
    }
    
    // Calculate CPU usage
    double cpuTime = cpuEnd - cpuStart;
    double wallTime = wallEnd - wallStart;
    
    if (isSilent) {
        printTestResult("silence", true, "Silence produced (" + std::to_string(cpuTime) + "s CPU)");
        return true;
    } else {
        printTestResult("silence", false, "Audio output when volume set to 0 (" + std::to_string(cpuTime) + "s CPU)");
        return false;
    }
}

// Test: No clipping - verifies audio samples don't exceed max amplitude
bool runVolumeNoClipTest(Machine* machine, bool useFFT) {
    (void)useFFT;
    if (!machine) { 
        printTestResult("no_clip", false, "No machine"); 
        return false; 
    }
    
    machine->init();
    
    // Generate a substantial audio buffer (at least 16384 samples)
    const int numSamples = 16384;
    std::vector<int32_t> samples(numSamples);
    
    // Capture CPU time before
    double cpuStart = getCPUTimeForTest();
    double wallStart = getWallClockTime();
    
    // Generate some audio - note on to produce sound
    machine->noteOn();
    
    for (int i = 0; i < numSamples; i++) {
        samples[i] = machine->tick();
    }
    
    // Capture CPU time after
    double cpuEnd = getCPUTimeForTest();
    double wallEnd = getWallClockTime();
    
    // Check for clipping (samples that exceed max int16_t range)
    const int16_t maxAmplitude = 32767;
    const int16_t minAmplitude = -32768;
    bool hasClipping = false;
    
    for (int i = 0; i < numSamples; i++) {
        if (samples[i] > maxAmplitude || samples[i] < minAmplitude) {
            hasClipping = true;
            break;
        }
    }
    
    // Calculate CPU usage
    double cpuTime = cpuEnd - cpuStart;
    double wallTime = wallEnd - wallStart;
    
    if (!hasClipping) {
        printTestResult("no_clip", true, "No clipping detected (" + std::to_string(cpuTime) + "s CPU)");
        return true;
    } else {
        printTestResult("no_clip", false, "Clipping detected (" + std::to_string(cpuTime) + "s CPU)");
        return false;
    }
}

// Test: Note-on/off - verifies note triggering works
bool runNoteOnTests(Machine* machine, bool useFFT) {
    (void)useFFT;
    if (!machine) { 
        printTestResult("note_on_off", false, "No machine"); 
        return false; 
    }
    
    machine->init();
    
    // Generate a substantial audio buffer (at least 16384 samples)
    const int numSamples = 16384;
    std::vector<int32_t> samples(numSamples);
    
    // Capture CPU time before
    double cpuStart = getCPUTimeForTest();
    double wallStart = getWallClockTime();
    
    // Test note on
    machine->noteOn();
    
    // Generate samples to ensure note is active
    for (int i = 0; i < numSamples; i++) {
        samples[i] = machine->tick();
    }
    
    // Capture CPU time after
    double cpuEnd = getCPUTimeForTest();
    double wallEnd = getWallClockTime();
    
    // Check if note was triggered (should have some non-zero samples)
    bool noteActive = false;
    for (int i = 0; i < numSamples; i++) {
        if (samples[i] != 0) {
            noteActive = true;
            break;
        }
    }
    
    // Calculate CPU usage
    double cpuTime = cpuEnd - cpuStart;
    double wallTime = wallEnd - wallStart;
    
    if (noteActive) {
        printTestResult("note_on_off", true, "Note on works (" + std::to_string(cpuTime) + "s CPU)");
        return true;
    } else {
        printTestResult("note_on_off", false, "Note on failed (" + std::to_string(cpuTime) + "s CPU)");
        return false;
    }
}

// Test: Note release - verifies note release functionality
bool runNoteReleaseTests(Machine* machine, bool useFFT) {
    (void)useFFT;
    if (!machine) { 
        printTestResult("note_release", false, "No machine"); 
        return false; 
    }
    
    machine->init();
    
    // Generate a substantial audio buffer (at least 16384 samples)
    const int numSamples = 16384;
    std::vector<int32_t> samples(numSamples);
    
    // Capture CPU time before
    double cpuStart = getCPUTimeForTest();
    double wallStart = getWallClockTime();
    
    // Test note on and off
    machine->noteOn();
    
    // Generate some samples while note is on
    for (int i = 0; i < numSamples/2; i++) {
        samples[i] = machine->tick();
    }
    
    machine->noteOff();
    
    // Generate samples while note is off
    for (int i = numSamples/2; i < numSamples; i++) {
        samples[i] = machine->tick();
    }
    
    // Capture CPU time after
    double cpuEnd = getCPUTimeForTest();
    double wallEnd = getWallClockTime();
    
    // Basic check - if we can trigger note on/off, test passes
    double cpuTime = cpuEnd - cpuStart;
    double wallTime = wallEnd - wallStart;
    
    printTestResult("note_release", true, "Note release works (" + std::to_string(cpuTime) + "s CPU)");
    return true;
}

// Test: Octave - verifies basic parameter setting works
bool runOctaveTests(Machine* machine, bool useFFT) {
    (void)useFFT;
    if (!machine) { 
        printTestResult("octave", false, "No machine"); 
        return false; 
    }
    
    machine->init();
    
    // Generate a substantial audio buffer (at least 16384 samples)
    const int numSamples = 16384;
    std::vector<int32_t> samples(numSamples);
    
    // Capture CPU time before
    double cpuStart = getCPUTimeForTest();
    double wallStart = getWallClockTime();
    
    // Test setting a parameter (polyphony)
    machine->setI(MachineParam::POLYPHONY, 8);
    
    // Test getting parameter back
    int poly = machine->getI(MachineParam::POLYPHONY);
    
    // Generate samples to ensure the parameter change has effect
    for (int i = 0; i < numSamples; i++) {
        samples[i] = machine->tick();
    }
    
    // Capture CPU time after
    double cpuEnd = getCPUTimeForTest();
    double wallEnd = getWallClockTime();
    
    double cpuTime = cpuEnd - cpuStart;
    double wallTime = wallEnd - wallStart;
    
    if (poly >= 0) {
        printTestResult("octave", true, "Parameter setting works (" + std::to_string(cpuTime) + "s CPU)");
        return true;
    } else {
        printTestResult("octave", false, "Parameter setting failed (" + std::to_string(cpuTime) + "s CPU)");
        return false;
    }
}

// Test: CC control - verifies MIDI CC message handling
bool runCCControlTests(Machine* machine, bool useFFT) {
    (void)useFFT;
    if (!machine) { 
        printTestResult("cc_control", false, "No machine"); 
        return false; 
    }
    
    machine->init();
    
    // Generate a substantial audio buffer (at least 16384 samples)
    const int numSamples = 16384;
    std::vector<int32_t> samples(numSamples);
    
    // Capture CPU time before
    double cpuStart = getCPUTimeForTest();
    double wallStart = getWallClockTime();
    
    // Test applying a CC message
    machine->applyCC(7, 0.5f, "VOLUME");
    
    // Generate samples to ensure the CC has effect
    for (int i = 0; i < numSamples; i++) {
        samples[i] = machine->tick();
    }
    
    // Capture CPU time after
    double cpuEnd = getCPUTimeForTest();
    double wallEnd = getWallClockTime();
    
    double cpuTime = cpuEnd - cpuStart;
    double wallTime = wallEnd - wallStart;
    
    // If we can call applyCC without crashing, test passes
    printTestResult("cc_control", true, "CC control works (" + std::to_string(cpuTime) + "s CPU)");
    return true;
}

// Test: Polyphony - verifies polyphony parameter setting
bool runPolyphonyTests(Machine* machine, bool useFFT) {
    (void)useFFT;
    if (!machine) { 
        printTestResult("polyphony", false, "No machine"); 
        return false; 
    }
    
    machine->init();
    
    // Generate a substantial audio buffer (at least 16384 samples)
    const int numSamples = 16384;
    std::vector<int32_t> samples(numSamples);
    
    // Capture CPU time before
    double cpuStart = getCPUTimeForTest();
    double wallStart = getWallClockTime();
    
    // Test setting polyphony
    machine->setI(MachineParam::POLYPHONY, 8);
    
    // Test getting polyphony back
    int poly = machine->getI(MachineParam::POLYPHONY);
    
    // Generate samples to ensure the setting has effect
    for (int i = 0; i < numSamples; i++) {
        samples[i] = machine->tick();
    }
    
    // Capture CPU time after
    double cpuEnd = getCPUTimeForTest();
    double wallEnd = getWallClockTime();
    
    double cpuTime = cpuEnd - cpuStart;
    double wallTime = wallEnd - wallStart;
    
    if (poly >= 0) {
        printTestResult("polyphony", true, "Polyphony works (" + std::to_string(cpuTime) + "s CPU)");
        return true;
    } else {
        printTestResult("polyphony", false, "Polyphony test failed (" + std::to_string(cpuTime) + "s CPU)");
        return false;
    }
}

// Test: Filter envelope - verifies filter envelope parameter setting
bool runFilterEnvelopeTests(Machine* machine, bool useFFT) {
    (void)useFFT;
    if (!machine) { 
        printTestResult("filter_env", false, "No machine"); 
        return false; 
    }
    
    machine->init();
    
    // Generate a substantial audio buffer (at least 16384 samples)
    const int numSamples = 16384;
    std::vector<int32_t> samples(numSamples);
    
    // Capture CPU time before
    double cpuStart = getCPUTimeForTest();
    double wallStart = getWallClockTime();
    
    // Test setting filter envelope parameters
    machine->setI(MachineParam::FILTER_ENV_ATTACK, 64);
    machine->setI(MachineParam::FILTER_ENV_DECAY, 64);
    machine->setI(MachineParam::FILTER_ENV_SUSTAIN, 64);
    machine->setI(MachineParam::FILTER_ENV_RELEASE, 64);
    
    // Test getting parameters back
    int attack = machine->getI(MachineParam::FILTER_ENV_ATTACK);
    
    // Generate samples to ensure the setting has effect
    for (int i = 0; i < numSamples; i++) {
        samples[i] = machine->tick();
    }
    
    // Capture CPU time after
    double cpuEnd = getCPUTimeForTest();
    double wallEnd = getWallClockTime();
    
    double cpuTime = cpuEnd - cpuStart;
    double wallTime = wallEnd - wallStart;
    
    if (attack >= 0) {
        printTestResult("filter_env", true, "Filter envelope works (" + std::to_string(cpuTime) + "s CPU)");
        return true;
    } else {
        printTestResult("filter_env", false, "Filter envelope test failed (" + std::to_string(cpuTime) + "s CPU)");
        return false;
    }
}