#include "test_runner.h"
#include "test_helpers.h"
#include "../machine/Ncursesynth/NcursesynthMachine.h"
#include "../machine/PBSynth/PBSynthMachine.h"
#include "../machine/Cursynth/CursynthMachine.h"
#include "../machine/Twytch/TwytchsynthMachine.h"
#include "../machine/Digits/DigitsMachine.h"
#include "../machine/Ambika/AmbikaMachine.h"
#include <iostream>
#include <iomanip>
#include <atomic>
#include <mutex>

bool runFilterFull3Tests(Machine* machine, bool useFFT);

static std::atomic<bool> g_running_(false);
static std::mutex g_machineMutex_;
static Machine* g_activeMachine_ = nullptr;

static int g_totalPassed = 0;
static int g_totalFailed = 0;

static const std::vector<std::string> g_availableEngines = {
    "ncursesynth",
    "pbsynth",
    "cursynth",
    "twytch",
    "digits",
    "ambika"
};

static const std::vector<std::string> g_availableTests = {
    "sound",
    "silence",
    "no_clip",
    "note_on_off",
    "note_release",
    "octave",
    "cc_control",
    "polyphony",
    "filter_env",
    "voice_level",
    "envelope",
    "preset",
    "filter_full",
    "filter_full2",
    "filter_full3",
    "cc_mapping",
    "midi_mapping_load",
    "preset_roundtrip",
    "lfo_modulation",
    "osc_crossmod",
    "effects",
    "filter_switch",
    "note_steal",
    "portamento",
    "edge_cases",
    "bipolar_params",
    "long_running",
    "init_pitch"
};

static const char* g_engineDescriptions[] = {
    "Original ncursesynth engine",
    "PBSynth (8-voice polyphonic)",
    "Cursynth engine",
    "Twytch (Helm-based) engine",
    "Digits Phase Distortion engine",
    "Ambika (6-voice, 22 oscillator algorithms)"
};

static const char* g_testDescriptions[] = {
    "Basic sound production",
    "Volume -> 0 produces silence",
    "Max volume doesn't clip",
    "Note on/off trigger",
    "Note release handling",
    "Octave progression tests",
    "CC parameter control",
    "Multi-voice handling",
    "Filter envelope tests",
    "Voice level increase with polyphony",
    "Amplitude envelope A/D/S/R shape",
    "Preset load/save/reload",
    "Comprehensive filter tests (all 11 filter types, FFT analysis)",
    "Filter frequency response tests (notes vs cutoff, LPF/HPF behavior)",
    "Extended filter sweep tests (cutoff + resonance + combined)",
    "CC mapping correctness (applyCC changes expected params)",
    "MIDI mapping file loading and validation",
    "Preset save/reload parameter roundtrip accuracy",
    "LFO modulation produces audible variation",
    "Oscillator cross-modulation changes harmonic content",
    "Effects chain (delay, reverb, chorus, distortion)",
    "Filter type switching during playback",
    "Note steal behavior with polyphony overflow",
    "Portamento/legato glide between notes",
    "Edge cases (note 0/127, velocity 0, CC boundaries)",
    "Bipolar parameter roundtrip accuracy",
    "Long-running stability (60s note loop)",
    "Default init pitch accuracy (simulates interactive UI init)"
};

TestRunner::TestRunner(const std::string& outputDir, bool verbose)
    : reporter_(outputDir), engines_({"ncursesynth", "pbsynth", "cursynth", "twytch", "digits", "ambika"}), verbose_(verbose), quiet_(false), running_(false), results_() {}

TestRunner::~TestRunner() {
    std::lock_guard<std::mutex> lock(g_machineMutex_);
    if (g_activeMachine_) {
        delete g_activeMachine_;
        g_activeMachine_ = nullptr;
    }
}

int TestRunner::getPassedCount() const {
    int count = 0;
    for (const auto& r : results_) {
        if (r.passed) count++;
    }
    return count;
}

int TestRunner::getFailedCount() const {
    return results_.size() - getPassedCount();
}

double TestRunner::getPassRate() const {
    int total = results_.size();
    if (total == 0) return 0.0;
    return (static_cast<double>(getPassedCount()) / total) * 100.0;
}

std::vector<std::string> TestRunner::getPassedNames() const {
    std::vector<std::string> names;
    for (const auto& r : results_)
        if (r.passed) names.push_back(r.test);
    return names;
}

std::vector<std::string> TestRunner::getFailedNames() const {
    std::vector<std::string> names;
    for (const auto& r : results_)
        if (!r.passed) names.push_back(r.test);
    return names;
}

