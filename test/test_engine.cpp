#include "test_engine.h"
#include "fft_analyzer.h"

TestContext::TestContext(TestRunner* runner, const std::string& engineName, bool verbose)
    : runner_(runner), engineName_(engineName), verbose_(verbose) {}

TestContext::~TestContext() {}

float TestHelpers::midiNoteToFrequency(int midiNote) {
    return FFTAnalyzer::midiToFrequency(midiNote);
}

float TestHelpers::frequencyToMidiNote(float frequency) {
    return FFTAnalyzer::frequencyToMidi(frequency);
}

bool TestHelpers::isWithinTolerance(float expected, float actual, float tolerance) {
    float diff = std::abs(expected - actual);
    float relDiff = (expected != 0) ? diff / std::abs(expected) : diff;
    return relDiff <= tolerance;
}

bool TestHelpers::isSilent(float rms, float threshold) {
    return rms < threshold;
}

bool TestHelpers::noClipping(float maxSample, float threshold) {
    return std::abs(maxSample) < threshold;
}
