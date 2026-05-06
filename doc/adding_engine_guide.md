# Adding a New Synth Engine

This guide describes how to integrate a new synth engine into ncursesynth, including creating the Machine wrapper, setting up polyphony, adding MIDI support, and connecting the UI.

## Overview

ncursesynth uses a plugin-based architecture called the **Machine** system. Each synth engine is wrapped in a class that inherits from the `Machine` base class:

```cpp
#include "machine/Machine.h"

class NewEngineMachine : public Machine {
    // ... implementation
};
```

## Step 1: Create Engine Types Header

Create `machine/NewEngine/engine_types.h` with necessary type definitions:

```cpp
#ifndef ENGINE_TYPES_H
#define ENGINE_TYPES_H

#include <cstdint>

// Basic types
#define Sint32 int32_t
#define Sint16 int16_t
#define Uint32 uint32_t
#define Uint16 uint16_t

// Debug macro
#define DPRINTF(...)

// Default sample rate
#ifndef DEFAULTFREQ
#define DEFAULTFREQ 48000
#endif

// Parameter IDs (match PBSynth/Cursynth conventions)
#define NOTE_ON 150
#define NOTE1 70

#define OSC1_TYPE 20
#define OSC2_TYPE 21
#define OSC1_SCALE 65
#define OSC2_SCALE 66
#define OSC1_DETUNE 74
#define OSC2_DETUNE 75

#define FILTER1_TYPE 50
#define FILTER1_CUTOFF 52
#define FILTER1_RESONANCE 53

#define LFO1_WAVEFORM 2
#define LFO1_FREQ 42

#define ADSR_ENV0_ATTACK 0
#define ADSR_ENV0_DECAY 1
#define ADSR_ENV0_SUSTAIN 2
#define ADSR_ENV0_RELEASE 3

#define ADSR_ENV1_ATTACK 4
#define ADSR_ENV1_DECAY 5
#define ADSR_ENV1_SUSTAIN 6
#define ADSR_ENV1_RELEASE 7

#define OSC12_MIX 28
#define VELOCITY 140
#define AMP 35
#define ENV1_DEPTH 94

#define TRIG_TIME_MODE 160
#define TRIG_TIME_DURATION 161

#endif
```

## Step 2: Create the Machine Header

Create `machine/NewEngine/NewEngineMachine.h`:

```cpp
#include "../Machine.h"
#include "engine_engine.h"  // The actual synth engine
#include "engine_types.h"

class NewEngineMachine : public Machine {
public:
    NewEngineMachine();
    ~NewEngineMachine();

    void init() override;
    void reset() override;
    Sint32 tick() override;

    void setI(int what, int val) override;
    void setF(int what, float val) override;
    int getI(int what) override;
    int checkI(int what, int val) override;

    const char* getMachineParamCharStar(int machineParam, int paramValue);

    // MIDI monitor accessors
    int getLastNote() const { return note; }
    int getKeyOn() const { return note_on; }

    // CC mapping
    void applyCC(int cc, float normalized, const std::string& paramName) override;

protected:
    Engine* engine;
    // ... your state variables
};
```

## Step 3: Create the Machine Implementation

Create `machine/NewEngine/NewEngineMachine.cpp`:

```cpp
#include <iostream>
#include "engine_types.h"
#include "NewEngineMachine.h"

#define SAM 64

NewEngineMachine::NewEngineMachine() {
    setName("NewEngine");  // Must set name!
    // Initialize state variables
}

NewEngineMachine::~NewEngineMachine() {
    delete engine;
    free(buffer_i);
    free(buffer_f);
}

void NewEngineMachine::init() {
    // Allocate buffers
    buffer_f = new float[SAM];
    buffer_i = new Sint16[SAM];
    
    // Create engine if needed
    if (engine == nullptr)
        engine = new Engine();
    
    engine->setBufferSize(SAM);
    engine->setSampleRate(DEFAULTFREQ);
    
    // Set default controls
    auto controls = engine->getControls();
    if (controls.count("volume"))
        controls.at("volume")->set(0.5f);
}

Sint32 NewEngineMachine::tick() {
    if (engine == nullptr) return 0;
    
    if (index >= SAM || index < 0)
        index = 0;
    
    if (index == 0) {
        engine->process();
        for (int i = 0; i < SAM; i++) {
            buffer_f[i] = engine->output()->buffer[i] * 8192;
            buffer_i[i] = (Sint16)buffer_f[i];
        }
    }
    
    Sint32 s = buffer_i[index];
    if (s > 32000) s = 32000;
    if (s < -32000) s = -32000;
    
    index++;
    return s;
}

void NewEngineMachine::setI(int what, int val) {
    if (engine == nullptr) return;
    
    float f_val = (float)val / 128.0f;
    auto controls = engine->getControls();
    
    // NOTE HANDLING
    if (what == NOTE_ON && val == 1) {
        engine->noteOn(note, velocity / 127.0f);
        note_on = 1;
    }
    
    if (what == NOTE_ON && val == 0) {
        engine->noteOff(note);
        note_on = 0;
    }
    
    // PARAMETER HANDLING - MUST call engine controls!
    if (what == FILTER1_CUTOFF) {
        if (controls.count("cutoff"))
            controls.at("cutoff")->set(28 + f_val * 100);
    }
    if (what == FILTER1_RESONANCE) {
        if (controls.count("resonance"))
            controls.at("resonance")->set(f_val * 10);
    }
    if (what == ADSR_ENV0_ATTACK) {
        if (controls.count("amp attack"))
            controls.at("amp attack")->set(f_val * 3);
    }
    // ... other parameters
}

void NewEngineMachine::applyCC(int cc, float normalized, const std::string& paramName) {
    int val = (int)(normalized * 127.0f);
    
    if (paramName == "CUTOFF") {
        setI(FILTER1_CUTOFF, val);
    } else if (paramName == "RESONANCE") {
        setI(FILTER1_RESONANCE, val);
    } else if (paramName == "FILTER_ENV_AMOUNT") {
        setI(ENV1_DEPTH, val);
    } else if (paramName == "AMP_ATTACK") {
        setI(ADSR_ENV0_ATTACK, val);
    } else if (paramName == "AMP_DECAY") {
        setI(ADSR_ENV0_DECAY, val);
    } else if (paramName == "AMP_SUSTAIN") {
        setI(ADSR_ENV0_SUSTAIN, val);
    } else if (paramName == "AMP_RELEASE") {
        setI(ADSR_ENV0_RELEASE, val);
    } else if (paramName == "FILTER_ATTACK") {
        setI(ADSR_ENV1_ATTACK, val);
    } else if (paramName == "FILTER_DECAY") {
        setI(ADSR_ENV1_DECAY, val);
    } else if (paramName == "FILTER_SUSTAIN") {
        setI(ADSR_ENV1_SUSTAIN, val);
    } else if (paramName == "FILTER_RELEASE") {
        setI(ADSR_ENV1_RELEASE, val);
    } else if (paramName == "VOLUME") {
        setI(AMP, val);
    }
}

int NewEngineMachine::getI(int what) {
    if (engine == nullptr) return 0;
    
    auto controls = engine->getControls();
    
    if (what == NOTE_ON) return note_on;
    if (what == NOTE1) return note;
    
    if (what == FILTER1_CUTOFF && controls.count("cutoff")) {
        float v = controls.at("cutoff")->current_value();
        return (int)((v - 28) / 100.0f * 127);
    }
    // ... other parameters
    return 0;
}
```

