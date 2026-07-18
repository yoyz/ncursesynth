#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

#include "../machine/Machine.h"
#include "../machine/MachineManager.h"
#include "test_engine.h"
#include "fake_audio_driver.h"
#include "midi_simulator.h"
#include "test_reporter.h"
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

class TestRunner {
public:
    TestRunner(const std::string& outputDir = "test_results", bool verbose = false);
    ~TestRunner();

    void setVerbose(bool v) { verbose_ = v; }
    void setQuiet(bool q) { quiet_ = q; }

    bool runSingleEngine(const std::string& engineName,
                         const std::vector<std::string>& testNames = {},
                         bool useFFT = false);
    bool runAllEngines(const std::vector<std::string>& testNames = {},
                       bool useFFT = false);

    const std::vector<TestResult>& getResults() const { return results_; }
    int getPassedCount() const;
    const std::vector<std::string>& getEngineNames() const { return engines_; }
    int getFailedCount() const;
    double getPassRate() const;
    std::vector<std::string> getPassedNames() const;
    std::vector<std::string> getFailedNames() const;

    TestReporter& getReporter() { return reporter_; }

    bool runAllTests(Machine* machine, bool useFFT = false, const std::vector<std::string>& testNames = {});

private:
    bool executeSoundProductionTest(Machine* machine, bool verbose);
    bool executeNoteOnOffTest(Machine* machine, bool verbose);
    bool executeCCControlTest(Machine* machine, bool verbose);
    bool executePolyphonyTest(Machine* machine, bool verbose);
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

bool runSoundProductionTests(Machine* machine, bool useFFT);
bool runVolumeSilenceTest(Machine* machine, bool useFFT);
bool runVolumeNoClipTest(Machine* machine, bool useFFT);
bool runNoteOnTests(Machine* machine, bool useFFT);
bool runNoteReleaseTests(Machine* machine, bool useFFT);
bool runOctaveTests(Machine* machine, bool useFFT);
bool runCCControlTests(Machine* machine, bool useFFT);
bool runPolyphonyTests(Machine* machine, bool useFFT);
bool runFilterEnvelopeTests(Machine* machine, bool useFFT);
bool runVoiceLevelTests(Machine* machine, bool useFFT);
bool runEnvelopeTests(Machine* machine, bool useFFT);
bool runPresetTests(Machine* machine, bool useFFT);
bool runFilterFullTests(Machine* machine, bool useFFT);
bool runFilterFull2Tests(Machine* machine, bool useFFT);
bool runCCMappingTests(Machine* machine, bool useFFT);
bool runMidiMappingLoadTests(Machine* machine, bool useFFT);
bool runPresetRoundtripTests(Machine* machine, bool useFFT);
bool runLFOModulationTests(Machine* machine, bool useFFT);
bool runOscCrossModTests(Machine* machine, bool useFFT);
bool runEffectsTests(Machine* machine, bool useFFT);
bool runFilterSwitchTests(Machine* machine, bool useFFT);
bool runNoteStealTests(Machine* machine, bool useFFT);
bool runPortamentoTests(Machine* machine, bool useFFT);
bool runEdgeCaseTests(Machine* machine, bool useFFT);
bool runBipolarParamTests(Machine* machine, bool useFFT);
bool runLongRunningTests(Machine* machine, bool useFFT);
