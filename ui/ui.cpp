#include "ui.h"
#include "ui_layout.h"
#include "ui_colors.h"
#include "ui_parameters.h"
#include "ui_draw.h"
#include "ui_input.h"
#include "ui_constants.h"
#include "machine/Ncursesynth/preset/preset_manager.h"
#include <ncurses.h>
#include <chrono>
#include <map>
#include <sstream>
#include <iostream>

UI::UI(SynthArchitecture* synthArch) 
    : synth(synthArch), running(true), selectedParameter(Parameter::POLYPHONY),
      mode(UIMode::NORMAL), browserSelectedIndex(0), savePresetName("") {}

UI::~UI() {
    stop();
}

void UI::start() {
    uiThread = std::thread(&UI::run, this);
}

void UI::stop() {
    running = false;
    if (uiThread.joinable()) {
        uiThread.join();
    }
}


void UI::run() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);

    UIColor::initialize();

    // Counter for periodic checks
    int releaseCheckCounter = 0;

while (running) {
        clear();

        // Draw all UI sections (cursynth-style 3-column layout)
        UIDraw::drawTitle();

        if (mode == UIMode::PRESET_BROWSER || mode == UIMode::PRESET_SAVE) {
            UIDraw::drawPresetBrowser(synth, browserSelectedIndex, savePresetName,
                                       mode == UIMode::PRESET_SAVE);
        } else if (mode == UIMode::HELP) {
            UIDraw::drawHelp();
        } else {
            UIDraw::drawStatus(synth);

            // Column 1 (LEFT): OSCILLATORS, LFO, MOD MATRIX, VOLUME
            UIDraw::drawOscillatorSection(synth, selectedParameter);
            UIDraw::drawLfoSection(synth, selectedParameter);
            UIDraw::drawModulationMatrix(synth, selectedParameter);
            UIDraw::drawVolumeSection(synth, selectedParameter);

            // Column 2 (CENTER): FILTER, FILTER ENVELOPE
            UIDraw::drawFilterSection(synth, selectedParameter);
            UIDraw::drawFilterEnvelopeSection(synth, selectedParameter);

            // Column 3 (RIGHT): PERFORMANCE, AMP ENV, DELAY, REVERB, CHORUS, DISTORTION, MASTER
            UIDraw::drawPerformanceSection(synth, selectedParameter);
            UIDraw::drawAmpEnvelopeSection(synth, selectedParameter);
            UIDraw::drawDelaySection(synth, selectedParameter);
            UIDraw::drawReverbSection(synth, selectedParameter);
            UIDraw::drawChorusSection(synth, selectedParameter);
            UIDraw::drawDistortionSection(synth, selectedParameter);
            UIDraw::drawMasterSection(synth, selectedParameter);

            UIDraw::drawKeyboard();
            UIDraw::drawControls();
            UIDraw::drawSelectedParameter(selectedParameter);
        }

        refresh();

        // Handle input
        int ch = getch();
        if (ch != ERR) {
            // F1 - Help screen
            if (ch == KEY_F(1)) {
                mode = (mode == UIMode::HELP) ? UIMode::NORMAL : UIMode::HELP;
            }
            if (ch == KEY_F(2)) {
                mode = UIMode::PRESET_BROWSER;
                browserSelectedIndex = synth->getPresetManager()->getCurrentPresetIndex();
                if (browserSelectedIndex < 0) browserSelectedIndex = 0;
            }
            // In preset browser or save mode
            else if (mode == UIMode::PRESET_BROWSER || mode == UIMode::PRESET_SAVE) {
                handleBrowserInput(ch);
            }
            // Preset navigation with +/- - remove this
            else if (ch == KEY_PPAGE) {
                UIParameters::increaseParameter(synth, selectedParameter);
            }
            else if (ch == KEY_NPAGE) {
                UIParameters::decreaseParameter(synth, selectedParameter);
            }
            // Navigation keys
            else if (ch == KEY_UP || ch == KEY_DOWN || ch == KEY_LEFT || ch == KEY_RIGHT) {
                UIInput::handleNavigation(ch, selectedParameter);
            }
            // ESC key - clear all notes gracefully (or exit browser/help)
            else if (ch == 27) {
                if (mode == UIMode::PRESET_BROWSER || mode == UIMode::PRESET_SAVE || mode == UIMode::HELP) {
                    mode = UIMode::NORMAL;
                } else {
                    UIInput::clearAllNotes(synth);
                }
            }
            // Any key closes help screen
            else if (mode == UIMode::HELP) {
                mode = UIMode::NORMAL;
            }
            // F5 - PANIC immediate stop
            else if (ch == KEY_F(5)) {
                UIInput::panicButton(synth);
            }
            // Number keys 1-4 to release specific voices (for testing)
            else if (ch >= '1' && ch <= '4') {
                int voiceNum = ch - '0';
                UIInput::releaseVoice(voiceNum, synth);
            }
            // Note keys (A, Z, E, R, T, Y, U, I, O, P, ^, $)
            else {
                // Check if it's a valid note key
                bool isNote = false;
                for (int i = 0; i < 12; i++) {
                    if (ch == KEY_BINDINGS[i]) {
                        isNote = true;
                        break;
                    }
                }

                if (isNote) {
                    UIInput::handleNoteInput(ch, synth);
                }
            }
        }

        // Periodically check for key releases (every 500ms)
        releaseCheckCounter++;
        if (releaseCheckCounter >= 50) {
            UIInput::checkKeyReleases(synth);
            releaseCheckCounter = 0;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // Clear all notes on exit
    UIInput::clearAllNotes(synth);

    endwin();
}

void UI::handleBrowserInput(int ch) {
    PresetManager* pm = synth->getPresetManager();
    int presetCount = pm->getPresetCount();
    
    if (mode == UIMode::PRESET_SAVE) {
        if (ch == '\n' || ch == KEY_ENTER) {
            if (!savePresetName.empty()) {
                printf("\n=== SAVING PRESET: %s ===\n", savePresetName.c_str());
                bool success = pm->saveCurrentAsNew(savePresetName, synth);
                printf("Save result: %d, reloading bank...\n", success);
                pm->loadBank();
                printf("Loaded %d presets\n", pm->getPresetCount());
                // Stay in browser mode to see the new preset
                mode = UIMode::PRESET_BROWSER;
                browserSelectedIndex = pm->getCurrentPresetIndex();
                printf("Current index: %d\n", browserSelectedIndex);
            }
        }
        else if (ch == 127 || ch == KEY_BACKSPACE) {
            if (!savePresetName.empty()) {
                savePresetName.pop_back();
            }
        }
        else if (ch >= 32 && ch <= 126) {
            if (savePresetName.length() < 32) {
                savePresetName += static_cast<char>(ch);
            }
        }
        return;
    }
    
    // PRESET_BROWSER mode
    if (ch == KEY_UP) {
        browserSelectedIndex = (browserSelectedIndex - 1 + presetCount) % presetCount;
    }
    else if (ch == KEY_DOWN) {
        browserSelectedIndex = (browserSelectedIndex + 1) % presetCount;
    }
    else if (ch == 'l' || ch == 'L') {
        // Load preset
        pm->loadPreset(browserSelectedIndex, synth);
        mode = UIMode::NORMAL;
    }
    else if (ch == 's' || ch == 'S') {
        // Save preset - enter save mode
        savePresetName = pm->getPresetName(browserSelectedIndex);
        mode = UIMode::PRESET_SAVE;
    }
    else if (ch == 'n' || ch == 'N') {
        // Save current as new preset
        savePresetName = "";
        mode = UIMode::PRESET_SAVE;
    }
    else if (ch == 27) { // ESC
        mode = UIMode::NORMAL;
    }
}