## Step 4: Fix Engine Header Includes

If your engine has non-standard includes, create compatibility headers:

```cpp
// machine/NewEngine/Master.h
#ifndef MASTER_H
#define MASTER_H
#include <cstdlib>
#define malloc __builtin_malloc
#define free __builtin_free
#endif
```

```cpp
// machine/NewEngine/engine_types.h - Fix include paths
#include "Machine.h" -> #include "../Machine.h"
#include "SomeHeader.h" -> // Check if header exists or create stub
```

## Step 5: Add to Makefile

Add all `.cpp` files from your engine directory (except any standalone `main.cpp` or SDL-dependent files):

```makefile
machine/NewEngine/file1.cpp \
machine/NewEngine/file2.cpp \
# ... all cpp files
```

## Step 6: Register in main.cpp

```cpp
#include "machine/NewEngine/NewEngineMachine.h"

// Register engine
machineManager.registerMachine(new NewEngineMachine());

// MIDI forwarding
NewEngineMachine* newengineMachine = dynamic_cast<NewEngineMachine*>(activeMachine);
if (newengineMachine) {
    midiInput.setMachine(newengineMachine);
    midiInput.setMappingMachine(newengineMachine);
}

// UI selection
bool useNewEngineUI = dynamic_cast<NewEngineMachine*>(activeMachine) != nullptr;

// In UI loop - MIDI monitor
if (newengineMachine && newengineMachine->getKeyOn()) {
    pui.setMidiNote(newengineMachine->getLastNote(), 127);
}
```

## Finding Engine Control Names

To find what control names your engine supports:

1. Search for `createPolyModControl` or similar in the engine code:
   ```bash
   grep -r "createPolyModControl" machine/EngineDir/
   ```

2. Check for control definitions:
   ```bash
   grep -r "\"cutoff\"" machine/EngineDir/
   ```

3. Common control names across engines:
   - `"cutoff"`, `"resonance"` - Filter
   - `"amp attack"`, `"amp decay"`, `"amp sustain"`, `"amp release"` - Amp envelope
   - `"filter attack"`, `"filter decay"`, `"filter sustain"`, `"filter release"` - Filter envelope
   - `"volume"` - Master volume
   - `"osc 1 waveform"`, `"osc 2 waveform"` - Oscillator waveforms
   - `"osc mix"` - Oscillator mix

## Summary Checklist

- [ ] Create `engine_types.h` with parameter IDs and types
- [ ] Create compatibility headers (Master.h, fix include paths)
- [ ] Implement Machine with: `setName()`, `getKeyOn()`, `getLastNote()`, `applyCC()`
- [ ] In `setI()`: call `engine->getControls().at("name")->set(value)`
- [ ] Add all source files to Makefile
- [ ] Register in main.cpp with MIDI forwarding
- [ ] Test with `--midi-debug` to verify CC mapping
- [ ] (Optional) Add preset support - see preset system below

## Preset System (Optional)

The current preset system is designed for the ncursesynth original engine only. Presets are stored in:
- `bank/ncursesynth/index.txt` - Maps preset names to filenames
- `bank/ncursesynth/*.txt` - Individual preset files

To add preset support for a new engine, you would need to extend the preset system or create a separate one. Preset format:
```
polyphony=8
filter_type=0
cutoff=3500
resonance=0.15
waveform=1
osc_mix=0.35
amp_attack=0.005
amp_decay=0.25
amp_sustain=0.6
amp_release=0.35
volume=0.55
```