static Machine* createMachine(const std::string& engineName) {
    if (engineName == "ncursesynth") return new NcursesynthMachine();
    if (engineName == "pbsynth") return new PBSynthMachine();
    if (engineName == "cursynth") return new CursynthMachine(8);
    if (engineName == "twytch") return new TwytchsynthMachine();
    if (engineName == "digits") return new DigitsMachine();
    if (engineName == "ambika") return new AmbikaMachine();
    return nullptr;
}

bool TestRunner::runSingleEngine(const std::string& engineName,
                                       const std::vector<std::string>& testNames,
                                       bool useFFT) {
    results_.clear();
    std::cout << "\n=== Running tests for " << engineName << " ===" << std::endl;

    Machine* machine = createMachine(engineName);
    if (!machine) {
        std::cerr << "  [ERROR] Failed to create machine for " << engineName << std::endl;
        return false;
    }

    machine->init();
    std::cout << "  [INFO] Machine " << engineName << " initialized" << std::endl;

    g_activeMachine_ = machine;
    g_running_.store(true);

    if (!runAllTests(machine, useFFT, testNames)) {
        std::cout << "  [FAIL] Some tests failed" << std::endl;
        delete machine;
        g_activeMachine_ = nullptr;
        g_running_.store(false);
        return false;
    }

    std::cout << "\n=== All Tests Complete ===" << std::endl;
    delete machine;
    g_activeMachine_ = nullptr;
    g_running_.store(false);
    return true;
}

bool TestRunner::runAllEngines(const std::vector<std::string>& testNames,
                                   bool useFFT) {
    bool allSuccess = true;
    results_.clear();

    for (const auto& name : engines_) {
        std::cout << "\n=== Running tests for " << name << " ===" << std::endl;
        Machine* machine = createMachine(name);

        if (!machine) {
            std::cerr << "  [ERROR] Failed to create machine for " << name << std::endl;
            allSuccess = false;
            continue;
        }

        machine->init();
        bool success = runAllTests(machine, useFFT, testNames);

        if (!success) allSuccess = false;

        delete machine;
        g_activeMachine_ = nullptr;
    }

    return allSuccess;
}

struct TestEntry {
    const char* name;
    bool (*func)(Machine*, bool);
};

static const TestEntry g_testEntries[] = {
    {"sound",             runSoundProductionTests},
    {"silence",           runVolumeSilenceTest},
    {"no_clip",           runVolumeNoClipTest},
    {"note_on_off",       runNoteOnTests},
    {"note_release",      runNoteReleaseTests},
    {"octave",            runOctaveTests},
    {"cc_control",        runCCControlTests},
    {"polyphony",         runPolyphonyTests},
    {"filter_env",        runFilterEnvelopeTests},
    {"voice_level",       runVoiceLevelTests},
    {"envelope",          runEnvelopeTests},
    {"preset",            runPresetTests},
    {"filter_full",       runFilterFullTests},
    {"filter_full2",      runFilterFull2Tests},
    {"filter_full3",      runFilterFull3Tests},
    {"cc_mapping",        runCCMappingTests},
    {"midi_mapping_load", runMidiMappingLoadTests},
    {"preset_roundtrip",  runPresetRoundtripTests},
    {"lfo_modulation",    runLFOModulationTests},
    {"osc_crossmod",      runOscCrossModTests},
    {"effects",           runEffectsTests},
    {"filter_switch",     runFilterSwitchTests},
    {"note_steal",        runNoteStealTests},
    {"portamento",        runPortamentoTests},
    {"edge_cases",        runEdgeCaseTests},
    {"bipolar_params",    runBipolarParamTests},
    {"long_running",      runLongRunningTests},
    {"init_pitch",        runDefaultInitPitchTest},
};

bool TestRunner::runAllTests(Machine* machine, bool useFFT, const std::vector<std::string>& testNames) {
    std::cout << "\n=== Running All Tests ===" << std::endl;

    auto shouldRunTest = [&testNames](const std::string& testName) {
        if (testNames.empty()) return true;
        for (const auto& name : testNames) {
            if (name == testName) return true;
        }
        return false;
    };

    if (g_activeMachine_ != nullptr) {
        std::lock_guard<std::mutex> lock(g_machineMutex_);
        g_activeMachine_ = nullptr;
        g_running_.store(false);
    }

    int passedCount = 0;
    int failedCount = 0;
    bool allPassed = true;

    int numEntries = sizeof(g_testEntries) / sizeof(g_testEntries[0]);
    for (int i = 0; i < numEntries; i++) {
        const auto& entry = g_testEntries[i];
        if (!shouldRunTest(entry.name)) continue;

        std::cout << "  [RUN] " << entry.name << std::endl;
        reporter_.startTest(entry.name);
        bool passed = entry.func(machine, useFFT);
        reporter_.endTest(entry.name, passed, useFFT);
        if (passed) passedCount++; else failedCount++;
        results_.push_back({entry.name, passed, "", 0, 0, 0, false, 0, 0});
        allPassed = passed && allPassed;
    }

    g_totalPassed += passedCount;
    g_totalFailed += failedCount;

    return allPassed;
}

