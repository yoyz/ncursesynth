
int main(int argc, char* argv[]) {
    bool useFFT = false;
    bool verbose = false;
    bool quiet = false;
    std::string engine = "cursynth";
    bool parallel = false;
    std::vector<std::string> tests;
    std::string reportFormat = "json,html,text,csv";
    std::string outputDir = "test_results";
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--engine" && i + 1 < argc) {
            engine = argv[++i];
        } else if (arg == "--all") {
            engine = "all";
        } else if (arg == "--parallel") {
            parallel = true;
        } else if (arg == "--tests" && i + 1 < argc) {
            std::string testStr = argv[++i];
            size_t pos = 0;
            while ((pos = testStr.find(',', pos)) != std::string::npos) {
                std::string segment = testStr.substr(0, pos);
                if (!segment.empty()) tests.push_back(segment);
                testStr.erase(0, pos + 1);
            }
            if (!testStr.empty()) tests.push_back(testStr);
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
        } else if (arg == "--help") {
            printUsage();
            return 0;
        }
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

    if (engine == "all") {
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
