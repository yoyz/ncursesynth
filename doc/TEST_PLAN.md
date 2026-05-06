# ncursesynth Automated Testing Framework

## Executive Summary

Comprehensive automated testing framework for synth engines integration into the virtual_synth main program. The framework provides **headless testing** capabilities without physical audio/MIDI hardware.

**Current Status**: Infrastructure in place - Test runner operational with `[NOTIMPLEMENTED]` for unimplemented tests. All 9 test categories defined but require real implementation.

---

## 1. Test Runner Usage

### 1.1 CLI Interface

```bash
# Show help (default when no flags)
./test_runner

# Show specific help
./test_runner -h
./test_runner --help

# List available engines
./test_runner --list-engines

# List available tests
./test_runner --list-tests

# Run specific engine + tests (REQUIRED)
./test_runner --engine ncursesynth --tests sound
./test_runner --engine pbsynth --tests sound,silence

# Run all tests for an engine
./test_runner --engine ncursesynth --tests all

# Run all engines, all tests
./test_runner --all-engines --all-tests

# With FFT analysis
./test_runner --engine ncursesynth --tests all --fft
```

### 1.2 CLI Options

| Option | Description | Required |
|--------|-------------|----------|
| `-h, --help` | Show this help | No |
| `--list-engines` | List available engines | No |
| `--list-tests` | List available tests | No |
| `--engine NAME` | Specify engine | Yes* |
| `--all-engines` | Run tests on all engines | Yes* |
| `--tests LIST` | Specify tests (comma-separated) | Yes* |
| `--all-tests` | Run all available tests | Yes* |
| `--verbose` | Verbose output | No |
| `--quiet` | Minimal output | No |
| `--fft` | Enable FFT analysis | No |

*Either `--engine` or `--all-engines` AND either `--tests` or `--all-tests` required.

### 1.3 Available Engines

Defined in `test_runner.cpp` at top:

```cpp
static const std::vector<std::string> g_availableEngines = {
    "ncursesynth",  // Original ncursesynth engine
    "pbsynth",      // PBSynth (8-voice polyphonic)
    "cursynth",     // Cursynth engine
    "twytch"        // Twytch (Helm-based) engine
};
```

### 1.4 Available Tests

Defined in `test_runner.cpp` at top:

```cpp
static const std::vector<std::string> g_availableTests = {
    "sound",        // Basic sound production
    "silence",      // Volume -> 0 produces silence
    "no_clip",      // Max volume doesn't clip
    "note_on_off",  // Note on/off trigger
    "note_release", // Note release handling
    "octave",       // Octave progression tests
    "cc_control",   // CC parameter control
    "polyphony",    // Multi-voice handling
    "filter_env"    // Filter envelope tests
};
```

---

## 2. Implementation Status

### 2.0 Current Test Results (ALL FAIL - NOTIMPLEMENTED)

```
=== Running tests for ncursesynth ===
  [INFO] Machine ncursesynth initialized

=== Running All Tests ===
  [RUN] sound
  [NOTIMPLEMENTED] sound
  [RUN] silence
  [NOTIMPLEMENTED] silence
  [RUN] no_clip
  [NOTIMPLEMENTED] no_clip
  [RUN] note_on_off
  [NOTIMPLEMENTED] note_on_off
  [RUN] note_release
  [NOTIMPLEMENTED] note_release
  [RUN] octave
  [NOTIMPLEMENTED] octave
  [RUN] cc_control
  [NOTIMPLEMENTED] cc_control
  [RUN] polyphony
  [NOTIMPLEMENTED] polyphony
  [RUN] filter_env
  [NOTIMPLEMENTED] filter_env

=== Test Summary ===
Passed: 0
Failed: 0
Rate: 0.0%
```

**All tests FAIL with [NOTIMPLEMENTED]** because they don't:
1. Capture audio from Machine using FakeAudioDriver
2. Analyze audio using FFTAnalyzer
3. Verify frequency, clipping, silence, or any parameter

### 2.1 Test Runner

| Feature | Status | Notes |
|---------|--------|-------|
| Help by default | ✓ Complete | Shows help when no flags |
| --list-engines | ✓ Complete | Lists available engines |
| --list-tests | ✓ Complete | Lists available tests |
| Engine selection | ✓ Complete | --engine or --all-engines |
| Test selection | ✓ Complete | --tests or --all-tests |
| NOTIMPLEMENTED output | ✓ Complete | Unimplemented tests FAIL with [NOTIMPLEMENTED] |

### 2.2 Current Test Results

