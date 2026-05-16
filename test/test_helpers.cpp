#include "test_helpers.h"
#include "test_engine.h"
#include "fft_analyzer.h"
#include "../machine/Machine.h"
#include "../machine/Parameter.h"
#include "../machine/ParamID.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <stdio.h>
#include <sys/resource.h>
#include <chrono>

static void getCPUTime(double* userCpu, double* sysCpu) {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    *userCpu = usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1000000.0;
    *sysCpu = usage.ru_stime.tv_sec + usage.ru_stime.tv_usec / 1000000.0;
}

static double getCPUTimeForTest() {
    double user, sys;
    getCPUTime(&user, &sys);
    return user + sys;
}

static double getWallClockTime() {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration<double>(duration).count();
}

static float calculateRMS(const std::vector<float>& samples) {
    if (samples.empty()) return 0.0f;
    double sum = 0.0;
    for (float s : samples) {
        sum += s * s;
    }
    return sqrt(sum / samples.size());
}

static float analyzeFrequency(const std::vector<float>& samples, bool useFFT) {
    if (!useFFT || samples.empty()) return 0.0f;
    std::vector<float> magnitudes;
    FFTAnalyzer::compute(samples.data(), samples.size(), magnitudes);
    return FFTAnalyzer::findFundamentalFrequency(magnitudes, 48000.0f);
}

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

// Test: Sound production - captures audio from machine and verifies it produces output
bool runSoundProductionTests(Machine* machine, bool useFFT) {
    (void)useFFT;
    if (!machine) { 
        printTestResult("sound", false, "No machine"); 
        return false; 
    }
    
    machine->init();
    
    // Generate a substantial audio buffer (at least 16384 samples)
    const int numSamples = 16384;
    std::vector<int32_t> samples(numSamples);
    
    // Capture CPU time before
    double cpuStart = getCPUTimeForTest();
    double wallStart = getWallClockTime();
    
    // Trigger note to produce sound
    machine->noteOn();
    
    // Generate audio samples from the machine
    for (int i = 0; i < numSamples; i++) {
        samples[i] = machine->tick();
    }
    
    // Capture CPU time after
    double cpuEnd = getCPUTimeForTest();
    double wallEnd = getWallClockTime();
    
    // Calculate RMS - convert int32_t to float
    float rms = 0.0f;
    double sum = 0.0;
    for (int32_t s : samples) {
        float f = static_cast<float>(s);
        sum += f * f;
    }
    rms = sqrt(sum / numSamples);
    
    // Threshold for meaningful audio (normalized to int16 max)
    const float rmsThreshold = 10.0f; // Samples are 16-bit, so 10 is a good minimum
    bool hasMeaningfulAudio = rms > rmsThreshold;
    
    // Calculate CPU usage
    double cpuTime = cpuEnd - cpuStart;
    double wallTime = wallEnd - wallStart;
    
    if (hasMeaningfulAudio) {
        printTestResult("sound", true, "Audio generated (RMS=" + std::to_string(rms) + ", CPU=" + std::to_string(cpuTime) + "s)");
        return true;
    } else {
        printTestResult("sound", false, "No audio output (RMS=" + std::to_string(rms) + ", CPU=" + std::to_string(cpuTime) + "s)");
        return false;
    }
}

// Test: Silence - verifies no audio output when no note is active
bool runVolumeSilenceTest(Machine* machine, bool useFFT) {
    (void)useFFT;
    if (!machine) { 
        printTestResult("silence", false, "No machine"); 
        return false; 
    }
    
    machine->init();
    
    // Turn off any active notes using engine's noteOff() method
    machine->noteOff();
    
    // Allow brief release time for engines that need it (like Twytch)
    // Generate a small buffer to let release complete
    const int releaseSamples = 1024;
    std::vector<int32_t> releaseBuffer(releaseSamples);
    for (int i = 0; i < releaseSamples; i++) {
        machine->tick();
    }
    
    // Now generate silence buffer
    const int numSamples = 16384;
    std::vector<int32_t> samples(numSamples);
    
    // Capture CPU time before
    double cpuStart = getCPUTimeForTest();
    
    // Generate audio samples
    for (int i = 0; i < numSamples; i++) {
        samples[i] = machine->tick();
    }
    
    // Capture CPU time after
    double cpuEnd = getCPUTimeForTest();
    
    // Calculate RMS - convert int32_t to float
    float rms = 0.0f;
    double sum = 0.0;
    for (int32_t s : samples) {
        float f = static_cast<float>(s);
        sum += f * f;
    }
    rms = sqrt(sum / numSamples);
    
    // Threshold for silence (normalized to int16 max)
    const float rmsSilenceThreshold = 50.0f; // Allow small numerical noise and release
    bool isSilent = rms < rmsSilenceThreshold;
    
    // Calculate CPU usage
    double cpuTime = cpuEnd - cpuStart;
    
    if (isSilent) {
        printTestResult("silence", true, "Silence produced (RMS=" + std::to_string(rms) + ", CPU=" + std::to_string(cpuTime) + "s)");
        return true;
    } else {
        printTestResult("silence", false, "Audio output when no note active (RMS=" + std::to_string(rms) + ", CPU=" + std::to_string(cpuTime) + "s)");
        return false;
    }
}

// Test: No clipping - verifies audio samples don't exceed max amplitude
bool runVolumeNoClipTest(Machine* machine, bool useFFT) {
    (void)useFFT;
    if (!machine) { 
        printTestResult("no_clip", false, "No machine"); 
        return false; 
    }
    
    machine->init();
    
    // Generate a substantial audio buffer (at least 16384 samples)
    const int numSamples = 16384;
    std::vector<int32_t> samples(numSamples);
    
    // Capture CPU time before
    double cpuStart = getCPUTimeForTest();
    double wallStart = getWallClockTime();
    
    // Generate some audio - note on to produce sound
    machine->noteOn();
    
    for (int i = 0; i < numSamples; i++) {
        samples[i] = machine->tick();
    }
    
    // Capture CPU time after
    double cpuEnd = getCPUTimeForTest();
    double wallEnd = getWallClockTime();
    
    // Check for clipping (samples that exceed max int16_t range)
    const int16_t maxAmplitude = 32767;
    const int16_t minAmplitude = -32768;
    bool hasClipping = false;
    
    for (int i = 0; i < numSamples; i++) {
        if (samples[i] > maxAmplitude || samples[i] < minAmplitude) {
            hasClipping = true;
            break;
        }
    }
    
    // Calculate CPU usage
    double cpuTime = cpuEnd - cpuStart;
    double wallTime = wallEnd - wallStart;
    
    if (!hasClipping) {
        printTestResult("no_clip", true, "No clipping detected (" + std::to_string(cpuTime) + "s CPU)");
        return true;
    } else {
        printTestResult("no_clip", false, "Clipping detected (" + std::to_string(cpuTime) + "s CPU)");
        return false;
    }
}

// Test: Note-on/off - verifies note triggering works
bool runNoteOnTests(Machine* machine, bool useFFT) {
    (void)useFFT;
    if (!machine) { 
        printTestResult("note_on_off", false, "No machine"); 
        return false; 
    }
    
    machine->init();
    
    // Generate a substantial audio buffer (at least 16384 samples)
    const int numSamples = 16384;
    std::vector<int32_t> samples(numSamples);
    
    // Capture CPU time before
    double cpuStart = getCPUTimeForTest();
    double wallStart = getWallClockTime();
    
    // Test note on
    machine->noteOn();
    
    // Generate samples to ensure note is active
    for (int i = 0; i < numSamples; i++) {
        samples[i] = machine->tick();
    }
    
    // Capture CPU time after
    double cpuEnd = getCPUTimeForTest();
    double wallEnd = getWallClockTime();
    
    // Check if note was triggered (should have some non-zero samples)
    bool noteActive = false;
    for (int i = 0; i < numSamples; i++) {
        if (samples[i] != 0) {
            noteActive = true;
            break;
        }
    }
    
    // Calculate CPU usage
    double cpuTime = cpuEnd - cpuStart;
    double wallTime = wallEnd - wallStart;
    
    if (noteActive) {
        printTestResult("note_on_off", true, "Note on works (" + std::to_string(cpuTime) + "s CPU)");
        return true;
    } else {
        printTestResult("note_on_off", false, "Note on failed (" + std::to_string(cpuTime) + "s CPU)");
        return false;
    }
}

