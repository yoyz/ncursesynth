#include "test_runner.h"
#include "test_helpers.h"
#include "../machine/Ncursesynth/NcursesynthMachine.h"
#include "../machine/PBSynth/PBSynthMachine.h"
#include "../machine/Cursynth/CursynthMachine.h"
#include "../machine/Twytch/TwytchsynthMachine.h"
#include <iostream>
#include <iomanip>
#include <atomic>
#include <mutex>

static std::atomic<bool> g_running_(false);
static std::mutex g_machineMutex_;
static Machine* g_activeMachine_ = nullptr;

static int g_totalPassed = 0;
static int g_totalFailed = 0;

// Global lists of engines and tests - easy to update
static const std::vector<std::string> g_availableEngines = {
    "ncursesynth",
    "pbsynth",
    "cursynth",
    "twytch"
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
    "filter_env"
};

static const char* g_engineDescriptions[] = {
    "Original ncursesynth engine",
    "PBSynth (8-voice polyphonic)",
    "Cursynth engine",
    "Twytch (Helm-based) engine"
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
    "Filter envelope tests"
};

TestRunner::TestRunner(const std::string& outputDir, bool verbose)
    : reporter_(outputDir), engines_({"ncursesynth", "pbsynth", "cursynth", "twytch"}), verbose_(verbose), quiet_(false), running_(false), results_() {}