```
=== Running tests for ncursesynth ===
  [INFO] Machine ncursesynth initialized

=== Running All Tests ===
  [RUN] sound
  [NOTIMPLEMENTED] sound
  [RUN] silence
  [NOTIMPLEMENTED] silence
  [RUN] no_clip
  [NOTIMPLEMENTED] no_clip
  [RUN] note_on_off
  [NOTIMPLEMENTED] note_on_off
  [RUN] note_release
  [NOTIMPLEMENTED] note_release
  [RUN] octave
  [NOTIMPLEMENTED] octave
  [RUN] cc_control
  [NOTIMPLEMENTED] cc_control
  [RUN] polyphony
  [NOTIMPLEMENTED] polyphony
  [RUN] filter_env
  [NOTIMPLEMENTED] filter_env

=== Test Summary ===
Passed: 0
Failed: 0
Rate: 0.0%
```

**IMPORTANT**: Tests show `[NOTIMPLEMENTED]` because they don't perform real verification - they just call machine->init(), tick(), noteOn(), noteOff() without checking results.

---

## 3. How to Add a New Test

### 3.1 Adding a Test to the Framework

1. **Add test name to global list** in `test_runner.cpp`:
   ```cpp
   static const std::vector<std::string> g_availableTests = {
       ...
       "my_new_test"  // Add here
   };
   
   static const char* g_testDescriptions[] = {
       ...
       "Description of my new test"  // Add matching description
   };
   ```

2. **Add test function declaration** in `test/test_helpers.h`:
   ```cpp
   bool runMyNewTestTests(Machine* machine, bool useFFT);
   ```

3. **Implement test function** in `test/test_helpers.cpp`:
   ```cpp
   bool runMyNewTestTests(Machine* machine, bool useFFT) {
       if (!machine) { 
           printTestResult("my_new_test", false, "No machine"); 
           return false; 
       }
       
       // TODO: Implement real verification
       // Example:
       machine->init();
       machine->noteOn();
       machine->tick();
       
       // Verify audio output, frequency, etc.
       // Use FakeAudioDriver and FFTAnalyzer for audio verification
       
       // If not implemented, show NOTIMPLEMENTED and return false
       printNotImplemented("my_new_test");
       return false;
   }
   ```

4. **Add test runner wrapper** in `test_runner.cpp`:
   ```cpp
   bool TestRunner::runMyNewTestTests(Machine* machine, bool useFFT) {
       return runMyNewTestTests(machine, useFFT);
   }
   ```

5. **Add declaration** in `test/test_runner.h`:
   ```cpp
   bool runMyNewTestTests(Machine* machine, bool useFFT = false);
   ```

6. **Register test in runAllTests()** in `test_runner.cpp`:
   ```cpp
   if (shouldRunTest("my_new_test")) {
       std::cout << "  [RUN] my_new_test" << std::endl;
       allPassed = runMyNewTestTests(machine, useFFT) && allPassed;
   }
   ```

### 3.2 Test Implementation Guidelines

A proper test should:
1. Initialize the machine
2. Send MIDI messages (note on/off, CC)
3. Capture audio output (via FakeAudioDriver)
4. Verify the output meets expected criteria
5. Return `true` on pass, `false` on fail

Example with FFT verification:
```cpp
bool runSoundProductionTests(Machine* machine, bool useFFT) {
    if (!machine) { 
        printTestResult("sound", false, "No machine"); 
        return false; 
    }
    
    machine->init();
    machine->noteOn();
    
    // Capture audio using FakeAudioDriver
    std::vector<float> audioData = fakeAudio.getFloat32Data();
    
    if (useFFT && !audioData.empty()) {
        // Use FFTAnalyzer to verify frequency
        std::vector<float> magnitudes;
        FFTAnalyzer::compute(audioData.data(), audioData.size(), magnitudes);
        float fundamentalFreq = FFTAnalyzer::findFundamentalFrequency(magnitudes, 48000.0f);
        float midiNote = FFTAnalyzer::frequencyToMidi(fundamentalFreq);
        
        // Verify within ±1 semitone of expected note (e.g., C4 = 60)
        if (std::abs(midiNote - 60.0f) <= 1.0f) {
            printTestResult("sound", true, "freq=261.63Hz MIDI=60");
            return true;
        }
    }
    
    // Fallback: just check if audio is generated
    if (!audioData.empty()) {
        printTestResult("sound", true, "audio generated");
        return true;
    }
    
    printTestResult("sound", false, "no audio output");
    return false;
}
```

