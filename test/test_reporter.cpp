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
    // Start time
    (void)name;
}

void TestReporter::endTest(const std::string& name, bool passed, bool fft) {
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
    (void)passedTests;
    (void)failedTests;
}

void TestReporter::printReport(const std::vector<std::string>& passedTests,
                               const std::vector<std::string>& failedTests,
                               const std::vector<std::string>& allTests) {
    (void)passedTests;
    (void)failedTests;
    (void)allTests;
}

void TestReporter::printReport(const std::vector<std::string>& passedTests,
                               const std::vector<std::string>& failedTests,
                               const std::vector<std::string>& allTests,
                               const std::vector<std::string>& fftTests) {
    (void)passedTests;
    (void)failedTests;
    (void)allTests;
    (void)fftTests;
}

double TestReporter::getAverageDuration() const {
    (void)0;
    return 0.0;
}
