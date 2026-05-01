#include "ui_draw.h"
#include "ui_parameters.h"
#include "ui_layout.h"
#include "ui_colors.h"
#include "ui_constants.h"
#include "midi/midi_input.h"
#include "midi/midi_mapping.h"
#include "../preset/preset_manager.h"
#include <ncurses.h>



void UIDraw::drawMidiSection(SynthArchitecture* synth, Parameter selectedParam) {
    (void)selectedParam;
    int row = UILayout::MIDI_SECTION_ROW;
    int col = UILayout::RIGHT_COL;

    attron(A_BOLD);
    mvprintw(row, col, "=== MIDI ===");
    attroff(A_BOLD);
    row++;

    MidiInput* midi = synth->getMidiInput();
    if (midi) {
        mvprintw(row, col, "%-18s: %s", "ENABLE",
                 UIParameters::getBoolText(midi->isRunning()));
        row++;

        int currentPort = midi->getSelectedPort();
        if (currentPort >= 0 && currentPort < midi->getDeviceCount()) {
            mvprintw(row, col, "%-18s: %s", "DEVICE",
                     midi->getDeviceName(currentPort).c_str());
        } else {
            mvprintw(row, col, "%-18s: %s", "DEVICE", "NONE");
        }
        row++;

        MappingManager* mm = midi->getMappingManager();
        if (mm && mm->getMappingCount() > 0) {
            mvprintw(row, col, "%-18s: %s", "MAPPING",
                     mm->getMappingName(mm->getCurrentMappingIndex()).c_str());
        } else {
            mvprintw(row, col, "%-18s: %s", "MAPPING", "NONE");
        }
        row++;

        if (midi->getDeviceCount() > 0) {
            mvprintw(row, col, "%-18s: %d devices", "AVAILABLE", midi->getDeviceCount());
        }
        row++;

        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - midi->getLastActivity()).count();

        if (elapsed < 2000 && midi->getLastCC() >= 0) {
            int lastCC = midi->getLastCC();
            int lastVal = midi->getLastCCValue();

            if (lastCC < 128) {
                mvprintw(row, col, "%-18s: CC%3d = %3d", "MONITOR", lastCC, lastVal);
            } else if (lastCC < 256) {
                mvprintw(row, col, "%-18s: Note%3d ON  %3d", "MONITOR", lastCC - 128, lastVal);
            } else {
                mvprintw(row, col, "%-18s: Note%3d OFF %3d", "MONITOR", lastCC - 256, lastVal);
            }
        }
    } else {
        mvprintw(row, col, "%-18s: %s", "MIDI", "NOT AVAILABLE");
    }
}


void UIDraw::drawVoiceDisplay(SynthArchitecture* synth) {
    int startRow = UILayout::VOICE_START_ROW;
    int col = UILayout::LEFT_COL;

    attron(A_BOLD);
    mvprintw(startRow, col, "=== VOICE MONITOR ===");
    attroff(A_BOLD);
    startRow++;

    const auto& voices = synth->getVoices();
    int polyphony = synth->getPolyphony();

    for (int i = 0; i < polyphony; i++) {
        const auto& voice = voices[i];
        bool isActive = voice->isActive();
        float freq = voice->getFrequency();
        float envelopeLevel = voice->getEnvelopeLevel();

        const char* noteName = "---";
        for (int n = 0; n < 12; n++) {
            if (std::abs(freq - NOTE_FREQUENCIES[n]) < 1.0f) {
                noteName = NOTE_NAMES[n];
                break;
            }
        }

        if (isActive) {
            attron(COLOR_PAIR(UIColor::STATUS_OK));
            attron(A_BOLD);
        } else {
            attron(COLOR_PAIR(UIColor::SELECTED));
        }

        mvprintw(startRow + i, col, "Voice %d: ", i + 1);

        if (isActive) {
            printw("[ACTIVE] ");
            attron(COLOR_PAIR(UIColor::STATUS_OK));
            printw("%s", noteName);
            attroff(COLOR_PAIR(UIColor::STATUS_OK));
            printw(" %5.1fHz", freq);
            int envBar = static_cast<int>(envelopeLevel * 10);
            printw(" [");
            for (int j = 0; j < envBar; j++) addch('|');
            for (int j = envBar; j < 10; j++) addch(' ');
            printw("]");
        } else {
            printw("[INACTIVE]");
        }
        
        if (isActive) {
            attroff(A_BOLD);
        }
    }

    int activeCount = 0;
    for (int i = 0; i < polyphony; i++) {
        if (voices[i]->isActive()) activeCount++;
    }
    attron(A_BOLD);
    mvprintw(startRow, col, "Active Voices: %d / %d", activeCount, polyphony);
    attroff(A_BOLD);
}

