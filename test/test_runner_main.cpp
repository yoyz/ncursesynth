#include <iostream>
#include <string>
#include <vector>
#include "test_runner.h"
#include "test_engine.h"
#include "../machine/Machine.h"
#include "../machine/Ncursesynth/NcursesynthMachine.h"
#include "../machine/PBSynth/PBSynthMachine.h"
#include "../machine/Cursynth/CursynthMachine.h"

void printUsage() {
    std::cout << "ncursesynth Test Runner\n\n";
    std::cout << "Usage: test_runner --engine ENGINE --tests TESTS [OPTIONS]\n";
    std::cout << "       test_runner --all-engines --all-tests [OPTIONS]\n\n";
    
    std::cout << "Examples:\n";
    std::cout << "  test_runner --engine ncursesynth --tests sound,note_on\n";
    std::cout << "  test_runner --all-engines --all-tests --fft\n";
    std::cout << "  test_runner --engine pbsynth --tests all --verbose\n\n";
    
    std::cout << "Available Engines:\n";
    std::cout << "  ncursesynth   - Original ncursesynth engine\n";
    std::cout << "  pbsynth       - PBSynth (8-voice polyphonic)\n";
    std::cout << "  cursynth      - Cursynth engine\n";
    std::cout << "  twytch        - Twytch (Helm-based) engine\n\n";
    
    std::cout << "Available Tests:\n";
    std::cout << "  sound         - Basic sound production\n";
    std::cout << "  silence       - Volume -> 0 produces silence\n";
    std::cout << "  no_clip       - Max volume doesn't clip\n";
    std::cout << "  note_on_off   - Note on/off trigger\n";
    std::cout << "  note_release  - Note release handling\n";
    std::cout << "  octave        - Octave progression tests\n";
    std::cout << "  cc_control   - CC parameter control\n";
    std::cout << "  polyphony     - Multi-voice handling\n";
    std::cout << "  filter_env   - Filter envelope tests\n";
    std::cout << "  all           - All tests above\n\n";
    
    std::cout << "Options:\n";
    std::cout << "  -h, --help           Show this help\n";
    std::cout << "  --engine NAME        Specify engine (required unless --all-engines)\n";
    std::cout << "  --all-engines        Run tests on all engines\n";
    std::cout << "  --tests LIST         Specify tests (comma-separated, required unless --all-tests)\n";
    std::cout << "  --all-tests          Run all available tests\n";
    std::cout << "  --parallel           Run engine tests in parallel\n";
    std::cout << "  --verbose            Verbose output\n";
    std::cout << "  --quiet              Minimal output\n";
    std::cout << "  --fft                Enable FFT analysis\n";
    std::cout << "  --report FORMAT      Report format: json,html,text,csv (default: all)\n";
    std::cout << "  --output DIR         Output directory for reports (default: test_results)\n";
}

int main(int argc, char* argv[]) {
    bool useFFT = false;
    bool verbose = false;
    bool quiet = false;
    bool runAllEngines = false;
    bool runAllTests = false;
    std::string engine;
    std::vector<std::string> tests;
    std::string reportFormat = "text";
    std::string outputDir = "test_results";
    
    if (argc == 1) {
        printUsage();
        return 0;
    }
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            printUsage();
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
        } else if (arg == "--parallel") {
            // Not implemented yet
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg == "--quiet") {
            quiet = true;
        } else if (arg == "--report" && i + 1 < argc) {
            reportFormat = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            outputDir = argv[++i];
        } else if (arg == "--fft") {
            useFFT = true;
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
    
    MachineManager machineManager;
    machineManager.registerMachine(new NcursesynthMachine());
    machineManager.registerMachine(new PBSynthMachine());
    machineManager.registerMachine(new CursynthMachine(8));

    TestRunner runner(outputDir);
    runner.setVerbose(verbose);
    runner.setQuiet(quiet);

    // Build environment as vector of pairs
    std::vector<std::pair<std::string, std::string>> env;
    env.push_back(std::make_pair("engines", "ncursesynth,pbsynth,cursynth,twytch"));
    env.push_back(std::make_pair("test_framework", "ncursesynth-test"));
    env.push_back(std::make_pair("report_format", reportFormat));
    env.push_back(std::make_pair("verbose", verbose ? "true" : "false"));
    env.push_back(std::make_pair("quiet", quiet ? "true" : "false"));
    env.push_back(std::make_pair("fft_enabled", useFFT ? "true" : "false"));
    runner.setEnvironment(env);

    bool success = true;
    
    if (runAllEngines) {
        for (const auto& engName : runner.getEngineNames()) {
            std::cout << "\n=== Running tests for " << engName << " ===" << std::endl;
            runner.runSingleEngine(engName, tests, useFFT);
        }
    } else {
        std::cout << "\n=== Running tests for " << engine << " ===" << std::endl;
        runner.runSingleEngine(engine, tests, useFFT);
    }

    std::cout << "\n\n=== Test Summary ===" << std::endl;
    std::cout << "Passed: " << runner.getPassedCount() << std::endl;
    std::cout << "Total: " << runner.getPassedCount() << std::endl;

    return runner.getPassedCount() == 1 ? 0 : 1;
}