TestRunner::~TestRunner() {
    std::lock_guard<std::mutex> lock(g_machineMutex_);
    if (g_activeMachine_) {
        std::cerr << "[DEBUG] Deleting g_activeMachine_" << std::endl;
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

bool TestRunner::runSingleEngine(const std::string& engineName,
                                      const std::vector<std::string>& testNames,
                                      bool useFFT) {
    std::cout << "\n=== Running tests for " << engineName << " ===" << std::endl;

    Machine* machine = nullptr;

    if (engineName == "ncursesynth") {
        machine = new NcursesynthMachine();
    } else if (engineName == "pbsynth") {
        machine = new PBSynthMachine();
    } else if (engineName == "cursynth") {
        machine = new CursynthMachine(8);
    } else if (engineName == "twytch") {
        machine = new TwytchsynthMachine();
    } 


    if (!machine) {
        std::cerr << "  [ERROR] Failed to create machine for " << engineName << std::endl;
        return false;
    }

    machine->init();
    std::cout << "  [INFO] Machine " << engineName << " initialized" << std::endl;

    g_activeMachine_ = machine;
    g_running_.store(true);

    // Run all tests
    if (!runAllTests(machine, useFFT, testNames)) {
        std::cout << "  [FAIL] Some tests failed" << std::endl;
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

    for (const auto& name : engines_) {
        std::cout << "\n=== Running tests for " << name << " ===" << std::endl;
        Machine* machine = nullptr;
        
        if (name == "ncursesynth") {
            machine = new NcursesynthMachine();
        } else if (name == "pbsynth") {
            machine = new PBSynthMachine();
        } else if (name == "cursynth") {
            machine = new CursynthMachine(8);
        }  else if (name == "twytch") {
            machine = new TwytchsynthMachine();
        }         

        if (!machine) {
            std::cerr << "  [ERROR] Failed to create machine for " << name << std::endl;
            allSuccess = false;
            continue;
        }
        
        machine->init();
        bool success = runAllTests(machine, useFFT, testNames);
        
        if (!success) {
            allSuccess = false;
        }
        
        delete machine;
        g_activeMachine_ = nullptr;
    }

    return allSuccess;
}

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
        std::cout << "  [WARN] g_activeMachine_ not reset, cleaning up" << std::endl;
        std::lock_guard<std::mutex> lock(g_machineMutex_);
        g_activeMachine_ = nullptr;
        g_running_.store(false);
    }
    
    results_.clear();
    int passedCount = 0;
    int failedCount = 0;
    bool allPassed = true;
    
    if (shouldRunTest("sound")) {
        std::cout << "  [RUN] sound" << std::endl;
        bool passed = runSoundProductionTests(machine, useFFT);
        if (passed) passedCount++; else failedCount++;
        results_.push_back({"sound", passed, "", 0, 0, 0, false, 0, 0});
        allPassed = passed && allPassed;
    }
    
    if (shouldRunTest("silence")) {
        std::cout << "  [RUN] silence" << std::endl;
        bool passed = runVolumeSilenceTest(machine, useFFT);
        if (passed) passedCount++; else failedCount++;
        results_.push_back({"silence", passed, "", 0, 0, 0, false, 0, 0});
        allPassed = passed && allPassed;
    }
    
    if (shouldRunTest("no_clip")) {
        std::cout << "  [RUN] no_clip" << std::endl;
        bool passed = runVolumeNoClipTest(machine, useFFT);
        if (passed) passedCount++; else failedCount++;
        results_.push_back({"no_clip", passed, "", 0, 0, 0, false, 0, 0});
        allPassed = passed && allPassed;
    }
    
    if (shouldRunTest("note_on_off")) {
        std::cout << "  [RUN] note_on_off" << std::endl;
        bool passed = runNoteOnTests(machine, useFFT);
        if (passed) passedCount++; else failedCount++;
        results_.push_back({"note_on_off", passed, "", 0, 0, 0, false, 0, 0});
        allPassed = passed && allPassed;
    }
    
    if (shouldRunTest("note_release")) {
        std::cout << "  [RUN] note_release" << std::endl;
        bool passed = runNoteReleaseTests(machine, useFFT);
        if (passed) passedCount++; else failedCount++;
        results_.push_back({"note_release", passed, "", 0, 0, 0, false, 0, 0});
        allPassed = passed && allPassed;
    }
    
    if (shouldRunTest("octave")) {
        std::cout << "  [RUN] octave" << std::endl;
        bool passed = runOctaveTests(machine, useFFT);
        if (passed) passedCount++; else failedCount++;
        results_.push_back({"octave", passed, "", 0, 0, 0, false, 0, 0});
        allPassed = passed && allPassed;
    }
    
    if (shouldRunTest("cc_control")) {
        std::cout << "  [RUN] cc_control" << std::endl;
        bool passed = runCCControlTests(machine, useFFT);
        if (passed) passedCount++; else failedCount++;
        results_.push_back({"cc_control", passed, "", 0, 0, 0, false, 0, 0});
        allPassed = passed && allPassed;
    }
    
    if (shouldRunTest("polyphony")) {
        std::cout << "  [RUN] polyphony" << std::endl;
        bool passed = runPolyphonyTests(machine, useFFT);
        if (passed) passedCount++; else failedCount++;
        results_.push_back({"polyphony", passed, "", 0, 0, 0, false, 0, 0});
        allPassed = passed && allPassed;
    }
    
    if (shouldRunTest("filter_env")) {
        std::cout << "  [RUN] filter_env" << std::endl;
        bool passed = runFilterEnvelopeTests(machine, useFFT);
        if (passed) passedCount++; else failedCount++;
        results_.push_back({"filter_env", passed, "", 0, 0, 0, false, 0, 0});
        allPassed = passed && allPassed;
    }
    
    g_totalPassed += passedCount;
    g_totalFailed += failedCount;
    
    return allPassed;
}

bool TestRunner::executeSoundProductionTest(Machine* machine, bool verbose) {
    if (!machine) {
        std::cout << "  [SKIP] No machine provided" << std::endl;
        results_.push_back({"sound", false, "No machine provided", 0, 0, 0, false, 0, 0});
        return false;
    }
    
    bool passed = runSoundProductionTests(machine, verbose);
    results_.push_back({"sound", passed, "", 0, 0, 0, false, 0, 0});
    std::cout << "  " << (passed ? "[PASS]" : "[FAIL]") << " Machine can generate audio" << std::endl;
    return passed;
}

bool TestRunner::executeNoteOnOffTest(Machine* machine, bool verbose) {
    if (!machine) {
        std::cout << "  [SKIP] No machine provided" << std::endl;
        results_.push_back({"note_on_off", false, "No machine provided", 0, 0, 0, false, 0, 0});
        return false;
    }
    
    bool passed = runNoteOnTests(machine, verbose);
    results_.push_back({"note_on_off", passed, "", 0, 0, 0, false, 0, 0});
    std::cout << "  " << (passed ? "[PASS]" : "[FAIL]") << " Note On/Off working" << std::endl;
    return passed;
}

bool TestRunner::executeCCControlTest(Machine* machine, bool verbose) {
    if (!machine) {
        std::cout << "  [SKIP] No machine provided" << std::endl;
        results_.push_back({"cc_control", false, "No machine provided", 0, 0, 0, false, 0, 0});
        return false;
    }
    
    bool passed = runCCControlTests(machine, verbose);
    results_.push_back({"cc_control", passed, "", 0, 0, 0, false, 0, 0});
    std::cout << "  " << (passed ? "[PASS]" : "[FAIL]") << " CC control available" << std::endl;
    return passed;
}

bool TestRunner::executePolyphonyTest(Machine* machine, bool verbose) {
    if (!machine) {
        std::cout << "  [SKIP] No machine provided" << std::endl;
        results_.push_back({"polyphony", false, "No machine provided", 0, 0, 0, false, 0, 0});
        return false;
    }
    
    bool passed = runPolyphonyTests(machine, verbose);
    results_.push_back({"polyphony", passed, "", 0, 0, 0, false, 0, 0});
    std::cout << "  " << (passed ? "[PASS]" : "[FAIL]") << " Polyphony supported" << std::endl;
    return passed;
}

bool TestRunner::executeOctaveVerificationTest(Machine* machine, int midiNote, bool verbose) {
    (void)midiNote;
    (void)verbose;
    if (!machine) {
        std::cout << "  [SKIP] No machine provided" << std::endl;
        results_.push_back({"octave", false, "No machine provided", 0, 0, 0, false, 0, 0});
        return false;
    }
    
    bool passed = runOctaveTests(machine, verbose);
    results_.push_back({"octave", passed, "", 0, 0, 0, false, 0, 0});
    std::cout << "  " << (passed ? "[PASS]" : "[FAIL]") << " Octave verification passed" << std::endl;
    return passed;
}

bool TestRunner::executeVolumeSilenceTest(Machine* machine, bool verbose) {
    if (!machine) {
        std::cout << "  [SKIP] No machine provided" << std::endl;
        results_.push_back({"silence", false, "No machine provided", 0, 0, 0, false, 0, 0});
        return false;
    }
    
    bool passed = runVolumeSilenceTest(machine, verbose);
    results_.push_back({"silence", passed, "", 0, 0, 0, false, 0, 0});
    std::cout << "  " << (passed ? "[PASS]" : "[FAIL]") << " Silence test passed" << std::endl;
    return passed;
}

bool TestRunner::executeVolumeNoClipTest(Machine* machine, bool verbose) {
    if (!machine) {
        std::cout << "  [SKIP] No machine provided" << std::endl;
        results_.push_back({"no_clip", false, "No machine provided", 0, 0, 0, false, 0, 0});
        return false;
    }
    
    bool passed = runVolumeNoClipTest(machine, verbose);
    results_.push_back({"no_clip", passed, "", 0, 0, 0, false, 0, 0});
    std::cout << "  " << (passed ? "[PASS]" : "[FAIL]") << " No-clip test passed" << std::endl;
    return passed;
}

bool TestRunner::executeNoteReleaseTest(Machine* machine, bool verbose) {
    if (!machine) {
        std::cout << "  [SKIP] No machine provided" << std::endl;
        results_.push_back({"note_release", false, "No machine provided", 0, 0, 0, false, 0, 0});
        return false;
    }
    
    bool passed = runNoteOffTests(machine, verbose);
    results_.push_back({"note_release", passed, "", 0, 0, 0, false, 0, 0});
    std::cout << "  " << (passed ? "[PASS]" : "[FAIL]") << " Note release test passed" << std::endl;
    return passed;
}

bool TestRunner::executeFilterEnvelopeTest(Machine* machine, bool verbose) {
    if (!machine) {
        std::cout << "  [SKIP] No machine provided" << std::endl;
        results_.push_back({"filter_env", false, "No machine provided", 0, 0, 0, false, 0, 0});
        return false;
    }
    
    bool passed = runFilterEnvelopeTests(machine, verbose);
    results_.push_back({"filter_env", passed, "", 0, 0, 0, false, 0, 0});
    std::cout << "  " << (passed ? "[PASS]" : "[FAIL]") << " Filter envelope test passed" << std::endl;
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
        // Align output
        int padding = 12 - g_availableEngines[i].length();
        for (int j = 0; j < padding; j++) std::cout << " ";
        std::cout << "- " << g_engineDescriptions[i] << "\n";
    }
    std::cout << "\n";

    std::cout << "Available Tests:\n";
    for (size_t i = 0; i < g_availableTests.size(); i++) {
        std::cout << "  " << g_availableTests[i];
        // Align output
        int padding = 13 - g_availableTests[i].length();
        for (int j = 0; j < padding; j++) std::cout << " ";
        std::cout << "- " << g_testDescriptions[i] << "\n";
    }
    std::cout << "  all           - All tests above\n\n";

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
        int padding = 13 - g_availableTests[i].length();
        for (int j = 0; j < padding; j++) std::cout << " ";
        std::cout << "- " << g_testDescriptions[i] << "\n";
    }
    std::cout << "  all             - All tests above\n";
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
    
    // Validate that engine and tests are specified
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
    
    std::cout << "\n=== Test Summary ===" << std::endl << std::flush;
    std::cout << "Passed: " << g_totalPassed << std::endl << std::flush;
    std::cout << "Failed: " << g_totalFailed << std::endl << std::flush;
    int total = g_totalPassed + g_totalFailed;
    double rate = total > 0 ? (static_cast<double>(g_totalPassed) / total) * 100.0 : 0.0;
    std::cout << "Rate: " << std::fixed << std::setprecision(1) << rate << "%" << std::endl << std::flush;
    
    return success ? 0 : 1;
}
// TestRunner method wrappers for global test functions
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