void UIDraw::drawTitle() {
    attron(A_BOLD);
    mvprintw(UILayout::TITLE_ROW, 2, "=== POLYPHONIC VIRTUAL ANALOG SYNTHESIZER ===");
    attroff(A_BOLD);
    mvprintw(UILayout::SUBTITLE_ROW, 2, "Dual Envelope | 4 Effects | Polyphonic");
    mvprintw(UILayout::DIVIDER_ROW, 2, "===============================================");
}

void UIDraw::drawStatus(SynthArchitecture* synth) {
    attron(A_REVERSE);
    if (synth->isPlaying()) {
        mvprintw(UILayout::STATUS_ROW, 2, " NOTES PLAYING - ENVELOPES ACTIVE    ");
    } else {
        mvprintw(UILayout::STATUS_ROW, 2, "      NO NOTES PLAYING              ");
    }
    attroff(A_REVERSE);
}


void UIDraw::drawPresetSection(SynthArchitecture* synth, Parameter selectedParam) {
    int row = UILayout::PRESET_SECTION_ROW;
    int col = UILayout::RIGHT_COL;
    
    attron(A_BOLD);
    mvprintw(row, col, "=== PRESET ===");
    attroff(A_BOLD);
    row++;
    
    PresetManager* pm = synth->getPresetManager();
    
    if (selectedParam == Parameter::PRESET_LOAD) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    if (pm && pm->exists()) {
        mvprintw(row, col, "%-18s: %s", "LOAD", pm->getPresetName(pm->getCurrentPresetIndex()).c_str());
    } else {
        mvprintw(row, col, "%-18s: %s", "LOAD", "NONE");
    }
    if (selectedParam == Parameter::PRESET_LOAD) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
    row++;
    
    if (selectedParam == Parameter::PRESET_SAVE) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-18s: %s", "SAVE", "current");
    if (selectedParam == Parameter::PRESET_SAVE) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
}

void UIDraw::drawDelaySection(SynthArchitecture* synth, Parameter selectedParam) {
    int row = UILayout::DELAY_ROW;
    int col = UILayout::RIGHT_COL;

    attron(A_BOLD);
    attron(COLOR_PAIR(UIColor::SECTION_HEADER));
    mvprintw(row, col, "=== DELAY ===");
    attroff(COLOR_PAIR(UIColor::SECTION_HEADER));
    attroff(A_BOLD);
    row++;

    if (selectedParam == Parameter::DELAY_ENABLE) {
        attron(A_REVERSE);
    }
    mvprintw(row, col, "%-14s: %-4s", "ENABLE", UIParameters::getBoolText(synth->getDelay()->isEnabled()));
    if (selectedParam == Parameter::DELAY_ENABLE) {
        attroff(A_REVERSE);
    }
    row++;

    if (selectedParam == Parameter::DELAY_TIME) {
        attron(A_REVERSE);
    }
    mvprintw(row, col, "%-14s: %5.0fms", "TIME", synth->getDelay()->getDelayTime());
    if (selectedParam == Parameter::DELAY_TIME) {
        attroff(A_REVERSE);
    }
    row++;

    if (selectedParam == Parameter::DELAY_FEEDBACK) {
        attron(A_REVERSE);
    }
    float fb = synth->getDelay()->getFeedback();
    int fbBar = static_cast<int>(fb * 16);
    mvprintw(row, col, "%-14s: [", "FB");
    for (int i = 0; i < fbBar; i++) addch('=');
    for (int i = fbBar; i < 16; i++) addch(' ');
    printw("] %4.2f", fb);
    if (selectedParam == Parameter::DELAY_FEEDBACK) {
        attroff(A_REVERSE);
    }
    row++;

    if (selectedParam == Parameter::DELAY_MIX) {
        attron(A_REVERSE);
    }
    float mix = synth->getDelay()->getMix();
    int mixBar = static_cast<int>(mix * 16);
    mvprintw(row, col, "%-14s: [", "MIX");
    for (int i = 0; i < mixBar; i++) addch('=');
    for (int i = mixBar; i < 16; i++) addch(' ');
    printw("] %4.2f", mix);
    if (selectedParam == Parameter::DELAY_MIX) {
        attroff(A_REVERSE);
    }
}