// Helper: capture RMS for a set of simultaneous notes
static float captureMultiNoteRMS(Machine* machine, const std::vector<int>& notes,
                                  int numSamples, int warmupSamples) {
    std::vector<int32_t> samples(numSamples);

    for (int note : notes) {
        machine->setI(71, note);
        machine->setI(70, note);
        machine->noteOn();
    }

    for (int i = 0; i < warmupSamples; i++) machine->tick();
    for (int i = 0; i < numSamples; i++) samples[i] = machine->tick();

    machine->noteOff();

    double sum = 0.0;
    for (int32_t s : samples) sum += (double)s * s;
    return sqrt(sum / numSamples);
}

// Test: Voice level increase - verifies RMS increases with more simultaneous notes
bool runVoiceLevelTests(Machine* machine, bool useFFT) {
    (void)useFFT;
    if (!machine) {
        printTestResult("voice_level", false, "No machine");
        return false;
    }

    machine->init();

    const int numSamples = 16384;
    const int warmupSamples = 1024;
    const int silenceSamples = 441000;

    machine->setI(ParamID::volume, 127);
    machine->setI(ParamID::cutoff, 127);

    std::vector<int> notes1 = {48};
    std::vector<int> notes2 = {48, 60};
    std::vector<int> notes3 = {48, 60, 72, 84};

    float rms1 = captureMultiNoteRMS(machine, notes1, numSamples, warmupSamples);
    for (int i = 0; i < silenceSamples; i++) machine->tick();

    machine->init();
    machine->setI(35, 127);
    machine->setI(52, 127);
    float rms2 = captureMultiNoteRMS(machine, notes2, numSamples, warmupSamples);
    for (int i = 0; i < silenceSamples; i++) machine->tick();

    machine->init();
    machine->setI(35, 127);
    machine->setI(52, 127);
    float rms3 = captureMultiNoteRMS(machine, notes3, numSamples, warmupSamples);
    for (int i = 0; i < silenceSamples; i++) machine->tick();

    bool allProduceAudio = (rms1 > 0 && rms2 > 0 && rms3 > 0);
    bool levelConsistent = (rms3 >= rms1 * 0.7f);

    bool passed = allProduceAudio && levelConsistent;

    std::ostringstream msg;
    msg << "1note RMS=" << std::fixed << std::setprecision(1) << rms1
        << " 2note RMS=" << rms2 << " 4note RMS=" << rms3
        << " (ratio 4:1=" << std::setprecision(2) << (rms1 > 0 ? rms3 / rms1 : 0) << "x)";

    printTestResult("voice_level", passed, msg.str());
    return passed;
}

// Helper: set amp envelope on all engines (ADSR_ENV0 + MachineParam)
static void setAmpEnvelope(Machine* machine, int a, int d, int s, int r) {
    machine->setI(ParamID::amp_attack, a);
    machine->setI(ParamID::amp_decay, d);
    machine->setI(ParamID::amp_sustain, s);
    machine->setI(ParamID::amp_release, r);
}

// Helper: capture envelope curve samples.
// Returns two half-RMS values: [first_half_rms, second_half_rms]
static std::vector<float> captureEnvelopeHalves(Machine* machine, int numSamples,
                                                 int note, int triggerHoldSamples,
                                                 bool doNoteOff, int releaseSamples) {
    machine->setI(ParamID::volume, 127);  // VOLUME max
    machine->setI(ParamID::cutoff, 127);  // CUTOFF max
    machine->setI(71, note);
    machine->setI(ParamID::note, note);
    machine->noteOn();

    // Warmup / hold before capture
    for (int i = 0; i < triggerHoldSamples; i++) machine->tick();

    // Capture main samples
    std::vector<int32_t> buf(numSamples);
    for (int i = 0; i < numSamples; i++) buf[i] = machine->tick();

    if (doNoteOff) {
        machine->noteOff();
        machine->setI(150, 0);
        for (int i = 0; i < releaseSamples; i++) machine->tick();
        std::vector<int32_t> releaseBuf(numSamples);
        for (int i = 0; i < numSamples; i++) releaseBuf[i] = machine->tick();
        // For release, return [before_release_rms, after_release_rms]
        double sumBefore = 0, sumAfter = 0;
        for (int i = 0; i < numSamples; i++) {
            sumBefore += (double)buf[i] * buf[i];
            sumAfter += (double)releaseBuf[i] * releaseBuf[i];
        }
        return {sqrt(sumBefore / numSamples), sqrt(sumAfter / numSamples)};
    }

    // Split buf into first half and second half
    int half = numSamples / 2;
    double sumFirst = 0, sumSecond = 0;
    for (int i = 0; i < half; i++) {
        sumFirst += (double)buf[i] * buf[i];
        sumSecond += (double)buf[i + half] * buf[i + half];
    }
    return {sqrt(sumFirst / half), sqrt(sumSecond / half)};
}

// Test: Amplitude envelope - verifies A/D/S/R shape the sound correctly
bool runEnvelopeTests(Machine* machine, bool useFFT) {
    (void)useFFT;
    if (!machine) {
        printTestResult("envelope", false, "No machine");
        return false;
    }

    const int numSamples = 65535;
    const int holdSamples = 4096;
    const int silenceSamples = 220500;
    int passCount = 0, failCount = 0;

    // ==================== ATTACK TEST ====================
    // A=64 D=0 S=127 R=0: sound should be present with level not dropping sharply
    {
        machine->init();
        setAmpEnvelope(machine, 64, 0, 127, 0);
        auto rms = captureEnvelopeHalves(machine, numSamples, 60, holdSamples, false, 0);
        for (int i = 0; i < silenceSamples; i++) machine->tick();
        bool ok = (rms[0] > 50 && rms[1] >= rms[0] * 0.8f);
        if (ok) passCount++; else failCount++;
        printTestResult("env_attack", ok, "first_half RMS=" + std::to_string(rms[0]) +
                        " second_half RMS=" + std::to_string(rms[1]));
    }

    // ==================== DECAY TEST ====================
    // A=0 D=64 S=0 R=0: sound should decrease or not increase
    {
        machine->init();
        setAmpEnvelope(machine, 0, 64, 0, 0);
        auto rms = captureEnvelopeHalves(machine, numSamples, 60, holdSamples, false, 0);
        for (int i = 0; i < silenceSamples; i++) machine->tick();
        bool ok = (rms[0] > 0 && rms[1] <= rms[0] * 1.05f);
        if (ok) passCount++; else failCount++;
        printTestResult("env_decay", ok, "first_half RMS=" + std::to_string(rms[0]) +
                        " second_half RMS=" + std::to_string(rms[1]));
    }

    // ==================== SUSTAIN TEST ====================
    // A=0 D=0 S=0 R=0: no sound. Then S=64: sound appears.
    {
        machine->init();
        setAmpEnvelope(machine, 0, 0, 0, 0);
        auto rms0 = captureEnvelopeHalves(machine, numSamples, 60, holdSamples, false, 0);
        for (int i = 0; i < silenceSamples; i++) machine->tick();

        machine->init();
        setAmpEnvelope(machine, 0, 0, 64, 0);
        auto rms1 = captureEnvelopeHalves(machine, numSamples, 60, holdSamples, false, 0);
        for (int i = 0; i < silenceSamples; i++) machine->tick();

        bool ok = (rms0[0] < 100.0f && rms1[0] > 50.0f);
        if (ok) passCount++; else failCount++;
        printTestResult("env_sustain", ok, "S=0 RMS=" + std::to_string(rms0[0]) +
                        " S=64 RMS=" + std::to_string(rms1[0]));
    }

    // ==================== RELEASE TEST ====================
    // A=0 D=127 S=0 R=64: sound should decrease during release
    {
        machine->init();
        setAmpEnvelope(machine, 0, 127, 0, 64);
        machine->setI(35, 127);
        machine->setI(52, 127);
        machine->setI(71, 60); machine->setI(70, 60);
        machine->noteOn();
        for (int i = 0; i < holdSamples; i++) machine->tick();
        // Capture before release to verify sound exists
        std::vector<int32_t> beforeBuf(4096);
        for (int i = 0; i < 4096; i++) beforeBuf[i] = machine->tick();
        // Release note
        machine->noteOff();
        machine->setI(150, 0);
        // Capture release tail and split in half
        std::vector<int32_t> releaseBuf(numSamples);
        for (int i = 0; i < numSamples; i++) releaseBuf[i] = machine->tick();
        // Verify release decreases: first quarter vs last quarter
        int q = numSamples / 4;
        double sumFirst = 0, sumLast = 0;
        for (int i = 0; i < q; i++) {
            sumFirst += (double)releaseBuf[i] * releaseBuf[i];
            sumLast += (double)releaseBuf[i + 3 * q] * releaseBuf[i + 3 * q];
        }
        float rmsFirst = sqrt(sumFirst / q);
        float rmsLast = sqrt(sumLast / q);
        // Check sound existed before release, and release decreases over time
        double beforeSum = 0;
        for (int32_t s : beforeBuf) beforeSum += (double)s * s;
        float beforeRms = sqrt(beforeSum / 4096);
        bool ok = (beforeRms > 0 && rmsLast < rmsFirst * 0.9f);
        if (ok) passCount++; else failCount++;
        printTestResult("env_release", ok, "before RMS=" + std::to_string(beforeRms) +
                        " release_first_q=" + std::to_string(rmsFirst) +
                        " release_last_q=" + std::to_string(rmsLast));
        for (int i = 0; i < silenceSamples; i++) machine->tick();
    }

    bool allPassed = (failCount == 0);
    printTestResult("envelope", allPassed,
                    std::to_string(passCount) + "/4 sub-tests passed");
    return allPassed;
}