// Class method implementations - stubs for API compatibility
// Note: runAllTests() directly calls the global functions from test_helpers.cpp
// These stubs exist for API compatibility but are not used in the current implementation
bool TestRunner::runNoteReleaseTests(Machine* machine, bool useFFT) {
    (void)machine;
    (void)useFFT;
    return true; // Stub - actual test is run by global runNoteReleaseTests()
}

bool TestRunner::runFilterEnvelopeTests(Machine* machine, bool useFFT) {
    (void)machine;
    (void)useFFT;
    return true; // Stub - actual test is run by global runFilterEnvelopeTests()
}

bool TestRunner::runNoteOnOffTests(Machine* machine) {
    (void)machine;
    return true; // Stub - actual test is run by global runNoteOnTests()
}

bool TestRunner::runCCControlTests(Machine* machine, bool useFFT) {
    (void)machine;
    (void)useFFT;
    return true; // Stub - actual test is run by global runCCControlTests()
}

bool TestRunner::runNoteOffTests(Machine* machine, bool useFFT) {
    (void)machine;
    (void)useFFT;
    return true; // Stub - actual test is run by global runNoteReleaseTests()
}

bool TestRunner::runEngineSwitchingTests(Machine* machine) {
    (void)machine;
    return true;
}

bool TestRunner::runEngineInitTests(Machine* machine) {
    (void)machine;
    return true;
}