void UIDraw::drawReverbSection(SynthArchitecture* synth, Parameter selectedParam) {
    int row = UILayout::REVERB_ROW;
    int col = UILayout::RIGHT_COL;

    attron(A_BOLD);
    attron(COLOR_PAIR(UIColor::SECTION_HEADER));
    mvprintw(row, col, "=== REVERB ===");
    attroff(COLOR_PAIR(UIColor::SECTION_HEADER));
    attroff(A_BOLD);
    row++;

    if (selectedParam == Parameter::REVERB_ENABLE) {
        attron(A_REVERSE);
    }
    mvprintw(row, col, "%-14s: %-4s", "ENABLE", UIParameters::getBoolText(synth->getReverb()->isEnabled()));
    if (selectedParam == Parameter::REVERB_ENABLE) {
        attroff(A_REVERSE);
    }
    row++;

    if (selectedParam == Parameter::REVERB_DECAY) {
        attron(A_REVERSE);
    }
    float dec = synth->getReverb()->getDecay();
    int decBar = static_cast<int>(dec * 16);
    mvprintw(row, col, "%-14s: [", "DECAY");
    for (int i = 0; i < decBar; i++) addch('=');
    for (int i = decBar; i < 16; i++) addch(' ');
    printw("] %4.2f", dec);
    if (selectedParam == Parameter::REVERB_DECAY) {
        attroff(A_REVERSE);
    }
    row++;

    if (selectedParam == Parameter::REVERB_MIX) {
        attron(A_REVERSE);
    }
    float mix = synth->getReverb()->getMix();
    int mixBar = static_cast<int>(mix * 16);
    mvprintw(row, col, "%-14s: [", "MIX");
    for (int i = 0; i < mixBar; i++) addch('=');
    for (int i = mixBar; i < 16; i++) addch(' ');
    printw("] %4.2f", mix);
    if (selectedParam == Parameter::REVERB_MIX) {
        attroff(A_REVERSE);
    }
}

void UIDraw::drawChorusSection(SynthArchitecture* synth, Parameter selectedParam) {
    int row = UILayout::CHORUS_ROW;
    int col = UILayout::RIGHT_COL;

    attron(A_BOLD);
    attron(COLOR_PAIR(UIColor::SECTION_HEADER));
    mvprintw(row, col, "=== CHORUS ===");
    attroff(COLOR_PAIR(UIColor::SECTION_HEADER));
    attroff(A_BOLD);
    row++;

    if (selectedParam == Parameter::CHORUS_ENABLE) {
        attron(A_REVERSE);
    }
    mvprintw(row, col, "%-14s: %-4s", "ENABLE", UIParameters::getBoolText(synth->getChorus()->isEnabled()));
    if (selectedParam == Parameter::CHORUS_ENABLE) {
        attroff(A_REVERSE);
    }
    row++;

    if (selectedParam == Parameter::CHORUS_DEPTH) {
        attron(A_REVERSE);
    }
    float depth = synth->getChorus()->getDepth();
    int depthBar = static_cast<int>(depth * 16);
    mvprintw(row, col, "%-14s: [", "DEPTH");
    for (int i = 0; i < depthBar; i++) addch('=');
    for (int i = depthBar; i < 16; i++) addch(' ');
    printw("] %4.2f", depth);
    if (selectedParam == Parameter::CHORUS_DEPTH) {
        attroff(A_REVERSE);
    }
    row++;

    if (selectedParam == Parameter::CHORUS_RATE) {
        attron(A_REVERSE);
    }
    mvprintw(row, col, "%-14s: %4.1fHz", "RATE", synth->getChorus()->getRate());
    if (selectedParam == Parameter::CHORUS_RATE) {
        attroff(A_REVERSE);
    }
    row++;

    if (selectedParam == Parameter::CHORUS_MIX) {
        attron(A_REVERSE);
    }
    float mix = synth->getChorus()->getMix();
    int mixBar = static_cast<int>(mix * 16);
    mvprintw(row, col, "%-14s: [", "MIX");
    for (int i = 0; i < mixBar; i++) addch('=');
    for (int i = mixBar; i < 16; i++) addch(' ');
    printw("] %4.2f", mix);
    if (selectedParam == Parameter::CHORUS_MIX) {
        attroff(A_REVERSE);
    }
}

