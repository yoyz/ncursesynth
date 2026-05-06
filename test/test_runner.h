#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

#include "../machine/Machine.h"
#include "../machine/MachineManager.h"
#include "test_engine.h"
#include "fake_audio_driver.h"
#include "midi_simulator.h"
#include "test_reporter.h"
#include "test_analysis.h"
#include "fft_analyzer.h"
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <iostream>
#include <iomanip>

struct TestResult {
    std::string test;
    bool passed;
    std::string failure;
    double audioEnergy;
    double fftFrequency;
    double expectedFrequency;
    bool fftPassed;
    int note;
    int ccValue;
};

enum class TestCaseType {
    SOUND_PRODUCTION,
    NOTE_ON_OFF,
    CC_CONTROL,
    POLYPHONY,
    ENGINE_SWITCHING,
    FILTER_ENVELOPE,
    OSC_MODULATION,
    LFO,
    EFFECTS_CHAIN,
    FREQUENCY_VERIFICATION,
    OCTAVE_VERIFICATION,
    VOLUME_SILENCE,
    VOLUME_NOCLIP,
    NOTE_RELEASE,
    UNKNOWN
};

struct TestCase {
    std::string name;
    TestCaseType type;
    std::vector<int> requiredCCs;
    std::vector<int> requiredNotes;
    int minAudioEnergy;
    bool requiresPolyphony;
};

class TestRunner {
public:
    TestRunner(const std::string& outputDir = "test_results", bool verbose = false);
    ~TestRunner();

    void setVerbose(bool v) { verbose_ = v; }
    void setQuiet(bool q) { quiet_ = q; }
    
    // Test execution
    bool runSingleEngine(const std::string& engineName, 
                         const std::vector<std::string>& testNames = {},
                         bool useFFT = false);
    bool runAllEngines(const std::vector<std::string>& testNames = {},
                       bool useFFT = false);
    bool runParallelTests(const std::vector<std::string>& testNames = {});
    
    // Get results
    const std::vector<TestResult>& getResults() const { return results_; }
    int getPassedCount() const;
    const std::vector<std::string>& getEngineNames() const { return engines_; }
    int getFailedCount() const;
    double getPassRate() const;
    
    // Access reporter (for internal use)
    const TestReporter& getReporter() const { return reporter_; }

    // FFT-based test methods
    bool runFrequencyVerificationTests(Machine* machine, bool useFFT);
    bool runAllTests(Machine* machine, bool useFFT = false, const std::vector<std::string>& testNames = {});
    bool runOctaveVerificationTests(Machine* machine);
    bool runVolumeTests(Machine* machine);
    void addTestResult(const std::string& name, bool passed);
    bool runNoteReleaseTests(Machine* machine, bool useFFT);
    bool runFilterEnvelopeTests(Machine* machine, bool useFFT);
    bool runNoteOnOffTests(Machine* machine);
    bool runCCControlTests(Machine* machine, bool useFFT);
    bool runNoteOffTests(Machine* machine, bool useFFT);
    bool runEngineSwitchingTests(Machine* machine);
    bool runEngineInitTests(Machine* machine);

private:
    bool runTest(const TestResult& result);
    bool executeSoundProductionTest(Machine* machine, bool verbose);
    bool executeNoteOnOffTest(Machine* machine, bool verbose);
    bool executeCCControlTest(Machine* machine, bool verbose);
    bool executePolyphonyTest(Machine* machine, bool verbose);
    bool executeEngineSwitchingTest(Machine* machine, bool verbose);
    bool executeFrequencyVerificationTest(Machine* machine, int midiNote, float expectedFreq, bool useFFT, bool verbose);
    bool executeOctaveVerificationTest(Machine* machine, int midiNote, bool verbose);
    bool executeVolumeSilenceTest(Machine* machine, bool verbose);
    bool executeVolumeNoClipTest(Machine* machine, bool verbose);
    bool executeNoteReleaseTest(Machine* machine, bool verbose);
    bool executeFilterEnvelopeTest(Machine* machine, bool verbose);
    
    TestReporter reporter_;
    std::vector<std::string> engines_;
    bool verbose_;
    bool quiet_;
    std::atomic<bool> running_;
    std::vector<TestResult> results_;
};

#endif

// Global test functions (implemented in test_helpers.cpp)
bool runSoundProductionTests(Machine* machine, bool useFFT);
bool runVolumeSilenceTest(Machine* machine, bool useFFT);
bool runVolumeNoClipTest(Machine* machine, bool useFFT);
bool runNoteOnTests(Machine* machine, bool useFFT);
bool runNoteOffTests(Machine* machine, bool useFFT);
bool runOctaveTests(Machine* machine, bool useFFT);
bool runCCControlTests(Machine* machine, bool useFFT);
bool runPolyphonyTests(Machine* machine, bool useFFT);
bool runFilterEnvelopeTests(Machine* machine, bool useFFT);
