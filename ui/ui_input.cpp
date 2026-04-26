#include "ui_input.h"
#include "ui_parameters.h"
#include "ui_constants.h"
#include <ncurses.h>
#include <map>
#include <chrono>
#include <vector>
#include <cmath>

// Static global variables for tracking key states
static std::map<int, bool> activeKeys;
static std::map<int, std::chrono::steady_clock::time_point> keyPressTimes;

void UIInput::panicButton(SynthArchitecture* synth) {
    // Kill all notes immediately without release
    synth->allNotesOff();
    activeKeys.clear();
    keyPressTimes.clear();
}

void UIInput::releaseVoice(int voiceNumber, SynthArchitecture* synth) {
    const auto& voices = synth->getVoices();
    if (voiceNumber >= 1 && voiceNumber <= static_cast<int>(voices.size())) {
        // Force note off on this specific voice
        voices[voiceNumber - 1]->noteOff();
        
        // Also clear any matching active keys
        float freq = voices[voiceNumber - 1]->getFrequency();
        for (int i = 0; i < 12; i++) {
            if (std::abs(freq - NOTE_FREQUENCIES[i]) < 1.0f) {
                for (auto& pair : activeKeys) {
                    if (pair.first == KEY_BINDINGS[i]) {
                        activeKeys[pair.first] = false;
                        break;
                    }
                }
                break;
            }
        }
    }
}

void UIInput::handleNavigation(int ch, Parameter& selectedParam) {
    int currentIdx = static_cast<int>(selectedParam);
    int maxIdx = static_cast<int>(Parameter::COUNT) - 1;
    
    // Simple linear navigation following visual order on screen:
    // - DOWN: next parameter
    // - UP: previous parameter
    // - LEFT/RIGHT: wrap around
    
    if (ch == KEY_DOWN) {
        // Move to next parameter
        currentIdx = (currentIdx + 1) % (maxIdx + 1);
    } else if (ch == KEY_UP) {
        // Move to previous parameter
        currentIdx = (currentIdx - 1 + maxIdx + 1) % (maxIdx + 1);
    } else if (ch == KEY_RIGHT) {
        // Move forward
        currentIdx = (currentIdx + 1) % (maxIdx + 1);
    } else if (ch == KEY_LEFT) {
        // Move backward
        currentIdx = (currentIdx - 1 + maxIdx + 1) % (maxIdx + 1);
    }
    
    selectedParam = static_cast<Parameter>(currentIdx);
}

void UIInput::handleNoteInput(int ch, SynthArchitecture* synth) {
    // Find which note this key corresponds to
    int noteIndex = -1;
    for (int i = 0; i < 12; i++) {
        if (ch == KEY_BINDINGS[i]) {
            noteIndex = i;
            break;
        }
    }
    
    if (noteIndex == -1) return;
    
    // Toggle note on/off
    if (activeKeys[ch]) {
        // Key is already pressed - turn it off
        activeKeys[ch] = false;
        synth->noteOff(NOTE_FREQUENCIES[noteIndex]);
    } else {
        // Key not pressed - turn it on
        activeKeys[ch] = true;
        keyPressTimes[ch] = std::chrono::steady_clock::now();
        synth->noteOn(NOTE_FREQUENCIES[noteIndex]);
    }
}

void UIInput::handleNoteRelease(int ch, SynthArchitecture* synth) {
    int noteIndex = -1;
    for (int i = 0; i < 12; i++) {
        if (ch == KEY_BINDINGS[i]) {
            noteIndex = i;
            break;
        }
    }
    
    if (noteIndex == -1) return;
    
    if (activeKeys[ch]) {
        activeKeys[ch] = false;
        synth->noteOff(NOTE_FREQUENCIES[noteIndex]);
    }
}

void UIInput::clearAllNotes(SynthArchitecture* synth) {
    for (int i = 0; i < 12; i++) {
        if (activeKeys[KEY_BINDINGS[i]]) {
            synth->noteOff(NOTE_FREQUENCIES[i]);
            activeKeys[KEY_BINDINGS[i]] = false;
        }
    }
    keyPressTimes.clear();
}

void UIInput::checkKeyReleases(SynthArchitecture* synth) {
    auto now = std::chrono::steady_clock::now();
    std::vector<int> toRelease;
    
    // Check for keys held longer than 10 seconds (auto-release as safety)
    for (auto& pair : keyPressTimes) {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - pair.second);
        if (duration.count() > 10000) { // Auto-release after 10 seconds
            toRelease.push_back(pair.first);
        }
    }
    
    for (int ch : toRelease) {
        handleNoteRelease(ch, synth);
    }
}