void UIDraw::drawDistortionSection(SynthArchitecture* synth, Parameter selectedParam) {
    int row = UILayout::DISTORTION_ROW;
    int col = UILayout::RIGHT_COL;

    attron(A_BOLD);
    attron(COLOR_PAIR(UIColor::SECTION_HEADER));
    mvprintw(row, col, "=== DISTORTION ===");
    attroff(COLOR_PAIR(UIColor::SECTION_HEADER));
    attroff(A_BOLD);
    row++;

    if (selectedParam == Parameter::DISTORTION_ENABLE) {
        attron(A_REVERSE);
    }
    mvprintw(row, col, "%-14s: %-4s", "ENABLE", UIParameters::getBoolText(synth->getDistortion()->isEnabled()));
    if (selectedParam == Parameter::DISTORTION_ENABLE) {
        attroff(A_REVERSE);
    }
    row++;

    if (selectedParam == Parameter::DISTORTION_DRIVE) {
        attron(A_REVERSE);
    }
    float drive = synth->getDistortion()->getDrive();
    int driveBar = static_cast<int>(drive * 16);
    mvprintw(row, col, "%-14s: [", "DRIVE");
    for (int i = 0; i < driveBar; i++) addch('=');
    for (int i = driveBar; i < 16; i++) addch(' ');
    printw("] %4.2f", drive);
    if (selectedParam == Parameter::DISTORTION_DRIVE) {
        attroff(A_REVERSE);
    }
    row++;

    if (selectedParam == Parameter::DISTORTION_MIX) {
        attron(A_REVERSE);
    }
    float mix = synth->getDistortion()->getMix();
    int mixBar = static_cast<int>(mix * 16);
    mvprintw(row, col, "%-14s: [", "MIX");
    for (int i = 0; i < mixBar; i++) addch('=');
    for (int i = mixBar; i < 16; i++) addch(' ');
    printw("] %4.2f", mix);
    if (selectedParam == Parameter::DISTORTION_MIX) {
        attroff(A_REVERSE);
    }
}

void UIDraw::drawMasterSection(SynthArchitecture* synth, Parameter selectedParam) {
    int row = UILayout::MASTER_ROW;
    int col = UILayout::RIGHT_COL;

    attron(A_BOLD);
    attron(COLOR_PAIR(UIColor::SECTION_HEADER));
    mvprintw(row, col, "=== MASTER ===");
    attroff(COLOR_PAIR(UIColor::SECTION_HEADER));
    attroff(A_BOLD);
    row++;

    float volume = synth->getVolume();
    if (selectedParam == Parameter::MASTER_VOLUME) {
        attron(A_REVERSE);
    }
    int volBar = static_cast<int>(volume * 16);
    mvprintw(row, col, "%-14s: [", "VOLUME");
    for (int i = 0; i < volBar; i++) addch('=');
    for (int i = volBar; i < 16; i++) addch(' ');
    printw("] %3d%%", static_cast<int>(volume * 100));
    if (selectedParam == Parameter::MASTER_VOLUME) {
        attroff(A_REVERSE);
    }
}

void UIDraw::drawKeyboard() {
    int row = UILayout::KEYBOARD_ROW;
    
    mvprintw(row, 2, "KEYBOARD LAYOUT (C4 to B4):");
    row++;
    mvprintw(row, 2, "  A   Z   E   R   T   Y   U   I   O   P   ^   $");
    row++;
    mvprintw(row, 2, "  C   C#  D   D#  E   F   F#  G   G#  A   A#  B");
    
    row += 2;
    mvprintw(row, 2, "Press ESC to stop all playing notes");
}


void UIDraw::drawControls() {
    int row = UILayout::CONTROLS_ROW;

    attron(A_BOLD);
    mvprintw(row, 2, "=== CONTROLS ===");
    attroff(A_BOLD);
    row++;
    mvprintw(row, 4, "NAVIGATION:");
    row++;
    mvprintw(row, 6, "[Up/Down/Left/Right] - Move through all parameters");
    row++;
    mvprintw(row, 4, "LOAD PRESET:");
    row++;
    mvprintw(row, 6, "[+] or [-] - Cycle presets");
    row++;
    mvprintw(row, 4, "ADJUST VALUES:");
    row++;
    mvprintw(row, 6, "[PageUp]   - Increase value");
    row++;
    mvprintw(row, 6, "[PageDown] - Decrease value");
    row++;
    mvprintw(row, 4, "PLAY NOTES:");
    row++;
    mvprintw(row, 6, "[A..$ keys] - Press once to play, again to stop");
    row++;
    mvprintw(row, 4, "VOICE CONTROL:");
    row++;
    mvprintw(row, 6, "[F5] - PANIC (stop all notes immediately)");
    row++;
    mvprintw(row, 4, "OTHER:");
    row++;
    mvprintw(row, 6, "[ESC] - Stop all playing notes (graceful)");
    row++;
    mvprintw(row, 6, "[Ctrl+C] - Exit program");
}

void UIDraw::drawSelectedParameter(Parameter param) {
    int row = UILayout::SELECTED_ROW;
    
    attron(A_BOLD);
    attron(COLOR_PAIR(UIColor::SELECTED));
    mvprintw(row, 2, " SELECTED: %-24s ", UIParameters::getParameterName(param));
    attroff(COLOR_PAIR(UIColor::SELECTED));
    attroff(A_BOLD);
    row++;
    mvprintw(row, 2, "Press PageUp to increase, PageDown to decrease the selected parameter");
}

