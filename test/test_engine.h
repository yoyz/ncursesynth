#ifndef TEST_ENGINE_H
#define TEST_ENGINE_H

#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cmath>

class TestRunner;

class TestContext {
public:
    TestContext(TestRunner* runner, const std::string& engineName, bool verbose);
    ~TestContext();

    TestRunner* getRunner() { return runner_; }
    const std::string& getEngineName() const { return engineName_; }
    bool getVerbose() const { return verbose_; }
    TestRunner* runner_;
    std::string engineName_;
    bool verbose_;
};

class TestHelpers {
public:
    static float midiNoteToFrequency(int midiNote);
    static float frequencyToMidiNote(float frequency);
    static bool isWithinTolerance(float expected, float actual, float tolerance);
    static bool isSilent(float rms, float threshold);
    static bool noClipping(float maxSample, float threshold);
};

#endif