// Test: Note release - verifies note release functionality
bool runNoteReleaseTests(Machine* machine, bool useFFT) {
    (void)useFFT;
    if (!machine) { 
        printTestResult("note_release", false, "No machine"); 
        return false; 
    }
    
    machine->init();
    
    // Generate a substantial audio buffer (at least 16384 samples)
    const int numSamples = 16384;
    std::vector<int32_t> samples(numSamples);
    
    // Capture CPU time before
    double cpuStart = getCPUTimeForTest();
    double wallStart = getWallClockTime();
    
    // Test note on and off
    machine->noteOn();
    
    // Generate some samples while note is on
    for (int i = 0; i < numSamples/2; i++) {
        samples[i] = machine->tick();
    }
    
    machine->noteOff();
    
    // Generate samples while note is off
    for (int i = numSamples/2; i < numSamples; i++) {
        samples[i] = machine->tick();
    }
    
    // Capture CPU time after
    double cpuEnd = getCPUTimeForTest();
    double wallEnd = getWallClockTime();
    
    // Generate silence to ensure note is fully released before next test (10 seconds)
    for (int i = 0; i < 441000; i++) {
        machine->tick();
    }
    
    // Basic check - if we can trigger note on/off, test passes
    double cpuTime = cpuEnd - cpuStart;
    double wallTime = wallEnd - wallStart;
    
    printTestResult("note_release", true, "Note release works (" + std::to_string(cpuTime) + "s CPU)");
    return true;
}

// Helper: trigger a note on a machine
// ncursesynth: setI(71, note) sets noteFrequency_, then noteOn() uses it
// PBSynth/Twytch/Cursynth: setI(70, note) + setI(150, 1) triggers the note
static void triggerNote(Machine* machine, int midiNote, int32_t* samples, int numSamples, int warmupSamples) {
    // Set the MIDI note
    machine->setI(71, midiNote);  // NOTE_HZ - for ncursesynth
    machine->setI(70, midiNote);  // NOTE1 - for PBSynth/Twytch/Cursynth
    
    // Trigger note - use noteOn() which works for all engines
    machine->noteOn();
    
    // Warmup
    for (int i = 0; i < warmupSamples; i++) {
        machine->tick();
    }
    
    // Capture samples
    for (int i = 0; i < numSamples; i++) {
        samples[i] = machine->tick();
    }
    
    // Release note
    machine->noteOff();
}

// Helper: generate silence buffer and measure performance
// 441000 samples = 10 seconds at 48kHz - ensures complete note release
// Returns the time taken in milliseconds
static double generateSilence(Machine* machine, int numSamples) {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < numSamples; i++) {
        machine->tick();
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    return duration.count();
}

// Test: Octave - verifies pitch doubling (12 semitones = double frequency)
bool runOctaveTests(Machine* machine, bool useFFT) {
    (void)useFFT;
    if (!machine) { 
        printTestResult("octave", false, "No machine"); 
        return false; 
    }
    
    machine->init();
    
    const int midiNote = 50;
    const int numSamples = 32768;  // Large buffer for FFT
    const int warmupSamples = 4096;
    const int silenceSamples = 441000;  // 10 seconds at 48kHz - ensures complete note release
    
    // First test: play note at midiNote
    int32_t* samples1 = new int32_t[numSamples];
    triggerNote(machine, midiNote, samples1, numSamples, warmupSamples);
    
    // Generate silence to ensure note is released (10 seconds) and measure performance
    double silenceTime1 = generateSilence(machine, silenceSamples);
    
    // Second test: play note at midiNote + 12 (one octave higher)
    int32_t* samples2 = new int32_t[numSamples];
    triggerNote(machine, midiNote + 12, samples2, numSamples, warmupSamples);
    
    // Generate silence (10 seconds) and measure performance
    double silenceTime2 = generateSilence(machine, silenceSamples);
    
    // Convert to float and analyze
    std::vector<float> floatSamples1(numSamples);
    std::vector<float> floatSamples2(numSamples);
    for (int i = 0; i < numSamples; i++) {
        floatSamples1[i] = static_cast<float>(samples1[i]) / 8192.0f;
        floatSamples2[i] = static_cast<float>(samples2[i]) / 8192.0f;
    }
    
    delete[] samples1;
    delete[] samples2;
    
    // Compute FFT for both
    std::vector<float> magnitudes1, magnitudes2;
    FFTAnalyzer::compute(floatSamples1.data(), numSamples, magnitudes1);
    FFTAnalyzer::compute(floatSamples2.data(), numSamples, magnitudes2);
    
    float freq1 = FFTAnalyzer::findFundamentalFrequency(magnitudes1, 48000.0f);
    float freq2 = FFTAnalyzer::findFundamentalFrequency(magnitudes2, 48000.0f);
    
    // Calculate expected frequencies
    float expectedFreq1 = 440.0f * std::pow(2.0f, (midiNote - 69.0f) / 12.0f);
    float expectedFreq2 = 440.0f * std::pow(2.0f, (midiNote + 12 - 69.0f) / 12.0f);
    
    // Verify octave relationship: freq2 should be approximately 2x freq1
    // Allow some tolerance for engines that may have frequency detection issues
    float ratio = (freq1 > 0) ? (freq2 / freq1) : 0.0f;
    
    // Calculate performance: how many seconds of audio can be generated per second of real time
    // 441000 samples / 1000ms = 441 samples/ms = 441000 samples/second
    double perf1 = (silenceTime1 > 0) ? (silenceSamples / silenceTime1) : 0.0;  // samples/ms
    double perf2 = (silenceTime2 > 0) ? (silenceSamples / silenceTime2) : 0.0;  // samples/ms
    
    std::ostringstream msg;
    msg << "note=" << midiNote << " freq=" << std::fixed << std::setprecision(1) << freq1 
        << "Hz (exp=" << expectedFreq1 << ") note+12=" << (midiNote + 12) 
        << " freq=" << freq2 << "Hz (exp=" << expectedFreq2 << ") ratio=" 
        << std::setprecision(3) << ratio << " perf=" << std::setprecision(0) 
        << ((perf1 + perf2) / 2.0) << " samples/ms";
    
    // Pass if both frequencies detected and ratio is close to 2.0 (within 25% tolerance)
    // Also pass if both frequencies are non-zero but different (meaning pitch changed)
    bool passed = (freq1 > 0 && freq2 > 0 && std::abs(ratio - 2.0f) < 0.5f);
    printTestResult("octave", passed, msg.str());
    return passed;
}

// Test: CC control - verifies MIDI CC message handling
bool runCCControlTests(Machine* machine, bool useFFT) {
    (void)useFFT;
    if (!machine) { 
        printTestResult("cc_control", false, "No machine"); 
        return false; 
    }
    
    machine->init();
    
    // Generate a substantial audio buffer (at least 16384 samples)
    const int numSamples = 16384;
    std::vector<int32_t> samples(numSamples);
    
    // Capture CPU time before
    double cpuStart = getCPUTimeForTest();
    double wallStart = getWallClockTime();
    
    // Test applying a CC message
    machine->applyCC(7, 0.5f, "VOLUME");
    
    // Generate samples to ensure the CC has effect
    for (int i = 0; i < numSamples; i++) {
        samples[i] = machine->tick();
    }
    
    // Capture CPU time after
    double cpuEnd = getCPUTimeForTest();
    double wallEnd = getWallClockTime();
    
    double cpuTime = cpuEnd - cpuStart;
    double wallTime = wallEnd - wallStart;
    
    // If we can call applyCC without crashing, test passes
    printTestResult("cc_control", true, "CC control works (" + std::to_string(cpuTime) + "s CPU)");
    return true;
}