void UIDraw::drawPresetBrowser(SynthArchitecture* synth, int selectedIndex, const std::string& saveName, bool isSaving) {
    int height = 20;
    int width = 50;
    int startY = (LINES - height) / 2;
    int startX = (COLS - width) / 2;
    
    attron(A_BOLD);
    mvprintw(startY, startX, "+------------------------------------------------+");
    for (int i = 1; i < height - 1; i++) {
        mvprintw(startY + i, startX, "|");
        mvprintw(startY + i, startX + width - 1, "|");
    }
    mvprintw(startY + height - 1, startX, "+------------------------------------------------+");
    attroff(A_BOLD);
    
    if (isSaving) {
        attron(A_BOLD);
        mvprintw(startY + 1, startX + 15, " SAVE PRESET ");
        attroff(A_BOLD);
        mvprintw(startY + 3, startX + 2, "Name: %s", saveName.c_str());
    } else {
        attron(A_BOLD);
        mvprintw(startY + 1, startX + 15, " PRESET BROWSER ");
        attroff(A_BOLD);
        
        PresetManager* pm = synth->getPresetManager();
        if (pm && pm->exists()) {
            int presetCount = pm->getPresetCount();
            int displayStart = std::max(0, selectedIndex - 6);
            int displayEnd = std::min(presetCount, displayStart + 14);
            
            for (int i = displayStart; i < displayEnd; i++) {
                int row = startY + 3 + (i - displayStart);
                if (i == selectedIndex) {
                    attron(A_REVERSE);
                    attron(COLOR_PAIR(UIColor::SELECTED));
                }
                mvprintw(row, startX + 2, "> %-44s", pm->getPresetName(i).c_str());
                if (i == selectedIndex) {
                    attroff(COLOR_PAIR(UIColor::SELECTED));
                    attroff(A_REVERSE);
                }
            }
            
            mvprintw(startY + height - 3, startX + 2, "Preset %d / %d", selectedIndex + 1, presetCount);
        } else {
            mvprintw(startY + 3, startX + 2, "No presets available");
        }
    }
    
    mvprintw(startY + height - 2, startX + 2, "Use Up/Down to select, Enter to load");
}

void UIDraw::drawOscillatorSection(SynthArchitecture* synth, Parameter selectedParam) {
    int row = UILayout::OSC_ROW;
    int col = UILayout::LEFT_COL;

    attron(A_BOLD);
    attron(COLOR_PAIR(UIColor::SECTION_HEADER));
    mvprintw(row, col, "=== OSCILLATORS ===");
    attroff(COLOR_PAIR(UIColor::SECTION_HEADER));
    attroff(A_BOLD);
    row++;

    if (selectedParam == Parameter::WAVEFORM) {
        attron(A_REVERSE);
    }
    mvprintw(row, col, "%-14s: %-8s", "OSC 1", UIParameters::getWaveformName(synth));
    if (selectedParam == Parameter::WAVEFORM) {
        attroff(A_REVERSE);
    }
    row++;

    if (selectedParam == Parameter::OSC_MIX) {
        attron(A_REVERSE);
    }
    float mix = synth->getOscMix();
    int mixBar = static_cast<int>(mix * 16);
    mvprintw(row, col, "%-14s: [", "MIX");
    for (int i = 0; i < mixBar; i++) addch('=');
    for (int i = mixBar; i < 16; i++) addch(' ');
    printw("] %3d%%", static_cast<int>(mix * 100));
    if (selectedParam == Parameter::OSC_MIX) {
        attroff(A_REVERSE);
    }
    row++;

    if (selectedParam == Parameter::OSC2_DETUNE) {
        attron(A_REVERSE);
    }
    float detune = synth->getOsc2Detune();
    int detuneBar = static_cast<int>((detune + 1.0f) * 8);
    mvprintw(row, col, "%-14s: [", "DETUNE");
    for (int i = 0; i < detuneBar; i++) addch('=');
    for (int i = detuneBar; i < 16; i++) addch(' ');
    printw("] %+4.1fst", detune * 100.0f);
    if (selectedParam == Parameter::OSC2_DETUNE) {
        attroff(A_REVERSE);
    }
}

