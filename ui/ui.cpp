#include "ui.h"
#include "ui_layout.h"
#include "ui_colors.h"
#include "ui_parameters.h"
#include "ui_draw.h"
#include "ui_input.h"
#include "ui_constants.h"
#include <ncurses.h>
#include <chrono>
#include <map>

UI::UI(SynthArchitecture* synthArch) : synth(synthArch), running(true), selectedParameter(Parameter::POLYPHONY) {}

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

        // Draw all UI sections
        UIDraw::drawTitle();
        UIDraw::drawStatus(synth);
        UIDraw::drawFilterSection(synth, selectedParameter);
        UIDraw::drawAmplitudeEnvelopeSection(synth, selectedParameter);
        UIDraw::drawFilterEnvelopeSection(synth, selectedParameter);
        UIDraw::drawOscillatorSection(synth, selectedParameter);
        UIDraw::drawPresetSection(synth, selectedParameter);
        UIDraw::drawDelaySection(synth, selectedParameter);
        UIDraw::drawReverbSection(synth, selectedParameter);
        UIDraw::drawChorusSection(synth, selectedParameter);
        UIDraw::drawDistortionSection(synth, selectedParameter);
        UIDraw::drawMasterSection(synth, selectedParameter);
        UIDraw::drawMidiSection(synth, selectedParameter);
        UIDraw::drawVoiceDisplay(synth);
        UIDraw::drawKeyboard();
        UIDraw::drawControls();
        UIDraw::drawSelectedParameter(selectedParameter);

        refresh();

        // Handle input
        int ch = getch();
        if (ch != ERR) {
            // Preset navigation with +/-
            if (ch == '+' || ch == '=') {
                UIParameters::loadNextPreset(synth);
            }
            else if (ch == '-' || ch == '_') {
                UIParameters::loadPrevPreset(synth);
            }
            // Parameter adjustment keys - PageUp/PageDown
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
            // ESC key - clear all notes gracefully
            else if (ch == 27) {
                UIInput::clearAllNotes(synth);
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
