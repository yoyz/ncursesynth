#include "test_reporter.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <string>
#include <ctime>

TestReporter::TestReporter(const std::string& outputDir)
    : outputDir_(outputDir), passedTests_(), failedTests_() {}

TestReporter::~TestReporter() {}

void TestReporter::setTestConfiguration(const std::vector<std::pair<std::string, std::string>>& config) {
    config_ = config;
}

void TestReporter::setEnvironmentInfo(const std::vector<std::pair<std::string, std::string>>& env) {
    environment_ = env;
}

void TestReporter::startTest(const std::string& name) {
    (void)name;
}

void TestReporter::endTest(const std::string& name, bool passed, bool fft) {
    (void)fft;
    if (passed) {
        passedTests_.push_back(name);
    } else {
        failedTests_.push_back(name);
    }
}

void TestReporter::endTest(const std::string& name, bool passed) {
    if (passed) {
        passedTests_.push_back(name);
    } else {
        failedTests_.push_back(name);
    }
}

void TestReporter::printReport(const std::vector<std::string>& passedTests,
                               const std::vector<std::string>& failedTests) {
    int total = passedTests.size() + failedTests.size();
    std::cout << "\n=== Test Report ===" << std::endl;
    std::cout << "Total: " << total << std::endl;
    std::cout << "Passed: " << passedTests.size() << std::endl;
    std::cout << "Failed: " << failedTests.size() << std::endl;
    if (total > 0) {
        std::cout << "Rate: " << std::fixed << std::setprecision(1)
                  << (100.0 * passedTests.size() / total) << "%" << std::endl;
    }
    if (!failedTests.empty()) {
        std::cout << "\nFailed tests:" << std::endl;
        for (const auto& t : failedTests) {
            std::cout << "  - " << t << std::endl;
        }
    }
}

void TestReporter::printReport(const std::vector<std::string>& passedTests,
                               const std::vector<std::string>& failedTests,
                               const std::vector<std::string>& allTests) {
    printReport(passedTests, failedTests);
    std::cout << "\nAll tests run:" << std::endl;
    for (const auto& t : allTests) {
        std::cout << "  - " << t << std::endl;
    }
}

void TestReporter::printReport(const std::vector<std::string>& passedTests,
                               const std::vector<std::string>& failedTests,
                               const std::vector<std::string>& allTests,
                               const std::vector<std::string>& fftTests) {
    printReport(passedTests, failedTests, allTests);
    if (!fftTests.empty()) {
        std::cout << "\nFFT-analyzed tests:" << std::endl;
        for (const auto& t : fftTests) {
            std::cout << "  - " << t << std::endl;
        }
    }
}

double TestReporter::getAverageDuration() const {
    return 0.0;
}