---

## 4. How to Add a New Engine

### 4.1 Engine Integration Checklist

1. **Register machine in test_runner.cpp**:
   ```cpp
   // In runSingleEngine() or runAllEngines()
   if (engineName == "myengine") {
       machine = new MyEngineMachine();
   }
   ```

2. **Add to global engines list**:
   ```cpp
   static const std::vector<std::string> g_availableEngines = {
       ...
       "myengine"  // Add here
   };
   
   static const char* g_engineDescriptions[] = {
       ...
       "My engine description"  // Add matching description
   };
   ```

3. **Create Machine class** following the Machine interface:
   ```cpp
   class MyEngineMachine : public Machine {
   public:
       MyEngineMachine() { setName("myengine"); }
       
       void init() override;
       void reset() override;
       int32_t tick() override;
       void noteOn() override;
       void noteOff() override;
       void setI(int index, int value) override;
       void setF(int index, float value) override;
       int getI(int index) override;
       float getF(int index) override;
       void applyCC(int cc, float normalized, const std::string& paramName) override;
       
       int getKeyOn() const { return note_on_; }
       int getLastNote() const { return note_; }
       
   private:
       int note_on_ = 0;
       int note_ = -1;
       MyEngine* engine_ = nullptr;
   };
   ```

4. **Add to Makefile** in `test/Makefile`:
   ```makefile
   tmp/myengine_machine.o: ../machine/MyEngine/MyEngineMachine.cpp
   	$(CXX) $(CXXFLAGS) -c $< -o $@
   ```
   And add `tmp/myengine_machine.o` to the test_runner target.

### 4.2 Machine Interface Requirements

The Machine class must implement:
- `init()` - Initialize engine
- `reset()` - Reset state
- `tick()` - Generate one audio sample (returns int32_t)
- `noteOn()` / `noteOff()` - Trigger/release notes
- `setI(int, int)` / `getI(int)` - Integer parameter access
- `setF(int, float)` / `getF(int)` - Float parameter access
- `applyCC(int, float, string)` - Handle MIDI CC messages
- `getKeyOn()` / `getLastNote()` - For MIDI monitor display

---

## 5. FakeAudioDriver Status

### 5.1 Current Implementation

Located in `test/fake_audio_driver.h/cpp`:

**What exists:**
```cpp
class FakeAudioDriver {
public:
    bool initialize(int sampleRate = 48000, int frames = 1024);
    bool start();           // Starts PortAudio stream
    void stop();            // Stops stream
    void clear();           // Clears captured data
    bool hasAudio();        // Returns if audio captured
    float getAudioEnergy(); // Returns RMS energy
    uint64_t getSamplesCaptured();
    double getCaptureDuration();
    std::vector<float> getCapturedData();  // Float32 data
    std::vector<int32_t> getInt16Data();   // Int16 data
    
    // MIDI-like methods (not connected to Machine)
    void sendMidiNoteOn(int note, int velocity);
    void sendMidiNoteOff(int note);
    void sendCC(int cc, int value);
    bool setToneMode(bool mode);
    bool setToneFrequency(float freq);
};
```

