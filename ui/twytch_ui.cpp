#include "pbsynth_ui.h"
#include "../machine/Machine.h"
#include <cmath>
#include <cstring>

PBSynthUI::PBSynthUI(Machine* mach)
    : selectedControl(0), machine(mach), screenRows(0), screenCols(0),
      lastMidiNote(-1), lastMidiVel(0), midiActivity(false) {
    initControls();
}

PBSynthUI::~PBSynthUI() {
    if (stdscr) {
        endwin();
    }
}

void PBSynthUI::initControls() {
    controls.clear();

    // Column 1: OSCILLATORS (col 2)
    controls.push_back({"OSC1 WAVE", PBSynthParam::P_OSC1_WAVEFORM, 6, 2, 0, 0, 3});
    controls.push_back({"OSC1 DETUNE", PBSynthParam::P_OSC1_DETUNE, 7, 2, 0.5, 0, 1});
    controls.push_back({"OSC1 SCALE", PBSynthParam::P_OSC1_SCALE, 8, 2, 0, -24, 24});
    controls.push_back({"OSC1 AMP", PBSynthParam::P_OSC1_AMP, 9, 2, 0.8, 0, 1});

    controls.push_back({"OSC2 WAVE", PBSynthParam::P_OSC2_WAVEFORM, 11, 2, 0, 0, 3});
    controls.push_back({"OSC2 DETUNE", PBSynthParam::P_OSC2_DETUNE, 12, 2, 0.5, 0, 1});
    controls.push_back({"OSC2 SCALE", PBSynthParam::P_OSC2_SCALE, 13, 2, 0, -24, 24});
    controls.push_back({"OSC2 AMP", PBSynthParam::P_OSC2_AMP, 14, 2, 0.8, 0, 1});

    // Column 2: FILTER (col 40)
    controls.push_back({"CUTOFF", PBSynthParam::P_FILTER_CUTOFF, 6, 40, 0.5, 0, 1});
    controls.push_back({"RESONANCE", PBSynthParam::P_FILTER_RESONANCE, 7, 40, 0.2, 0, 1});

    controls.push_back({"FENV ATTACK", PBSynthParam::P_FILTER_ENV_ATTACK, 9, 40, 0.01, 0, 1});
    controls.push_back({"FENV DECAY", PBSynthParam::P_FILTER_ENV_DECAY, 10, 40, 0.3, 0, 1});
    controls.push_back({"FENV SUSTAIN", PBSynthParam::P_FILTER_ENV_SUSTAIN, 11, 40, 0.5, 0, 1});
    controls.push_back({"FENV RELEASE", PBSynthParam::P_FILTER_ENV_RELEASE, 12, 40, 0.3, 0, 1});
    controls.push_back({"FENV DEPTH", PBSynthParam::P_FILTER_ENV_DEPTH, 13, 40, 0.5, 0, 1});

    // Column 3: AMP ENVELOPE & LFO (col 78)
    controls.push_back({"AENV ATTACK", PBSynthParam::P_AMP_ATTACK, 6, 78, 0.01, 0, 1});
    controls.push_back({"AENV DECAY", PBSynthParam::P_AMP_DECAY, 7, 78, 0.3, 0, 1});
    controls.push_back({"AENV SUSTAIN", PBSynthParam::P_AMP_SUSTAIN, 8, 78, 0.7, 0, 1});
    controls.push_back({"AENV RELEASE", PBSynthParam::P_AMP_RELEASE, 9, 78, 0.3, 0, 1});

    controls.push_back({"LFO1 WAVE", PBSynthParam::P_LFO1_WAVEFORM, 11, 78, 0, 0, 3});
    controls.push_back({"LFO1 FREQ", PBSynthParam::P_LFO1_FREQ, 12, 78, 0.1, 0, 1});
    controls.push_back({"LFO1 DEPTH", PBSynthParam::P_LFO1_DEPTH, 13, 78, 0, 0, 1});

    controls.push_back({"LFO2 WAVE", PBSynthParam::P_LFO2_WAVEFORM, 15, 78, 0, 0, 3});
    controls.push_back({"LFO2 FREQ", PBSynthParam::P_LFO2_FREQ, 16, 78, 0.1, 0, 1});
    controls.push_back({"LFO2 DEPTH", PBSynthParam::P_LFO2_DEPTH, 17, 78, 0, 0, 1});

    // OSC Mix & Master
    controls.push_back({"OSC MIX", PBSynthParam::P_OSC_MIX, 20, 78, 0.5, 0, 1});
    controls.push_back({"MASTER", PBSynthParam::P_MASTER_VOL, 21, 78, 0.7, 0, 1});

    for (auto& c : controls) {
        machine->setI(c.param, (int)(c.value * 128));
    }
}