void UIDraw::drawLfoSection(SynthArchitecture* synth, Parameter selectedParam) {
    (void)synth;
    int row = UILayout::LFO_ROW;
    int col = UILayout::LEFT_COL;

    attron(A_BOLD);
    attron(COLOR_PAIR(UIColor::SECTION_HEADER));
    mvprintw(row, col, "=== LFO ===");
    attroff(COLOR_PAIR(UIColor::SECTION_HEADER));
    attroff(A_BOLD);
    row++;

    if (selectedParam == Parameter::LFO_WAVEFORM) {
        attron(A_REVERSE);
    }
    mvprintw(row, col, "%-14s: %-8s", "WAVEFORM", "SINE");
    if (selectedParam == Parameter::LFO_WAVEFORM) {
        attroff(A_REVERSE);
    }
    row++;

    if (selectedParam == Parameter::LFO_FREQUENCY) {
        attron(A_REVERSE);
    }
    mvprintw(row, col, "%-14s: %5.1f Hz", "FREQ", 1.0f);
    if (selectedParam == Parameter::LFO_FREQUENCY) {
        attroff(A_REVERSE);
    }
    row++;

    if (selectedParam == Parameter::LFO_DEPTH) {
        attron(A_REVERSE);
    }
    float depth = 0.5f;
    int depthBar = static_cast<int>(depth * 16);
    mvprintw(row, col, "%-14s: [", "DEPTH");
    for (int i = 0; i < depthBar; i++) addch('=');
    for (int i = depthBar; i < 16; i++) addch(' ');
    printw("] %4.2f", depth);
    if (selectedParam == Parameter::LFO_DEPTH) {
        attroff(A_REVERSE);
    }
}

void UIDraw::drawFilterSection(SynthArchitecture* synth, Parameter selectedParam) {
    int row = UILayout::FILTER_ROW;
    int col = UILayout::CENTER_COL;

    attron(A_BOLD);
    attron(COLOR_PAIR(UIColor::SECTION_HEADER));
    mvprintw(row, col, "=== FILTER ===");
    attroff(COLOR_PAIR(UIColor::SECTION_HEADER));
    attroff(A_BOLD);
    row++;

    if (selectedParam == Parameter::FILTER_TYPE) {
        attron(A_REVERSE);
    }
    mvprintw(row, col, "%-14s: %-8s", "TYPE", UIParameters::getFilterTypeName(synth));
    if (selectedParam == Parameter::FILTER_TYPE) {
        attroff(A_REVERSE);
    }
    row++;

    if (selectedParam == Parameter::CUTOFF) {
        attron(A_REVERSE);
    }
    float cutoff = synth->getCutoff();
    int cutoffBar = static_cast<int>((cutoff / 20000.0f) * 16);
    mvprintw(row, col, "%-14s: [", "CUTOFF");
    for (int i = 0; i < cutoffBar; i++) addch('|');
    for (int i = cutoffBar; i < 16; i++) addch(' ');
    printw("] %5.0fHz", cutoff);
    if (selectedParam == Parameter::CUTOFF) {
        attroff(A_REVERSE);
    }
    row++;

    if (selectedParam == Parameter::RESONANCE) {
        attron(A_REVERSE);
    }
    float res = synth->getResonance();
    int resBar = static_cast<int>(res * 16);
    mvprintw(row, col, "%-14s: [", "RESO");
    for (int i = 0; i < resBar; i++) addch('{');
    for (int i = resBar; i < 16; i++) addch(' ');
    printw("] %4.2f", res);
    if (selectedParam == Parameter::RESONANCE) {
        attroff(A_REVERSE);
    }
    row++;

    if (selectedParam == Parameter::FILTER_ENV_AMOUNT) {
        attron(A_REVERSE);
    }
    float envAmt = synth->getFilterEnvelopeAmount();
    int envBar = static_cast<int>((envAmt + 1.0f) * 8);
    mvprintw(row, col, "%-14s: [", "ENV AMT");
    for (int i = 0; i < envBar; i++) addch('=');
    for (int i = envBar; i < 16; i++) addch(' ');
    printw("] %+4.2f", envAmt);
    if (selectedParam == Parameter::FILTER_ENV_AMOUNT) {
        attroff(A_REVERSE);
    }
    row++;

    if (selectedParam == Parameter::HPF_FREQ) {
        attron(A_REVERSE);
    }
    float hpf = synth->getHPFCutoff();
    int hpfBar = static_cast<int>((hpf / 5000.0f) * 16);
    mvprintw(row, col, "%-14s: [", "HPF");
    for (int i = 0; i < hpfBar; i++) addch('|');
    for (int i = hpfBar; i < 16; i++) addch(' ');
    printw("] %4.0fHz", hpf);
    if (selectedParam == Parameter::HPF_FREQ) {
        attroff(A_REVERSE);
    }
}

