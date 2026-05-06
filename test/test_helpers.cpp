#include "test_helpers.h"
#include "test_engine.h"
#include "fft_analyzer.h"
#include "../machine/Machine.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <sys/resource.h>

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

// Test: Sound production - just calls tick() once, no real verification
bool runSoundProductionTests(Machine* machine, bool useFFT) {
    (void)useFFT;
    if (!machine) { printTestResult("sound", false, "No machine"); return false; }
    machine->init();
    machine->tick();
    printNotImplemented("sound");
    return false;
}

// Test: Silence - just calls tick(), no silence verification
bool runVolumeSilenceTest(Machine* machine, bool useFFT) {
    (void)useFFT;
    if (!machine) { printTestResult("silence", false, "No machine"); return false; }
    machine->init();
    machine->reset();
    machine->tick();
    printNotImplemented("silence");
    return false;
}

// Test: No clipping - just calls tick(), no clipping verification
bool runVolumeNoClipTest(Machine* machine, bool useFFT) {
    (void)useFFT;
    if (!machine) { printTestResult("no_clip", false, "No machine"); return false; }
    machine->init();
    machine->reset();
    machine->tick();
    printNotImplemented("no_clip");
    return false;
}

// Test: Note-on - just calls noteOn() and tick(), no verification
bool runNoteOnTests(Machine* machine, bool useFFT) {
    (void)useFFT;
    if (!machine) { printTestResult("note_on", false, "No machine"); return false; }
    machine->init();
    machine->reset();
    machine->noteOn();
    machine->tick();
    printNotImplemented("note_on_off");
    return false;
}

// Test: Note release - just calls noteOn/Off, no verification
bool runNoteReleaseTests(Machine* machine, bool useFFT) {
    (void)useFFT;
    if (!machine) { printTestResult("note_release", false, "No machine"); return false; }
    machine->init();
    machine->reset();
    machine->noteOn();
    machine->noteOff();
    machine->tick();
    printNotImplemented("note_release");
    return false;
}

// Test: Octave - does nothing
bool runOctaveTests(Machine* machine, bool useFFT) {
    (void)machine;
    (void)useFFT;
    printNotImplemented("octave");
    return false;
}

// Test: CC control - does nothing
bool runCCControlTests(Machine* machine, bool useFFT) {
    (void)machine;
    (void)useFFT;
    printNotImplemented("cc_control");
    return false;
}

// Test: Polyphony - does nothing
bool runPolyphonyTests(Machine* machine, bool useFFT) {
    (void)machine;
    (void)useFFT;
    printNotImplemented("polyphony");
    return false;
}

// Test: Filter envelope - does nothing
bool runFilterEnvelopeTests(Machine* machine, bool useFFT) {
    (void)machine;
    (void)useFFT;
    printNotImplemented("filter_env");
    return false;
}