bool TestRunner::executeSoundProductionTest(Machine* machine, bool verbose) {
    if (!machine) {
        results_.push_back({"sound", false, "No machine provided", 0, 0, 0, false, 0, 0});
        return false;
    }
    bool passed = runSoundProductionTests(machine, verbose);
    results_.push_back({"sound", passed, "", 0, 0, 0, false, 0, 0});
    return passed;
}

bool TestRunner::executeNoteOnOffTest(Machine* machine, bool verbose) {
    if (!machine) {
        results_.push_back({"note_on_off", false, "No machine provided", 0, 0, 0, false, 0, 0});
        return false;
    }
    bool passed = runNoteOnTests(machine, verbose);
    results_.push_back({"note_on_off", passed, "", 0, 0, 0, false, 0, 0});
    return passed;
}

bool TestRunner::executeCCControlTest(Machine* machine, bool verbose) {
    if (!machine) {
        results_.push_back({"cc_control", false, "No machine provided", 0, 0, 0, false, 0, 0});
        return false;
    }
    bool passed = runCCControlTests(machine, verbose);
    results_.push_back({"cc_control", passed, "", 0, 0, 0, false, 0, 0});
    return passed;
}

bool TestRunner::executePolyphonyTest(Machine* machine, bool verbose) {
    if (!machine) {
        results_.push_back({"polyphony", false, "No machine provided", 0, 0, 0, false, 0, 0});
        return false;
    }
    bool passed = runPolyphonyTests(machine, verbose);
    results_.push_back({"polyphony", passed, "", 0, 0, 0, false, 0, 0});
    return passed;
}

bool TestRunner::executeOctaveVerificationTest(Machine* machine, int midiNote, bool verbose) {
    (void)midiNote;
    if (!machine) {
        results_.push_back({"octave", false, "No machine provided", 0, 0, 0, false, 0, 0});
        return false;
    }
    bool passed = runOctaveTests(machine, verbose);
    results_.push_back({"octave", passed, "", 0, 0, 0, false, 0, 0});
    return passed;
}

bool TestRunner::executeVolumeSilenceTest(Machine* machine, bool verbose) {
    if (!machine) {
        results_.push_back({"silence", false, "No machine provided", 0, 0, 0, false, 0, 0});
        return false;
    }
    bool passed = runVolumeSilenceTest(machine, verbose);
    results_.push_back({"silence", passed, "", 0, 0, 0, false, 0, 0});
    return passed;
}

bool TestRunner::executeVolumeNoClipTest(Machine* machine, bool verbose) {
    if (!machine) {
        results_.push_back({"no_clip", false, "No machine provided", 0, 0, 0, false, 0, 0});
        return false;
    }
    bool passed = runVolumeNoClipTest(machine, verbose);
    results_.push_back({"no_clip", passed, "", 0, 0, 0, false, 0, 0});
    return passed;
}

bool TestRunner::executeNoteReleaseTest(Machine* machine, bool verbose) {
    if (!machine) {
        results_.push_back({"note_release", false, "No machine provided", 0, 0, 0, false, 0, 0});
        return false;
    }
    bool passed = runNoteReleaseTests(machine, verbose);
    results_.push_back({"note_release", passed, "", 0, 0, 0, false, 0, 0});
    return passed;
}

bool TestRunner::executeFilterEnvelopeTest(Machine* machine, bool verbose) {
    if (!machine) {
        results_.push_back({"filter_env", false, "No machine provided", 0, 0, 0, false, 0, 0});
        return false;
    }
    bool passed = runFilterEnvelopeTests(machine, verbose);
    results_.push_back({"filter_env", passed, "", 0, 0, 0, false, 0, 0});
    return passed;
}