void UIDraw::drawAmpEnvelopeSection(SynthArchitecture* synth, Parameter selectedParam) {
    int row = UILayout::AMP_ENV_ROW;
    int col = UILayout::RIGHT_COL;

    attron(A_BOLD);
    attron(COLOR_PAIR(UIColor::SECTION_HEADER));
    mvprintw(row, col, "=== AMP ENV ===");
    attroff(COLOR_PAIR(UIColor::SECTION_HEADER));
    attroff(A_BOLD);
    row++;

    if (selectedParam == Parameter::AMP_ATTACK) {
        attron(A_REVERSE);
    }
    mvprintw(row, col, "%-14s: %5.3fs", "ATTACK", synth->getAmpAttack());
    if (selectedParam == Parameter::AMP_ATTACK) {
        attroff(A_REVERSE);
    }
    row++;

    if (selectedParam == Parameter::AMP_DECAY) {
        attron(A_REVERSE);
    }
    mvprintw(row, col, "%-14s: %5.3fs", "DECAY", synth->getAmpDecay());
    if (selectedParam == Parameter::AMP_DECAY) {
        attroff(A_REVERSE);
    }
    row++;

    if (selectedParam == Parameter::AMP_SUSTAIN) {
        attron(A_REVERSE);
    }
    float sustain = synth->getAmpSustain();
    int susBar = static_cast<int>(sustain * 16);
    mvprintw(row, col, "%-14s: [", "SUSTAIN");
    for (int i = 0; i < susBar; i++) addch('=');
    for (int i = susBar; i < 16; i++) addch(' ');
    printw("] %4.2f", sustain);
    if (selectedParam == Parameter::AMP_SUSTAIN) {
        attroff(A_REVERSE);
    }
    row++;

    if (selectedParam == Parameter::AMP_RELEASE) {
        attron(A_REVERSE);
    }
    mvprintw(row, col, "%-14s: %5.3fs", "RELEASE", synth->getAmpRelease());
    if (selectedParam == Parameter::AMP_RELEASE) {
        attroff(A_REVERSE);
    }
}

void UIDraw::drawFilterEnvelopeSection(SynthArchitecture* synth, Parameter selectedParam) {
    int row = UILayout::FILTER_ENV_ROW;
    int col = UILayout::CENTER_COL;

    attron(A_BOLD);
    attron(COLOR_PAIR(UIColor::SECTION_HEADER));
    mvprintw(row, col, "=== FILTER ENV ===");
    attroff(COLOR_PAIR(UIColor::SECTION_HEADER));
    attroff(A_BOLD);
    row++;

    if (selectedParam == Parameter::FILTER_ATTACK) {
        attron(A_REVERSE);
    }
    mvprintw(row, col, "%-14s: %5.3fs", "ATTACK", synth->getFilterAttack());
    if (selectedParam == Parameter::FILTER_ATTACK) {
        attroff(A_REVERSE);
    }
    row++;

    if (selectedParam == Parameter::FILTER_DECAY) {
        attron(A_REVERSE);
    }
    mvprintw(row, col, "%-14s: %5.3fs", "DECAY", synth->getFilterDecay());
    if (selectedParam == Parameter::FILTER_DECAY) {
        attroff(A_REVERSE);
    }
    row++;

    if (selectedParam == Parameter::FILTER_SUSTAIN) {
        attron(A_REVERSE);
    }
    float sus = synth->getFilterSustain();
    int susBar = static_cast<int>(sus * 16);
    mvprintw(row, col, "%-14s: [", "SUSTAIN");
    for (int i = 0; i < susBar; i++) addch('=');
    for (int i = susBar; i < 16; i++) addch(' ');
    printw("] %4.2f", sus);
    if (selectedParam == Parameter::FILTER_SUSTAIN) {
        attroff(A_REVERSE);
    }
    row++;

    if (selectedParam == Parameter::FILTER_RELEASE) {
        attron(A_REVERSE);
    }
    mvprintw(row, col, "%-14s: %5.3fs", "RELEASE", synth->getFilterRelease());
    if (selectedParam == Parameter::FILTER_RELEASE) {
        attroff(A_REVERSE);
    }
}

void UIDraw::drawPerformanceSection(SynthArchitecture* synth, Parameter selectedParam) {
    int row = UILayout::PERFORMANCE_ROW;
    int col = UILayout::RIGHT_COL;

    attron(A_BOLD);
    attron(COLOR_PAIR(UIColor::SECTION_HEADER));
    mvprintw(row, col, "=== PERFORMANCE ===");
    attroff(COLOR_PAIR(UIColor::SECTION_HEADER));
    attroff(A_BOLD);
    row++;

    if (selectedParam == Parameter::POLYPHONY) {
        attron(A_REVERSE);
    }
    int poly = synth->getPolyphony();
    int polyBar = static_cast<int>((poly / 16.0f) * 16);
    mvprintw(row, col, "%-14s: [", "POLY");
    for (int i = 0; i < polyBar; i++) addch('=');
    for (int i = polyBar; i < 16; i++) addch(' ');
    printw("] %2d", poly);
    if (selectedParam == Parameter::POLYPHONY) {
        attroff(A_REVERSE);
    }
    row++;

    if (selectedParam == Parameter::LEGATO) {
        attron(A_REVERSE);
    }
    mvprintw(row, col, "%-14s: %-4s", "LEGATO", "OFF");
    if (selectedParam == Parameter::LEGATO) {
        attroff(A_REVERSE);
    }
    row++;

    if (selectedParam == Parameter::PORTAMENTO) {
        attron(A_REVERSE);
    }
    float port = 0.0f;
    int portBar = static_cast<int>(port * 16);
    mvprintw(row, col, "%-14s: [", "PORTA");
    for (int i = 0; i < portBar; i++) addch('=');
    for (int i = portBar; i < 16; i++) addch(' ');
    printw("] %4.2f", port);
    if (selectedParam == Parameter::PORTAMENTO) {
        attroff(A_REVERSE);
    }
}