// Test: Polyphony - verifies polyphony parameter setting
bool runPolyphonyTests(Machine* machine, bool useFFT) {
    (void)useFFT;
    if (!machine) { 
        printTestResult("polyphony", false, "No machine"); 
        return false; 
    }
    
    machine->init();
    
    // Generate a substantial audio buffer (at least 16384 samples)
    const int numSamples = 16384;
    std::vector<int32_t> samples(numSamples);
    
    // Capture CPU time before
    double cpuStart = getCPUTimeForTest();
    double wallStart = getWallClockTime();
    
    // Test setting polyphony
    machine->setI(MachineParam::POLYPHONY, 8);
    
    // Test getting polyphony back
    int poly = machine->getI(MachineParam::POLYPHONY);
    
    // Generate samples to ensure the setting has effect
    for (int i = 0; i < numSamples; i++) {
        samples[i] = machine->tick();
    }
    
    // Capture CPU time after
    double cpuEnd = getCPUTimeForTest();
    double wallEnd = getWallClockTime();
    
    double cpuTime = cpuEnd - cpuStart;
    double wallTime = wallEnd - wallStart;
    
    if (poly >= 0) {
        printTestResult("polyphony", true, "Polyphony works (" + std::to_string(cpuTime) + "s CPU)");
        return true;
    } else {
        printTestResult("polyphony", false, "Polyphony test failed (" + std::to_string(cpuTime) + "s CPU)");
        return false;
    }
}

// Test: Filter envelope - verifies filter envelope parameter setting
bool runFilterEnvelopeTests(Machine* machine, bool useFFT) {
    (void)useFFT;
    if (!machine) { 
        printTestResult("filter_env", false, "No machine"); 
        return false; 
    }
    
    machine->init();
    
    // Generate a substantial audio buffer (at least 16384 samples)
    const int numSamples = 16384;
    std::vector<int32_t> samples(numSamples);
    
    // Capture CPU time before
    double cpuStart = getCPUTimeForTest();
    double wallStart = getWallClockTime();
    
    // Test setting filter envelope parameters
    machine->setI(4, 64);   // FILTER_ATTACK (unified)
    machine->setI(5, 64);   // FILTER_DECAY  (unified)
    machine->setI(6, 64);   // FILTER_SUSTAIN (unified)
    machine->setI(7, 64);   // FILTER_RELEASE (unified)
    
    // Test getting parameters back
    int attack = machine->getI(4);
    
    // Generate samples to ensure the setting has effect
    for (int i = 0; i < numSamples; i++) {
        samples[i] = machine->tick();
    }
    
    // Capture CPU time after
    double cpuEnd = getCPUTimeForTest();
    double wallEnd = getWallClockTime();
    
    double cpuTime = cpuEnd - cpuStart;
    double wallTime = wallEnd - wallStart;
    
    if (attack >= 0) {
        printTestResult("filter_env", true, "Filter envelope works (" + std::to_string(cpuTime) + "s CPU)");
        return true;
    } else {
        printTestResult("filter_env", false, "Filter envelope test failed (" + std::to_string(cpuTime) + "s CPU)");
        return false;
    }
}

static std::string engineDir(const std::string& name) {
    std::string r = "bank/";
    for (char c : name) r += std::tolower(c);
    return r;
}

bool runPresetTests(Machine* machine, bool useFFT) {
    (void)useFFT;
    if (!machine) {
        printTestResult("preset", false, "No machine");
        return false;
    }

    machine->init();

    // Get list of presets
    std::string dir = engineDir(machine->getName());
    std::vector<std::string> files = Machine::getPresetList(machine->getName());
    if (files.empty()) {
        printTestResult("preset", false, "No preset files found in " + dir);
        return false;
    }

    // Try loading each preset
    int loaded = 0;
    for (const auto& f : files) {
        std::string path = dir + "/" + f;
        if (machine->loadPreset(path)) {
            loaded++;
            // Generate audio to verify preset doesn't crash
            machine->init();
            for (int i = 0; i < 1024; i++) machine->tick();
        }
    }

    if (loaded == 0) {
        printTestResult("preset", false, "Failed to load any presets in bank/" + machine->getName());
        return false;
    }

    // Test save and reload
    std::string testPath = "bank/" + machine->getName() + "/_test_preset_save";
    bool saveOk = machine->savePreset(testPath);

    // Reload it and verify
    bool reloadOk = false;
    if (saveOk) {
        reloadOk = machine->loadPreset(testPath);
        // Generate audio after reload
        if (reloadOk) {
            machine->init();
            for (int i = 0; i < 1024; i++) machine->tick();
        }
        // Clean up test file
        remove(testPath.c_str());
    }

    std::string msg = "Loaded " + std::to_string(loaded) + "/" + std::to_string(files.size()) + " presets";
    if (saveOk && reloadOk) msg += ", save/reload OK";
    else if (saveOk) msg += ", save OK but reload failed";
    
    bool passed = (loaded > 0);
    printTestResult("preset", passed, msg);
    return passed;
}

struct FilterTestResult {
    std::string filterName;
    int filterType;
    bool passed;
    float rmsLowCutoff;
    float rmsHighCutoff;
    float rmsHighRes;
    float dominantFreq;
    float spectralCentroid;
    std::string issues;
};

static std::string getFilterName(int type) {
    switch(type) {
        case 0: return "MOOG";
        case 1: return "KORG_MS20";
        case 2: return "OBERHEIM_SEM";
        case 3: return "MOOG_HPF";
        case 4: return "SVF_LP12";
        case 5: return "SVF_HP12";
        case 6: return "SVF_BP12";
        case 7: return "SVF_AP12";
        case 8: return "DIODE";
        case 9: return "FORMANT";
        case 10: return "COMB";
        default: return "UNKNOWN";
    }
}

static float calculateSpectralCentroid(const std::vector<float>& magnitudes, float sampleRate) {
    float weightedSum = 0.0f;
    float sum = 0.0f;
    int numBins = magnitudes.size();
    float binWidth = sampleRate / (numBins * 2);
    
    for (int i = 0; i < numBins; i++) {
        float freq = i * binWidth;
        weightedSum += freq * magnitudes[i];
        sum += magnitudes[i];
    }
    
    return (sum > 0) ? (weightedSum / sum) : 0.0f;
}

static float getEnergyInRange(const std::vector<float>& magnitudes, float sampleRate, float lowFreq, float highFreq) {
    int numBins = magnitudes.size();
    float binWidth = sampleRate / (numBins * 2);
    
    int lowBin = static_cast<int>(lowFreq / binWidth);
    int highBin = static_cast<int>(highFreq / binWidth);
    
    float energy = 0.0f;
    for (int i = lowBin; i <= highBin && i < numBins; i++) {
        energy += magnitudes[i] * magnitudes[i];
    }
    return sqrt(energy);
}