void printUsage() {
    std::cout << "ncursesynth Test Runner\n\n";
    std::cout << "Usage: test_runner --engine ENGINE --tests TESTS [OPTIONS]\n";
    std::cout << "       test_runner --all-engines --all-tests [OPTIONS]\n\n";

    std::cout << "Examples:\n";
    std::cout << "  test_runner --engine ncursesynth --tests sound,note_on\n";
    std::cout << "  test_runner --all-engines --all-tests --fft\n";
    std::cout << "  test_runner --engine pbsynth --tests all --verbose\n\n";

    std::cout << "Available Engines:\n";
    for (size_t i = 0; i < g_availableEngines.size(); i++) {
        std::cout << "  " << g_availableEngines[i];
        int padding = 12 - g_availableEngines[i].length();
        for (int j = 0; j < padding; j++) std::cout << " ";
        std::cout << "- " << g_engineDescriptions[i] << "\n";
    }
    std::cout << "\n";

    std::cout << "Available Tests:\n";
    for (size_t i = 0; i < g_availableTests.size(); i++) {
        std::cout << "  " << g_availableTests[i];
        int padding = 18 - g_availableTests[i].length();
        for (int j = 0; j < padding; j++) std::cout << " ";
        std::cout << "- " << g_testDescriptions[i] << "\n";
    }
    std::cout << "  all               - All tests above\n\n";

    std::cout << "Options:\n";
    std::cout << "  -h, --help           Show this help\n";
    std::cout << "  --list-engines      List available engines\n";
    std::cout << "  --list-tests        List available tests\n";
    std::cout << "  --engine NAME       Specify engine (required unless --all-engines)\n";
    std::cout << "  --all-engines       Run tests on all engines\n";
    std::cout << "  --tests LIST        Specify tests (comma-separated, required unless --all-tests)\n";
    std::cout << "  --all-tests        Run all available tests\n";
    std::cout << "  --verbose          Verbose output\n";
    std::cout << "  --quiet             Minimal output\n";
    std::cout << "  --fft               Enable FFT analysis\n";
}

void listEngines() {
    std::cout << "Available Engines:\n";
    for (size_t i = 0; i < g_availableEngines.size(); i++) {
        std::cout << "  " << g_availableEngines[i];
        int padding = 12 - g_availableEngines[i].length();
        for (int j = 0; j < padding; j++) std::cout << " ";
        std::cout << "- " << g_engineDescriptions[i] << "\n";
    }
}

void listTests() {
    std::cout << "Available Tests:\n";
    for (size_t i = 0; i < g_availableTests.size(); i++) {
        std::cout << "  " << g_availableTests[i];
        int padding = 18 - g_availableTests[i].length();
        for (int j = 0; j < padding; j++) std::cout << " ";
        std::cout << "- " << g_testDescriptions[i] << "\n";
    }
    std::cout << "  all               - All tests above\n";
}

int main(int argc, char** argv) {
    if (argc == 1) {
        printUsage();
        return 0;
    }

    std::string engine;
    bool runAllEngines = false;
    bool runAllTests = false;
    bool useFFT = false;
    bool verbose = false;
    bool quiet = false;
    std::vector<std::string> tests;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            printUsage();
            return 0;
        } else if (arg == "--list-engines") {
            listEngines();
            return 0;
        } else if (arg == "--list-tests") {
            listTests();
            return 0;
        } else if (arg == "--engine" && i + 1 < argc) {
            engine = argv[++i];
        } else if (arg == "--all-engines") {
            runAllEngines = true;
        } else if (arg == "--tests" && i + 1 < argc) {
            std::string testStr = argv[++i];
            if (testStr == "all") {
                runAllTests = true;
            } else {
                size_t pos = 0;
                while ((pos = testStr.find(',', pos)) != std::string::npos) {
                    std::string segment = testStr.substr(0, pos);
                    if (!segment.empty()) tests.push_back(segment);
                    testStr.erase(0, pos + 1);
                }
                if (!testStr.empty()) tests.push_back(testStr);
            }
        } else if (arg == "--all-tests") {
            runAllTests = true;
        } else if (arg == "--fft") {
            useFFT = true;
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg == "--quiet") {
            quiet = true;
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            printUsage();
            return 1;
        }
    }

    if (!runAllEngines && engine.empty()) {
        std::cerr << "Error: --engine or --all-engines required" << std::endl;
        printUsage();
        return 1;
    }
    if (!runAllTests && tests.empty()) {
        std::cerr << "Error: --tests or --all-tests required" << std::endl;
        printUsage();
        return 1;
    }

    if (quiet) verbose = false;

    TestRunner runner("reports", verbose);
    bool success = true;

    if (runAllEngines) {
        success = runner.runAllEngines(tests, useFFT);
    } else {
        success = runner.runSingleEngine(engine, tests, useFFT);
    }

    {
        auto passed = runner.getPassedNames();
        auto failed = runner.getFailedNames();
        auto all = passed;
        all.insert(all.end(), failed.begin(), failed.end());
        runner.getReporter().printReport(passed, failed, all);
    }

    return success ? 0 : 1;
}