void UIDraw::drawVolumeSection(SynthArchitecture* synth, Parameter selectedParam) {
    int row = UILayout::VOLUME_ROW;
    int col = UILayout::LEFT_COL;

    attron(A_BOLD);
    attron(COLOR_PAIR(UIColor::SECTION_HEADER));
    mvprintw(row, col, "=== VOLUME ===");
    attroff(COLOR_PAIR(UIColor::SECTION_HEADER));
    attroff(A_BOLD);
    row++;

    if (selectedParam == Parameter::VOLUME) {
        attron(A_REVERSE);
    }
    float volume = synth->getVolume();
    int volBar = static_cast<int>(volume * 16);
    mvprintw(row, col, "%-14s: [", "MASTER");
    for (int i = 0; i < volBar; i++) addch('=');
    for (int i = volBar; i < 16; i++) addch(' ');
    printw("] %3d%%", static_cast<int>(volume * 100));
    if (selectedParam == Parameter::VOLUME) {
        attroff(A_REVERSE);
    }
}

void UIDraw::drawModulationMatrix(SynthArchitecture* synth, Parameter selectedParam) {
    (void)synth;
    (void)selectedParam;
    int row = UILayout::MOD_MATRIX_ROW;
    int col = UILayout::LEFT_COL;

    attron(A_BOLD);
    attron(COLOR_PAIR(UIColor::MOD_MATRIX));
    mvprintw(row, col, "=== MOD MATRIX ===");
    attroff(COLOR_PAIR(UIColor::MOD_MATRIX));
    attroff(A_BOLD);
    row++;

    mvprintw(row, col, "%-10s %-8s %-10s", "SRC", "SCALE", "DEST");
    row++;

    const char* sources[] = {"LFO 1", "LFO 2", "ENV"};
    const char* dests[] = {"CUTOFF", "RESO", "MIX"};

    for (int i = 0; i < 2; i++) {
        mvprintw(row + i, col, "%-10s %-8s %-10s",
               sources[i], "1.00", dests[i]);
    }
}

void UIDraw::drawLegacySections(SynthArchitecture* synth, Parameter selectedParam) {
    drawPresetSection(synth, selectedParam);
    drawDelaySection(synth, selectedParam);
    drawReverbSection(synth, selectedParam);
    drawChorusSection(synth, selectedParam);
    drawDistortionSection(synth, selectedParam);
    drawMasterSection(synth, selectedParam);
}

void UIDraw::drawHelp() {
    int height = 20;
    int width = 60;
    int startY = (LINES - height) / 2;
    int startX = (COLS - width) / 2;

    attron(A_BOLD);
    for (int i = 0; i < width - 2; i++) {
        mvaddch(startY, startX + i, '-');
        mvaddch(startY + height - 1, startX + i, '-');
    }
    for (int i = 1; i < height - 1; i++) {
        mvaddch(startY + i, startX, '|');
        mvaddch(startY + i, startX + width - 1, '|');
    }
    attroff(A_BOLD);

    mvprintw(startY + 1, startX + 20, "KEYBOARD CONTROLS");
    mvprintw(startY + 3, startX + 2, "[Arrow Keys] Navigate parameters");
    mvprintw(startY + 4, startX + 2, "[PageUp/Down] Increase/decrease value");
    mvprintw(startY + 5, startX + 2, "[1-9,0]   Set slider 10-100%%");
    mvprintw(startY + 6, startX + 2, "[A-$,^]   Play notes C4-B4");
    mvprintw(startY + 7, startX + 2, "[F1]      This help screen");
    mvprintw(startY + 8, startX + 2, "[F2]      Preset browser");
    mvprintw(startY + 9, startX + 2, "[F5]      Panic (stop all)");
    mvprintw(startY + 10, startX + 2, "[ESC]     Release notes");
    mvprintw(startY + 11, startX + 2, "[Ctrl+C]  Exit program");

    mvprintw(startY + 13, startX + 18, "Press any key to close");
}