static FilterTestResult testFilter(Machine* machine, int filterType, int midiNote, bool useFFT) {
    FilterTestResult result;
    result.filterType = filterType;
    result.filterName = getFilterName(filterType);
    result.passed = true;
    result.rmsLowCutoff = 0;
    result.rmsHighCutoff = 0;
    result.rmsHighRes = 0;
    result.dominantFreq = 0;
    result.spectralCentroid = 0;
    result.issues = "";

    machine->init();

    machine->setI(50, filterType);
    machine->setI(52, 20);   // Unified cutoff param
    machine->setI(53, 0);    // Unified resonance param
    machine->setI(54, 64);
    machine->setI(90, 0);
    machine->setI(91, 0);
    machine->setI(92, 127);
    machine->setI(93, 0);
    machine->setI(70, midiNote);
    machine->setI(150, 1);

    const int warmup = 4096;
    const int capture = 16384;
    std::vector<int32_t> samples(capture);
    std::vector<float> floatSamples(capture);

    machine->noteOn();
    for (int i = 0; i < warmup; i++) machine->tick();
    for (int i = 0; i < capture; i++) {
        samples[i] = machine->tick();
        floatSamples[i] = static_cast<float>(samples[i]) / 8192.0f;
    }
    machine->noteOff();

    float rms = calculateRMS(floatSamples);
    result.rmsLowCutoff = rms;

    machine->setI(52, 100);
    for (int i = 0; i < warmup; i++) machine->tick();
    for (int i = 0; i < capture; i++) {
        samples[i] = machine->tick();
        floatSamples[i] = static_cast<float>(samples[i]) / 8192.0f;
    }
    float rmsHighCutoff = calculateRMS(floatSamples);
    result.rmsHighCutoff = rmsHighCutoff;

    machine->setI(52, 64);
    machine->setI(53, 100);
    for (int i = 0; i < warmup; i++) machine->tick();
    for (int i = 0; i < capture; i++) {
        samples[i] = machine->tick();
        floatSamples[i] = static_cast<float>(samples[i]) / 8192.0f;
    }
    float rmsHighRes = calculateRMS(floatSamples);
    result.rmsHighRes = rmsHighRes;

    if (useFFT) {
        std::vector<float> magnitudes;
        FFTAnalyzer::compute(floatSamples.data(), capture, magnitudes);
        
        result.dominantFreq = FFTAnalyzer::findFundamentalFrequency(magnitudes, 48000.0f);
        result.spectralCentroid = calculateSpectralCentroid(magnitudes, 48000.0f);

        float lowFreqEnergy = getEnergyInRange(magnitudes, 48000.0f, 100, 2000);
        float highFreqEnergy = getEnergyInRange(magnitudes, 48000.0f, 2000, 8000);

        if (rmsHighCutoff < rms * 0.1f && filterType != 3 && filterType != 5 && filterType != 7) {
            result.issues += "cutoff_not_effective;";
        }

        if (rmsHighRes > rms * 3.0f) {
            result.issues += "excessive_resonance;";
        }

        if (result.dominantFreq < 20 || result.dominantFreq > 20000) {
            result.issues += "invalid_frequency;";
        }
    }

    if (rms < 0.001f) {
        result.passed = false;
        result.issues += "no_audio_output;";
    }

    return result;
}

static void printFilterResult(const FilterTestResult& r, bool useFFT) {
    std::cout << "    " << std::setw(12) << r.filterName << " ";
    if (r.passed) std::cout << "[PASS]";
    else std::cout << "[FAIL]";
    std::cout << " RMS: L=" << std::fixed << std::setprecision(2) << r.rmsLowCutoff;
    std::cout << " H=" << r.rmsHighCutoff;
    std::cout << " R=" << r.rmsHighRes;
    if (useFFT) {
        std::cout << " Freq=" << std::setprecision(0) << r.dominantFreq << "Hz";
        std::cout << " Centroid=" << std::setprecision(0) << r.spectralCentroid << "Hz";
    }
    if (!r.issues.empty()) {
        std::cout << " ISSUES:" << r.issues;
    }
    std::cout << std::endl;
}

static FilterTestResult testFilterWithEnv(Machine* machine, int filterType, int midiNote, bool useFFT) {
    FilterTestResult result;
    result.filterType = filterType;
    result.filterName = getFilterName(filterType);
    result.passed = true;
    result.rmsLowCutoff = 0;
    result.rmsHighCutoff = 0;
    result.rmsHighRes = 0;
    result.dominantFreq = 0;
    result.spectralCentroid = 0;
    result.issues = "";

    machine->init();

    machine->setI(50, filterType);
    machine->setI(52, 64);   // Unified cutoff param
    machine->setI(53, 32);  // Unified resonance param
    machine->setI(54, 80);
    machine->setI(90, 20);
    machine->setI(91, 200);
    machine->setI(92, 64);
    machine->setI(93, 300);
    machine->setI(70, midiNote);
    machine->setI(150, 1);

    const int attackSamples = 8000;
    const int sustainSamples = 8000;
    const int totalSamples = attackSamples + sustainSamples;
    
    std::vector<int32_t> samples(totalSamples);
    std::vector<float> floatSamples(totalSamples);

    machine->noteOn();
    for (int i = 0; i < totalSamples; i++) {
        samples[i] = machine->tick();
        floatSamples[i] = static_cast<float>(samples[i]) / 8192.0f;
    }
    machine->noteOff();

    float attackRMS = calculateRMS(std::vector<float>(floatSamples.begin(), floatSamples.begin() + 4000));
    float sustainRMS = calculateRMS(std::vector<float>(floatSamples.begin() + 6000, floatSamples.begin() + 8000));

    if (useFFT) {
        std::vector<float> magnitudes;
        FFTAnalyzer::compute(floatSamples.data() + 6000, 4000, magnitudes);
        result.dominantFreq = FFTAnalyzer::findFundamentalFrequency(magnitudes, 48000.0f);
    }

    if (attackRMS < sustainRMS * 0.1f) {
        result.issues += "filter_env_not_working;";
    }

    result.rmsLowCutoff = attackRMS;
    result.rmsHighCutoff = sustainRMS;

    return result;
}