**What's MISSING:**
- ❌ No connection to Machine's tick() method for audio capture
- ❌ No FFT computation (that's in fft_analyzer.cpp)
- ❌ No fundamental frequency detection
- ❌ No frequency verification
- ❌ No clipping detection (hasClipping method doesn't exist)
- ❌ No silence detection (isSilent method doesn't exist)

### 5.2 Actual Status

| Feature | Status | Notes |
|---------|--------|-------|
| PortAudio stream setup | ✓ Complete | Can start/stop in headless mode |
| Sine wave tone generation | ✓ Complete | Generates simple 440Hz tone |
| Float32 data storage | ✓ Partial | Stores in capturedData_ but never populated from Machine |
| Int16 conversion | ✓ Complete | Converts float to int16 |
| Audio energy calculation | ✓ Complete | getAudioEnergy() works |
| Samples captured counter | ✓ Complete | Increments but not used properly |
| FFT from Machine | ❌ Missing | Need to call Machine::tick() and store samples |
| Frequency verification | ❌ Missing | Need fft_analyzer integration |
| Clipping detection | ❌ Missing | Need peak level check |
| Silence detection | ❌ Missing | Need RMS threshold check |

### 5.3 What's Needed for Real Audio Capture

The FakeAudioDriver needs to be enhanced to capture audio from a Machine:

```cpp
// Enhanced FakeAudioDriver needed:
class FakeAudioDriver {
public:
    // NEW: Capture audio directly from Machine
    void captureFromMachine(Machine* machine, int numSamples) {
        for (int i = 0; i < numSamples; i++) {
            int32_t sample = machine->tick();
            float f = static_cast<float>(sample) / 640.0f;  // Convert from int32_t
            capturedData_.push_back(f);
        }
        samplesCaptured_.store(capturedData_.size());
    }
    
    // NEW: FFT analysis (delegates to FFTAnalyzer)
    std::vector<float> computeFFT(int fftSize = 1024) {
        return FFTAnalyzer::compute(capturedData_.data(), capturedData_.size(), fftSize);
    }
    
    // NEW: Fundamental frequency detection
    int findFundamentalFrequency() {
        std::vector<float> magnitudes;
        computeFFT(1024, magnitudes);
        return FFTAnalyzer::findFundamentalFrequency(magnitudes, sampleRate_);
    }
    
    // NEW: Clipping detection
    bool hasClipping() const {
        for (float s : capturedData_) {
            if (std::abs(s) > 0.999f) return true;
        }
        return false;
    }
    
    // NEW: Silence detection
    bool isSilent() const {
        return getAudioEnergy() < 0.001f;
    }
};
```

### 5.4 Current Workaround

Tests currently work by calling Machine methods directly without audio capture:
```cpp
// Current test approach (no real audio capture)
machine->init();
machine->noteOn();
machine->tick();  // Just generates one sample, not captured
printNotImplemented("sound");
return false;
```

## 6. FFTAnalyzer Status

### 6.1 Current Implementation

Located in `test/fft_analyzer.h/cpp`:

**What exists:**
```cpp
class FFTAnalyzer {
public:
    static void compute(const float* buffer, int size, std::vector<float>& magnitudes);
    static float findFundamentalFrequency(const std::vector<float>& magnitudes, float sampleRate);
    static float frequencyToMidi(float frequency);
    static float midiToFrequency(int midiNote);
    static bool isSilent(const std::vector<float>& magnitudes, float threshold = 0.001f);
    static float getMagnitudeAtBin(const std::vector<float>& magnitudes, int bin);
    static float getBinFrequency(const std::vector<float>& magnitudes, int bin, float sampleRate);
    static int getBinAtFrequency(float frequency, float sampleRate, int bufferSize);
};
```

### 6.2 Actual Status

| Feature | Status | Notes |
|---------|--------|-------|
| FFT computation | ✓ Complete | Cooley-Tukey algorithm |
| Magnitude spectrum | ✓ Complete | Returns normalized magnitudes |
| Fundamental frequency detection | ✓ Complete | Peak detection with harmonic check |
| Frequency ↔ MIDI conversion | ✓ Complete | Bidirectional |
| Silence detection | ✓ Complete | Magnitude threshold |
| Bin utilities | ✓ Complete | Frequency/bin conversion |

### 6.3 Usage Example

```cpp
// Capture audio from machine
std::vector<float> audio;
for (int i = 0; i < 1024; i++) {
    int32_t sample = machine->tick();
    audio.push_back(static_cast<float>(sample) / 640.0f);
}

// Compute FFT
std::vector<float> magnitudes;
FFTAnalyzer::compute(audio.data(), audio.size(), magnitudes);

// Find fundamental frequency
float freq = FFTAnalyzer::findFundamentalFrequency(magnitudes, 48000.0f);
float midiNote = FFTAnalyzer::frequencyToMidi(freq);

// Verify (e.g., C4 = 60)
if (std::abs(midiNote - 60.0f) <= 1.0f) {
    // Note verified
}
```

---

## 7. MIDI Test Status

### 7.1 Current Implementation

Located in `test/midi_simulator.cpp`:

**What exists:**
```cpp
class MidiSimulator {
public:
    bool initialize();
    void sendNoteOn(int note, int velocity);
    void sendNoteOff(int note, int velocity);
    void sendCC(int cc, int value);
    void sendProgramChange(int prog, int channel);
    void sendPitchBend(int value);
    void sendMessages(const std::vector<MidiMessage>& messages);
    void sendSequential(const std::vector<MidiMessage>& messages, int delayUs);
    void sendAsync(const std::vector<MidiMessage>& messages);
    const std::vector<MidiMessage>& getSentMessages() const;
    void clear();
};
```

**What's MISSING:**
- ❌ Does NOT forward messages to Machine - only records them
- ❌ Tests use Machine methods directly instead of MidiSimulator

### 7.2 Actual Status

| Feature | Status | Notes |
|---------|--------|-------|
| Note On/Off messages | ✓ Complete | Records to sentMessages_ |
| CC messages | ✓ Complete | Records to sentMessages_ |
| Program Change | ✓ Complete | Records to sentMessages_ |
| Pitch Bend | ✓ Complete | Records to sentMessages_ |
| Message history | ✓ Complete | getSentMessages() |
| Forward to Machine | ❌ Missing | Not connected to Machine |

### 7.3 Current Usage in Tests

Tests currently call Machine methods directly:
```cpp
// Current test approach
machine->noteOn();              // Instead of MidiSimulator::sendNoteOn()
machine->noteOff();             // Instead of MidiSimulator::sendNoteOff()
machine->applyCC(7, 0.5, "VOLUME");  // Instead of MidiSimulator::sendCC()
```

### 7.4 What's Needed for Real MIDI Testing

To use MidiSimulator properly:
```cpp
// Need to connect MidiSimulator to Machine
MidiSimulator midi;
midi.sendNoteOn(60, 100);  // C4, velocity 100

// Then forward to machine
for (const auto& msg : midi.getSentMessages()) {
    if (msg.status == 0x90) {  // Note On
        machine->setI(NOTE_ON, 1);
        machine->setI(NOTE1, msg.data1);
    } else if (msg.status == 0xB0) {  // CC
        machine->applyCC(msg.data1, msg.data2 / 127.0f, "");
    }
}
```

---

## 7. FFT Analyzer Status

### 7.1 Current Implementation

Located in `test/fft_analyzer.h/cpp`:

```cpp
class FFTAnalyzer {
public:
    static void compute(const float* audio, int size, std::vector<float>& magnitudes);
    static float findFundamentalFrequency(const std::vector<float>& magnitudes, float sampleRate);
    static float frequencyToMidi(float frequency);
    static float midiToFrequency(int midiNote);
    static bool verifyNote(int actualNote, int expectedNote, int tolerance = 1);
};
```

### 7.2 Status

| Feature | Status | Notes |
|---------|--------|-------|
| FFT computation | ✓ Complete | 1024-point FFT |
| Magnitude spectrum | ✓ Complete | Returns magnitudes |
| Fundamental frequency | ✓ Complete | Returns Hz |
| Frequency ↔ MIDI | ✓ Complete | Bidirectional |
| Note verification | ✓ Complete | ±1 semitone tolerance |

---

## 8. Test Implementation Roadmap

### Priority 1: Basic Sound Tests

- [ ] **sound** - Verify machine produces audio when note triggered
- [ ] **note_on_off** - Verify noteOn/noteOff work correctly

### Priority 2: Audio Quality Tests

- [ ] **silence** - Verify volume=0 produces silent output
- [ ] **no_clip** - Verify max volume doesn't clip

### Priority 3: Frequency Tests

- [ ] **octave** - Verify note frequencies match MIDI note values
- [ ] FFT verification - Use FFTAnalyzer to verify pitch accuracy

### Priority 4: Advanced Tests

- [ ] **cc_control** - Verify CC messages affect parameters
- [ ] **polyphony** - Verify multi-voice handling (for polyphonic engines)
- [ ] **filter_env** - Verify filter envelope affects sound

---

## 9. Build and Run

### Build
```bash
cd /home/ollama/build/ncursesynth/test
make test_runner
```

### Run
```bash
# Show help
./test_runner

# List engines/tests
./test_runner --list-engines
./test_runner --list-tests

# Run tests
./test_runner --engine ncursesynth --tests all
./test_runner --engine pbsynth --tests sound,silence
```

---

## 10. Notes

1. **NOTIMPLEMENTED**: Tests that don't perform real verification show `[NOTIMPLEMENTED]` and FAIL
2. **Engine/Test Lists**: Defined as global variables at top of `test_runner.cpp` for easy updates
3. **Machine Interface**: Must implement all Machine virtual methods for integration
4. **Test Isolation**: Each test should be independent and not rely on previous test state
5. **FFT Resolution**: 1024-point FFT at 48kHz gives ~46.88 Hz resolution (sufficient for note verification)

---

## 11. Future Enhancements

- [ ] Real FFT-based frequency verification in tests
- [ ] Audio playback for manual verification
- [ ] Waveform visualization in console
- [ ] HTML report generation
- [ ] CI/CD integration
- [ ] Test coverage reporting
- [ ] Performance benchmarking

---

*This plan documents the testing infrastructure for synth engine integration into virtual_synth. Update as implementation progresses.*