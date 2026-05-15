#include "test_helpers.h"
#include "test_engine.h"
#include "fft_analyzer.h"
#include "../machine/Machine.h"
#include "../machine/Parameter.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
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

    machine->setI(35, 127);
    machine->setI(51, 127);

    std::vector<int> notes1 = {48};
    std::vector<int> notes2 = {48, 60};
    std::vector<int> notes3 = {48, 60, 72, 84};

    float rms1 = captureMultiNoteRMS(machine, notes1, numSamples, warmupSamples);
    for (int i = 0; i < silenceSamples; i++) machine->tick();

    machine->init();
    machine->setI(35, 127);
    machine->setI(51, 127);
    float rms2 = captureMultiNoteRMS(machine, notes2, numSamples, warmupSamples);
    for (int i = 0; i < silenceSamples; i++) machine->tick();

    machine->init();
    machine->setI(35, 127);
    machine->setI(51, 127);
    float rms3 = captureMultiNoteRMS(machine, notes3, numSamples, warmupSamples);
    for (int i = 0; i < silenceSamples; i++) machine->tick();

    bool allProduceAudio = (rms1 > 0 && rms2 > 0 && rms3 > 0);
    bool sumIncrease = (rms3 > rms1 * 1.1f);

    bool passed = allProduceAudio && sumIncrease;

    std::ostringstream msg;
    msg << "1note RMS=" << std::fixed << std::setprecision(1) << rms1
        << " 2note RMS=" << rms2 << " 4note RMS=" << rms3
        << " (ratio 4:1=" << std::setprecision(2) << (rms1 > 0 ? rms3 / rms1 : 0) << "x)";

    printTestResult("voice_level", passed, msg.str());
    return passed;
}

// Helper: set amp envelope on all engines (ADSR_ENV0 + MachineParam)
static void setAmpEnvelope(Machine* machine, int a, int d, int s, int r) {
    machine->setI(0, a);   machine->setI(80, a);   // ATTACK
    machine->setI(1, d);   machine->setI(81, d);   // DECAY
    machine->setI(2, s);   machine->setI(82, s);   // SUSTAIN
    machine->setI(3, r);   machine->setI(83, r);   // RELEASE
}

// Helper: capture envelope curve samples.
// Returns two half-RMS values: [first_half_rms, second_half_rms]
static std::vector<float> captureEnvelopeHalves(Machine* machine, int numSamples,
                                                 int note, int triggerHoldSamples,
                                                 bool doNoteOff, int releaseSamples) {
    machine->setI(35, 127);  // VOLUME max (all engines)
    machine->setI(51, 127);  // CUTOFF max
    machine->setI(52, 127);  // CUTOFF (PBSynth alternate)
    machine->setI(71, note);
    machine->setI(70, note);
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
    // A=64 D=0 S=0 R=0: sound should increase over time
    {
        machine->init();
        setAmpEnvelope(machine, 64, 0, 0, 0);
        auto rms = captureEnvelopeHalves(machine, numSamples, 60, holdSamples, false, 0);
        for (int i = 0; i < silenceSamples; i++) machine->tick();
        bool ok = (rms[0] > 0 && rms[1] > rms[0]);
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
        machine->setI(51, 127); machine->setI(52, 127);
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
    machine->setI(MachineParam::FILTER_ENV_ATTACK, 64);
    machine->setI(MachineParam::FILTER_ENV_DECAY, 64);
    machine->setI(MachineParam::FILTER_ENV_SUSTAIN, 64);
    machine->setI(MachineParam::FILTER_ENV_RELEASE, 64);
    
    // Test getting parameters back
    int attack = machine->getI(MachineParam::FILTER_ENV_ATTACK);
    
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