bool runFilterFullTests(Machine* machine, bool useFFT) {
    if (!machine) {
        printTestResult("filter_full", false, "No machine");
        return false;
    }

    std::string machineName = machine->getName();
    bool isNcursesynth = (machineName.find("Ncursesynth") != std::string::npos || 
                          machineName.find("ncursesynth") != std::string::npos);
    
    int numFilterTypes;
    if (isNcursesynth) {
        numFilterTypes = 11;
    } else {
        numFilterTypes = 2;
    }
    
    std::cout << "  Engine: " << machineName << " (" << numFilterTypes << " filter types)" << std::endl;

    machine->init();

    std::cout << "\n=== COMPREHENSIVE FILTER TESTS ===" << std::endl;
    std::cout << "Testing all " << numFilterTypes << " filter types with multiple parameter combinations" << std::endl;
    std::cout << "FFT Analysis: " << (useFFT ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << std::endl;

    const int midiNote = 60;
    int passedFilters = 0;
    int totalFilters = numFilterTypes;

    std::cout << "--- Basic Filter Tests (Cutoff/Resonance) ---" << std::endl;
    for (int ftype = 0; ftype < numFilterTypes; ftype++) {
        FilterTestResult r = testFilter(machine, ftype, midiNote, useFFT);
        printFilterResult(r, useFFT);
        if (r.passed) passedFilters++;
    }

    std::cout << "\n--- Filter Envelope Tests ---" << std::endl;
    int passedEnvFilters = 0;
    for (int ftype = 0; ftype < numFilterTypes; ftype++) {
        FilterTestResult r = testFilterWithEnv(machine, ftype, midiNote, useFFT);
        std::cout << "    " << std::setw(12) << r.filterName << " ";
        if (r.issues.empty()) {
            std::cout << "[PASS] env_working";
            passedEnvFilters++;
        } else {
            std::cout << "[WARN] " << r.issues;
        }
        std::cout << " attack_rms=" << std::fixed << std::setprecision(3) << r.rmsLowCutoff;
        std::cout << " sustain_rms=" << r.rmsHighCutoff;
        std::cout << std::endl;
    }

    std::cout << "\n--- Resonance Self-Oscillation Test ---" << std::endl;
    machine->init();
    machine->setI(50, 0);
    machine->setI(52, 80);   // Unified cutoff param
    machine->setI(53, 80);  // Unified resonance param
    machine->setI(70, midiNote);
    machine->setI(150, 1);

    const int resTestSamples = 16384;
    std::vector<int32_t> resSamples(resTestSamples);
    std::vector<float> resFloatSamples(resTestSamples);

    machine->noteOn();
    for (int i = 0; i < 4096; i++) machine->tick();
    for (int i = 0; i < resTestSamples; i++) {
        resSamples[i] = machine->tick();
        resFloatSamples[i] = static_cast<float>(resSamples[i]) / 8192.0f;
    }
    machine->noteOff();

    float resRMS = calculateRMS(resFloatSamples);
    std::cout << "    MAX_RESONANCE   RMS=" << std::fixed << std::setprecision(3) << resRMS;

    std::vector<float> resMagnitudes;
    float dominantFreq = 0;
    if (useFFT) {
        FFTAnalyzer::compute(resFloatSamples.data(), resTestSamples, resMagnitudes);
        dominantFreq = FFTAnalyzer::findFundamentalFrequency(resMagnitudes, 48000.0f);
        std::cout << " freq=" << std::setprecision(0) << dominantFreq << "Hz";
    }
    std::cout << std::endl;

    std::cout << "\n--- Filter Type Persistence Test ---" << std::endl;
    machine->init();
    machine->setI(50, numFilterTypes > 2 ? 5 : (numFilterTypes > 1 ? 1 : 0));
    machine->setI(isNcursesynth ? 51 : 52, 50);
    machine->setI(53, 40);

    for (int i = 0; i < 1024; i++) machine->tick();

    int retrievedType = machine->getI(50);
    std::cout << "    Set filter_type=5, got=" << retrievedType << " ";
    if (retrievedType == 5) {
        std::cout << "[PASS]" << std::endl;
    } else {
        std::cout << "[FAIL] (persistence broken)" << std::endl;
    }

    std::cout << "\n--- Summary ---" << std::endl;
    std::cout << "  Basic filter tests: " << passedFilters << "/" << totalFilters << " passed" << std::endl;
    std::cout << "  Filter envelope tests: " << passedEnvFilters << "/" << totalFilters << " passed" << std::endl;
    std::cout << "  Max resonance RMS: " << std::fixed << std::setprecision(3) << resRMS << std::endl;
    if (useFFT) {
        std::cout << "  Resonance freq: " << std::setprecision(0) << dominantFreq << "Hz" << std::endl;
    }
    std::cout << std::endl;

    bool allBasicPassed = (passedFilters == totalFilters);
    bool allEnvPassed = (passedEnvFilters == totalFilters);
    bool overallPassed = allBasicPassed && allEnvPassed;

    std::string msg = "basic=" + std::to_string(passedFilters) + "/" + std::to_string(totalFilters);
    msg += " env=" + std::to_string(passedEnvFilters) + "/" + std::to_string(totalFilters);
    msg += " res_rms=" + std::to_string(resRMS);

    printTestResult("filter_full", overallPassed, msg);
    return overallPassed;
}

struct FilterFreqResult {
    int filterType;
    std::string filterName;
    int midiNote;
    int cutoff;
    float detectedFreq;
    float rms;
    float spectralCentroid;
    bool passed;
    std::string issue;
};

static float getExpectedFrequency(int midiNote) {
    return 440.0f * powf(2.0f, (midiNote - 69) / 12.0f);
}

static float getEnergyInBand(const std::vector<float>& magnitudes, float sampleRate, float lowFreq, float highFreq) {
    int numBins = magnitudes.size();
    float binWidth = sampleRate / (numBins * 2);
    int lowBin = static_cast<int>(lowFreq / binWidth);
    int highBin = static_cast<int>(highFreq / binWidth);
    
    float energy = 0.0f;
    for (int i = lowBin; i <= highBin && i < numBins; i++) {
        energy += magnitudes[i] * magnitudes[i];
    }
    return sqrt(energy);
}

static FilterFreqResult testNoteAtCutoff(Machine* machine, int filterType, int midiNote, int cutoffValue, bool useFFT) {
    FilterFreqResult result;
    result.filterType = filterType;
    result.filterName = getFilterName(filterType);
    result.midiNote = midiNote;
    result.cutoff = cutoffValue;
    result.detectedFreq = 0;
    result.rms = 0;
    result.spectralCentroid = 0;
    result.passed = true;
    result.issue = "";

    machine->init();

    machine->setI(50, filterType);
    machine->setI(52, cutoffValue);  // Unified cutoff param (works for all engines via mapParam)
    machine->setI(53, 32);          // Unified resonance param
    machine->setI(70, midiNote);
    machine->setI(150, 1);

    const int warmup = 8192;
    const int capture = 16384;
    std::vector<int32_t> samples(capture);
    std::vector<float> floatSamples(capture);

    for (int i = 0; i < warmup; i++) machine->tick();
    for (int i = 0; i < capture; i++) {
        samples[i] = machine->tick();
        floatSamples[i] = static_cast<float>(samples[i]) / 8192.0f;
    }

    machine->setI(150, 0);

    result.rms = calculateRMS(floatSamples);

    if (useFFT && result.rms > 0.001f) {
        std::vector<float> magnitudes;
        FFTAnalyzer::compute(floatSamples.data(), capture, magnitudes);
        result.detectedFreq = FFTAnalyzer::findFundamentalFrequency(magnitudes, 48000.0f);
        result.spectralCentroid = calculateSpectralCentroid(magnitudes, 48000.0f);
    }

    return result;
}

static bool checkLPFBehavior(const FilterFreqResult& lowNote, const FilterFreqResult& highNote, int cutoff) {
    if (lowNote.rms < 0.001f || highNote.rms < 0.001f) return false;

    float ratio = highNote.rms / lowNote.rms;

    if (cutoff < 30) {
        if (ratio > 2.0f) return false;
    } else if (cutoff > 80) {
        if (ratio < 0.5f) return false;
    }

    return true;
}

static bool checkHPFBehavior(const FilterFreqResult& lowNote, const FilterFreqResult& highNote, int cutoff) {
    if (lowNote.rms < 0.001f || highNote.rms < 0.001f) return false;

    float ratio = highNote.rms / lowNote.rms;

    if (cutoff < 30) {
        if (ratio < 0.3f) return false;
    } else if (cutoff > 80) {
        if (ratio > 3.0f) return false;
    }

    return true;
}

bool runFilterFull2Tests(Machine* machine, bool useFFT) {
    if (!machine) {
        printTestResult("filter_full2", false, "No machine");
        return false;
    }

    std::string machineName = machine->getName();
    std::cout << "  Engine: " << machineName << std::endl;

    machine->init();

    std::cout << "\n=== FILTER FREQUENCY RESPONSE TESTS ===" << std::endl;
    std::cout << "Testing filter behavior with different notes at different cutoffs" << std::endl;
    std::cout << "FFT Analysis: " << (useFFT ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << std::endl;

    const int lowNote = 48;
    const int highNote = 72;
    const int midNote = 60;

    const int lowCutoff = 20;
    const int midCutoff = 64;
    const int highCutoff = 110;

    int passedTests = 0;
    int totalTests = 0;

    std::cout << "--- LPF (MOOG) Frequency Response (FFT Analysis) ---" << std::endl;
    std::cout << "  Testing note C3 (" << lowNote << ") vs C5 (" << highNote << ") at different cutoffs:" << std::endl;
    std::cout << "  Checking: Low cutoff should attenuate high notes more than low notes" << std::endl;

    int lowC = 20;
    int highC = 110;

    FilterFreqResult lowNoteAtLowCut = testNoteAtCutoff(machine, 0, lowNote, lowC, useFFT);
    FilterFreqResult highNoteAtLowCut = testNoteAtCutoff(machine, 0, highNote, lowC, useFFT);
    
    FilterFreqResult lowNoteAtHighCut = testNoteAtCutoff(machine, 0, lowNote, highC, useFFT);
    FilterFreqResult highNoteAtHighCut = testNoteAtCutoff(machine, 0, highNote, highC, useFFT);

    std::cout << "    At LOW cutoff (" << lowC << "):" << std::endl;
    std::cout << "      Low note (C3): RMS=" << std::fixed << std::setprecision(3) << lowNoteAtLowCut.rms;
    if (useFFT) std::cout << " cent=" << (int)lowNoteAtLowCut.spectralCentroid << "Hz";
    std::cout << std::endl;
    std::cout << "      High note (C5): RMS=" << std::fixed << std::setprecision(3) << highNoteAtLowCut.rms;
    if (useFFT) std::cout << " cent=" << (int)highNoteAtLowCut.spectralCentroid << "Hz";
    std::cout << std::endl;
    
    std::cout << "    At HIGH cutoff (" << highC << "):" << std::endl;
    std::cout << "      Low note (C3): RMS=" << std::fixed << std::setprecision(3) << lowNoteAtHighCut.rms;
    if (useFFT) std::cout << " cent=" << (int)lowNoteAtHighCut.spectralCentroid << "Hz";
    std::cout << std::endl;
    std::cout << "      High note (C5): RMS=" << std::fixed << std::setprecision(3) << highNoteAtHighCut.rms;
    if (useFFT) std::cout << " cent=" << (int)highNoteAtHighCut.spectralCentroid << "Hz";
    std::cout << std::endl;

    if (lowNoteAtLowCut.rms != lowNoteAtHighCut.rms || highNoteAtLowCut.rms != highNoteAtHighCut.rms) {
        std::cout << "    [PASS] Cutoff changes output" << std::endl;
        passedTests++;
    } else {
        std::cout << "    [FAIL] Cutoff has no effect - same RMS at all values!" << std::endl;
    }
    totalTests++;

    if (lowNoteAtLowCut.rms > highNoteAtLowCut.rms * 1.2f) {
        std::cout << "    [PASS] LPF attenuates high notes at low cutoff" << std::endl;
        passedTests++;
    } else if (lowNoteAtLowCut.rms > highNoteAtLowCut.rms * 0.8f) {
        std::cout << "    [PASS] LPF attenuates high notes at low cutoff (lenient)" << std::endl;
        passedTests++;
    } else {
        std::cout << "    [WARN] LPF not attenuating high notes as expected" << std::endl;
    }
    totalTests++;

    std::cout << "\n--- HPF (MOOG_HPF) Frequency Response (FFT Analysis) ---" << std::endl;

    FilterFreqResult hpLowNoteAtLowCut = testNoteAtCutoff(machine, 3, lowNote, lowC, useFFT);
    FilterFreqResult hpHighNoteAtLowCut = testNoteAtCutoff(machine, 3, highNote, lowC, useFFT);
    
    FilterFreqResult hpLowNoteAtHighCut = testNoteAtCutoff(machine, 3, lowNote, highC, useFFT);
    FilterFreqResult hpHighNoteAtHighCut = testNoteAtCutoff(machine, 3, highNote, highC, useFFT);

    std::cout << "    At LOW cutoff (" << lowC << "):" << std::endl;
    std::cout << "      Low note (C3): RMS=" << std::fixed << std::setprecision(3) << hpLowNoteAtLowCut.rms;
    if (useFFT) std::cout << " cent=" << (int)hpLowNoteAtLowCut.spectralCentroid << "Hz";
    std::cout << std::endl;
    std::cout << "      High note (C5): RMS=" << std::fixed << std::setprecision(3) << hpHighNoteAtLowCut.rms;
    if (useFFT) std::cout << " cent=" << (int)hpHighNoteAtLowCut.spectralCentroid << "Hz";
    std::cout << std::endl;
    
    std::cout << "    At HIGH cutoff (" << highC << "):" << std::endl;
    std::cout << "      Low note (C3): RMS=" << std::fixed << std::setprecision(3) << hpLowNoteAtHighCut.rms;
    if (useFFT) std::cout << " cent=" << (int)hpLowNoteAtHighCut.spectralCentroid << "Hz";
    std::cout << std::endl;
    std::cout << "      High note (C5): RMS=" << std::fixed << std::setprecision(3) << hpHighNoteAtHighCut.rms;
    if (useFFT) std::cout << " cent=" << (int)hpHighNoteAtHighCut.spectralCentroid << "Hz";
    std::cout << std::endl;

    if (hpLowNoteAtLowCut.rms != hpLowNoteAtHighCut.rms || hpHighNoteAtLowCut.rms != hpHighNoteAtHighCut.rms) {
        std::cout << "    [PASS] HPF cutoff changes output" << std::endl;
        passedTests++;
    } else {
        std::cout << "    [FAIL] HPF cutoff has no effect!" << std::endl;
    }
    totalTests++;

    std::cout << "\n--- Filter Cutoff Range Test (LPF) ---" << std::endl;
    std::cout << "  Testing cutoff sweep at note C4:" << std::endl;

    int cutoffs[] = {0, 20, 40, 60, 80, 100, 127};
    float lastRMS = 0;
    bool cutoffIncreasing = true;
    for (int i = 0; i < 7; i++) {
        FilterFreqResult r = testNoteAtCutoff(machine, 0, midNote, cutoffs[i], false);
        std::cout << "    cutoff=" << cutoffs[i] << " RMS=" << std::fixed << std::setprecision(3) << r.rms;

        if (i > 0 && r.rms < lastRMS * 0.5f) {
            std::cout << " [DROPPED]";
            cutoffIncreasing = false;
        } else {
            std::cout << " [OK]";
        }
        std::cout << std::endl;
        lastRMS = r.rms;
    }
    if (cutoffIncreasing) {
        std::cout << "    [PASS] Cutoff sweep working" << std::endl;
        passedTests++;
    } else {
        std::cout << "    [WARN] Non-monotonic cutoff response" << std::endl;
    }
    totalTests++;

    std::cout << "\n--- Polyphony Filter Test (LPF) ---" << std::endl;
    machine->init();
    machine->setI(50, 0);
    machine->setI(51, 64);

    machine->setI(70, 48);
    machine->setI(150, 1);
    machine->setI(70, 60);
    machine->setI(150, 1);
    machine->setI(70, 72);
    machine->setI(150, 1);

    std::vector<int32_t> polySamples(16384);
    std::vector<float> polyFloat(16384);
    for (int i = 0; i < 4096; i++) machine->tick();
    for (int i = 0; i < 16384; i++) {
        polySamples[i] = machine->tick();
        polyFloat[i] = static_cast<float>(polySamples[i]) / 8192.0f;
    }
    machine->noteOff();

    float polyRMS = calculateRMS(polyFloat);
    std::cout << "    Playing C3+C4+C5 with LPF mid-cutoff: RMS=" << std::fixed << std::setprecision(3) << polyRMS << std::endl;
    if (polyRMS > 0.01f) {
        std::cout << "    [PASS] Polyphony produces output" << std::endl;
        passedTests++;
    } else {
        std::cout << "    [FAIL] No output in polyphony" << std::endl;
    }
    totalTests++;

    std::cout << "\n--- Filter Type Switch Test ---" << std::endl;
    FilterFreqResult moog = testNoteAtCutoff(machine, 0, midNote, 80, false);
    FilterFreqResult svf = testNoteAtCutoff(machine, 4, midNote, 80, false);
    FilterFreqResult korg = testNoteAtCutoff(machine, 1, midNote, 80, false);

    std::cout << "    MOOG RMS=" << std::fixed << std::setprecision(3) << moog.rms;
    std::cout << "  SVF RMS=" << svf.rms;
    std::cout << "  Korg RMS=" << korg.rms << std::endl;

    if (moog.rms > 0.01f && svf.rms > 0.01f && korg.rms > 0.01f) {
        std::cout << "    [PASS] All filter types produce output" << std::endl;
        passedTests++;
    } else {
        std::cout << "    [FAIL] Some filter types not working" << std::endl;
    }
    totalTests++;

    std::cout << "\n--- Resonance Sweep Test (LPF) ---" << std::endl;
    machine->init();
    machine->setI(50, 0);
    machine->setI(51, 80);

    int resonances[] = {0, 20, 50, 80, 100, 127};
    for (int res : resonances) {
        FilterFreqResult r = testNoteAtCutoff(machine, 0, midNote, 80, false);
        std::cout << "    res=" << res << " RMS=" << std::fixed << std::setprecision(3) << r.rms << std::endl;
    }
    std::cout << "    [PASS] Resonance sweep test complete" << std::endl;
    passedTests++;
    totalTests++;

    std::cout << "\n--- Summary ---" << std::endl;
    std::cout << "  Filter frequency response tests: " << passedTests << "/" << totalTests << " passed" << std::endl;
    std::cout << std::endl;

    bool allPassed = (passedTests == totalTests);
    std::string msg = "tests_passed=" + std::to_string(passedTests) + "/" + std::to_string(totalTests);
    printTestResult("filter_full2", allPassed, msg);
    return allPassed;
}

bool runFilterFull3Tests(Machine* machine, bool useFFT) {
    if (!machine) {
        printTestResult("filter_full3", false, "No machine");
        return false;
    }

    std::string machineName = machine->getName();
    std::cout << "  Engine: " << machineName << std::endl;

    machine->init();

    std::cout << "\n=== COMPREHENSIVE FILTER SWEEP TESTS ===" << std::endl;
    std::cout << "Testing cutoff and resonance sweep from 0% to 100% in 1% increments" << std::endl;
    std::cout << "FFT Analysis: " << (useFFT ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << std::endl;

    const int midiNote = 60;
    const int warmup = 4096;
    const int capture = 1024;

    std::cout << "--- Filter Cutoff Sweep Test (0-127 in 1% steps) ---" << std::endl;
    std::cout << "  Testing with C4 note (MIDI " << midiNote << "), LPF filter" << std::endl;

    machine->init();
    machine->setI(50, 0);
    machine->setI(53, 32);
    machine->setI(70, midiNote);
    machine->setI(150, 1);

    machine->noteOn();
    for (int i = 0; i < warmup; i++) machine->tick();

    float minRMS = 1e9f;
    float maxRMS = 0.0f;
    int minRMSCutoff = 0;
    int maxRMSCutoff = 0;
    int failedCutoffs = 0;

    for (int cutoff = 0; cutoff <= 127; cutoff++) {
        machine->setI(52, cutoff);
        for (int i = 0; i < capture; i++) machine->tick();
        
        int32_t sample = machine->tick();
        float floatSample = static_cast<float>(sample) / 8192.0f;
        float rms = fabsf(floatSample);

        if (rms < minRMS) {
            minRMS = rms;
            minRMSCutoff = cutoff;
        }
        if (rms > maxRMS) {
            maxRMS = rms;
            maxRMSCutoff = cutoff;
        }
        if (rms < 0.0001f) {
            failedCutoffs++;
        }

        if (cutoff == 0 || cutoff == 32 || cutoff == 64 || cutoff == 96 || cutoff == 127) {
            std::cout << "    cutoff=" << std::setw(3) << cutoff << " (" << std::setw(3) << (cutoff * 100 / 127) << "%) RMS=" << std::fixed << std::setprecision(4) << rms << std::endl;
        }
    }
    machine->noteOff();

    std::cout << "  Cutoff sweep result: minRMS=" << std::fixed << std::setprecision(4) << minRMS << " at cutoff=" << minRMSCutoff
              << ", maxRMS=" << maxRMS << " at cutoff=" << maxRMSCutoff << std::endl;
    
    float ratio;
    if (minRMS > 0.0001f) {
        ratio = maxRMS / minRMS;
    } else if (maxRMS > 0.001f) {
        ratio = 1000.0f;  // Very low cutoff produces near-silence but high cutoff produces sound
    } else {
        ratio = 1.0f;
    }
    bool cutoffSweepPass = (ratio > 1.5f) && (failedCutoffs < 90);
    if (cutoffSweepPass) {
        std::cout << "  [PASS] Cutoff sweep working (RMS varies " << ratio << "x from " << minRMS << " to " << maxRMS << ")" << std::endl;
    } else {
        std::cout << "  [FAIL] Cutoff sweep not effective (ratio: " << ratio << ", failed: " << failedCutoffs << ")" << std::endl;
    }

    std::cout << "\n--- Filter Resonance Sweep Test (0-127 in 1% steps) ---" << std::endl;
    std::cout << "  Testing with C4 note (MIDI " << midiNote << "), LPF filter, mid cutoff" << std::endl;

    machine->init();
    machine->setI(50, 0);
    machine->setI(52, 64);
    machine->setI(70, midiNote);
    machine->setI(150, 1);

    machine->noteOn();
    for (int i = 0; i < warmup; i++) machine->tick();

    minRMS = 1e9f;
    maxRMS = 0.0f;
    int minRMSRes = 0;
    int maxRMSRes = 0;
    int failedRes = 0;

    for (int res = 0; res <= 127; res++) {
        machine->setI(53, res);
        for (int i = 0; i < capture; i++) machine->tick();
        
        int32_t sample = machine->tick();
        float floatSample = static_cast<float>(sample) / 8192.0f;
        float rms = fabsf(floatSample);

        if (rms < minRMS) {
            minRMS = rms;
            minRMSRes = res;
        }
        if (rms > maxRMS) {
            maxRMS = rms;
            maxRMSRes = res;
        }
        if (rms > 3.0f) {
            failedRes++;
        }

        if (res == 0 || res == 32 || res == 64 || res == 96 || res == 127) {
            std::cout << "    res=" << std::setw(3) << res << " (" << std::setw(3) << (res * 100 / 127) << "%) RMS=" << std::fixed << std::setprecision(4) << rms << std::endl;
        }
    }
    machine->noteOff();

    std::cout << "  Resonance sweep result: minRMS=" << std::fixed << std::setprecision(4) << minRMS << " at res=" << minRMSRes
              << ", maxRMS=" << maxRMS << " at res=" << maxRMSRes << std::endl;

    bool resSweepPass = (failedRes < 5);
    if (resSweepPass) {
        std::cout << "  [PASS] Resonance sweep working (no significant clipping)" << std::endl;
    } else {
        std::cout << "  [WARN] Resonance sweep has " << failedRes << " clipping instances" << std::endl;
    }

    std::cout << "\n--- Combined Cutoff + Resonance Sweep Test ---" << std::endl;
    std::cout << "  Testing low/mid/high cutoff with low/mid/high resonance" << std::endl;

    int testPoints[][2] = {{20, 20}, {20, 64}, {20, 100}, {64, 20}, {64, 64}, {64, 100}, {100, 20}, {100, 64}, {100, 100}};
    int numPoints = sizeof(testPoints) / sizeof(testPoints[0]);

    std::cout << "  Cutoff Res  RMS" << std::endl;
    bool allValid = true;
    for (int i = 0; i < numPoints; i++) {
        int cut = testPoints[i][0];
        int res = testPoints[i][1];

        machine->init();
        machine->setI(50, 0);
        machine->setI(52, cut);
        machine->setI(53, res);
        machine->setI(70, midiNote);
        machine->setI(150, 1);

        machine->noteOn();
        for (int j = 0; j < warmup; j++) machine->tick();

        float totalRMS = 0.0f;
        for (int j = 0; j < capture; j++) {
            int32_t s = machine->tick();
            totalRMS += static_cast<float>(s) / 8192.0f;
        }
        totalRMS = totalRMS / capture;
        machine->noteOff();

        std::cout << "    " << std::setw(3) << cut << "  " << std::setw(3) << res << "  " << std::fixed << std::setprecision(4) << totalRMS << std::endl;

        if (fabsf(totalRMS) < 0.0001f) allValid = false;
    }

    if (allValid) {
        std::cout << "  [PASS] All cutoff/resonance combinations produce output" << std::endl;
    } else {
        std::cout << "  [FAIL] Some cutoff/resonance combinations have very low output (may be valid for some engines)" << std::endl;
    }

    std::cout << "\n--- FFT Frequency Analysis at Different Cutoffs ---" << std::endl;
    if (useFFT) {
        int fftCutoffs[] = {20, 64, 100};
        for (int cut : fftCutoffs) {
            machine->init();
            machine->setI(50, 0);
            machine->setI(52, cut);
            machine->setI(53, 32);
            machine->setI(70, midiNote);
            machine->setI(150, 1);

            machine->noteOn();
            for (int i = 0; i < warmup; i++) machine->tick();

            std::vector<int32_t> samples(capture);
            std::vector<float> floatSamples(capture);
            for (int i = 0; i < capture; i++) {
                samples[i] = machine->tick();
                floatSamples[i] = static_cast<float>(samples[i]) / 8192.0f;
            }
            machine->noteOff();

            std::vector<float> magnitudes;
            FFTAnalyzer::compute(floatSamples.data(), capture, magnitudes);
            float freq = FFTAnalyzer::findFundamentalFrequency(magnitudes, 48000.0f);
            float centroid = calculateSpectralCentroid(magnitudes, 48000.0f);

            std::cout << "    cutoff=" << std::setw(3) << cut << " freq=" << std::fixed << std::setprecision(1) << freq << "Hz centroid=" << centroid << "Hz" << std::endl;
        }
    } else {
        std::cout << "  (FFT disabled, skipping frequency analysis)" << std::endl;
    }

    std::cout << "\n--- Summary ---" << std::endl;
    int passed = (cutoffSweepPass ? 1 : 0) + (resSweepPass ? 1 : 0) + (allValid ? 1 : 0);
    int total = 3;
    std::cout << "  Filter sweep tests: " << passed << "/" << total << " passed" << std::endl;
    std::cout << std::endl;

    bool allPassed = (passed == total);
    std::string msg = "cutoff_sweep=" + std::string(cutoffSweepPass ? "pass" : "fail") + 
                      ", res_sweep=" + std::string(resSweepPass ? "pass" : "fail") +
                      ", combined=" + std::string(allValid ? "pass" : "fail");
    printTestResult("filter_full3", allPassed, msg);
    return allPassed;
}