#ifndef TEST_REPORTER_H
#define TEST_REPORTER_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

struct TestConfig {
    std::string framework;
    std::string report_format;
    bool verbose;
    bool quiet;
    bool fft_enabled;
    std::vector<std::string> engines;
};

class TestReporter {
public:
    TestReporter(const std::string& outputDir = "test_results");
    ~TestReporter();

    void setTestConfiguration(const std::vector<std::pair<std::string, std::string>>& config);
    void setEnvironmentInfo(const std::vector<std::pair<std::string, std::string>>& env);
    
    void startTest(const std::string& name);
    void endTest(const std::string& name, bool passed, bool fft);
    void endTest(const std::string& name, bool passed);
    
    void printReport(const std::vector<std::string>& passedTests,
                     const std::vector<std::string>& failedTests);
    void printReport(const std::vector<std::string>& passedTests,
                     const std::vector<std::string>& failedTests,
                     const std::vector<std::string>& allTests);
    void printReport(const std::vector<std::string>& passedTests,
                     const std::vector<std::string>& failedTests,
                     const std::vector<std::string>& allTests,
                     const std::vector<std::string>& fftTests);
    
    double getAverageDuration() const;

private:
    std::vector<std::pair<std::string, std::string>> config_;
    std::vector<std::pair<std::string, std::string>> environment_;
    std::string outputDir_;
    std::vector<std::string> passedTests_;
    std::vector<std::string> failedTests_;
};

#endif