void PBSynthUI::init() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    getmaxyx(stdscr, screenRows, screenCols);
}

void PBSynthUI::draw() {
    clear();

    attron(A_BOLD);
    mvprintw(2, 2, "PBSYNTH ENGINE");
    attroff(A_BOLD);

    // Column headers
    drawColumnHeader(2, "OSCILLATORS");
    drawColumnHeader(40, "FILTER");
    drawColumnHeader(78, "ENVELOPE / LFO");

    // Draw controls
    for (size_t i = 0; i < controls.size(); i++) {
        drawControl(i, (int)i == selectedControl);
    }

    // Instructions
    attron(A_DIM);
    mvprintw(screenRows - 3, 2, "ARROWS: Navigate | 1-9: Set Value | PAGEUP/DOWN: Adjust | Q: Quit");
    attroff(A_DIM);

    // MIDI Monitor
    drawMidiMonitor();

    refresh();
}

void PBSynthUI::drawMidiMonitor() {
    int row = screenRows - 5;

    if (midiActivity) {
        attron(A_REVERSE | A_BOLD);
        mvprintw(row, 2, "MIDI: Note=%d Vel=%d  ", lastMidiNote, lastMidiVel);
        attroff(A_REVERSE | A_BOLD);

        // Clear remaining
        for (int i = 25; i < screenCols - 1; i++) {
            mvaddch(row, i, ' ');
        }
    } else {
        attron(A_DIM);
        mvprintw(row, 2, "MIDI: Waiting for input...");
        attroff(A_DIM);
    }
}

void PBSynthUI::drawColumnHeader(int col, const char* title) {
    attron(A_BOLD | A_UNDERLINE);
    mvprintw(4, col, "%s", title);
    attroff(A_BOLD | A_UNDERLINE);
}

void PBSynthUI::drawControl(int index, bool selected) {
    const auto& c = controls[index];
    int row = c.row;
    int col = c.col;

    if (selected) {
        attron(A_REVERSE);
    }

    drawSlider(row, col, c.name.c_str(), c.value, selected);

    if (selected) {
        attroff(A_REVERSE);
    }
}

void PBSynthUI::drawSlider(int row, int col, const char* name, float value, bool selected) {
    char bar[18];
    memset(bar, ' ', 16);
    bar[16] = '\0';

    int filled = (int)(value * 16);
    if (filled > 16) filled = 16;
    if (filled < 0) filled = 0;

    bar[0] = '[';
    for (int i = 1; i <= 16; i++) {
        if (i <= filled) {
            bar[i] = '=';
        } else {
            bar[i] = '-';
        }
    }
    bar[16] = ']';

    if (selected) {
        attron(A_BOLD);
    }
    mvprintw(row, col, "%-12s", name);
    mvprintw(row, col + 13, "%s %3d%%", bar, (int)(value * 100));
    if (selected) {
        attroff(A_BOLD);
    }
}

void PBSynthUI::handleInput(int ch) {
    auto& c = controls[selectedControl];

    switch (ch) {
        case KEY_UP:
            if (selectedControl >= 10) selectedControl -= 10;
            break;
        case KEY_DOWN:
            if (selectedControl + 10 < (int)controls.size()) selectedControl += 10;
            break;
        case KEY_LEFT:
            if (selectedControl > 0) selectedControl--;
            break;
        case KEY_RIGHT:
            if (selectedControl < (int)controls.size() - 1) selectedControl++;
            break;
        case KEY_PPAGE:
            c.value += 0.05f;
            if (c.value > c.maxVal) c.value = c.maxVal;
            machine->setI(c.param, (int)(c.value * 128));
            break;
        case KEY_NPAGE:
            c.value -= 0.05f;
            if (c.value < c.minVal) c.value = c.minVal;
            machine->setI(c.param, (int)(c.value * 128));
            break;
        case '1': case '2': case '3': case '4': case '5':
        case '6': case '7': case '8': case '9':
            c.value = (ch - '0') * 0.1f;
            machine->setI(c.param, (int)(c.value * 128));
            break;
        case '0':
            c.value = 1.0f;
            machine->setI(c.param, (int)(c.value * 128));
            break;
    }
}

void PBSynthUI::updateValues() {
    for (auto& c : controls) {
        int raw = machine->getI(c.param);
        if (raw > 0) {
            float normalized = (float)raw / 128.0f;
            if (normalized > 1.0f) normalized = 1.0f;
            if (normalized < 0.0f) normalized = 0.0f;
            c.value = normalized;
        }
    }

    if (midiActivity) {
        midiActivity = false;
    }
}

void PBSynthUI::setControlValue(int paramId, float value) {
    for (auto& c : controls) {
        if (c.param == paramId) {
            c.value = value;
            break;
        }
    }
}