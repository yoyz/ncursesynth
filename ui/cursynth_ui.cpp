#include "cursynth_ui.h"
#include "ui_layout.h"
#include <cmath>
#include <cstring>

CursynthUI::CursynthUI(Machine* mach, MachineManager* mgr)
    : MachineUI(mach, mgr) {
    init();
}

CursynthUI::~CursynthUI() {}

void CursynthUI::init() {
    initControls();
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    start_color();
    nodelay(stdscr, TRUE);
    timeout(50);
    getmaxyx(stdscr, screenRows, screenCols);
}

void CursynthUI::initControls() {
    controls.clear();

    controls.push_back({"OSC1 WAVE", 0, 3, 2, 0.5f, 0.0f, 1.0f});
    controls.push_back({"OSC1 DETUNE", 1, 3, 26, 0.5f, 0.0f, 1.0f});
    controls.push_back({"OSC1 SCALE", 2, 3, 50, 0.0f, 0.0f, 1.0f});
    controls.push_back({"OSC1 AMP", 3, 3, 74, 0.8f, 0.0f, 1.0f});

    controls.push_back({"OSC2 WAVE", 4, 7, 2, 0.5f, 0.0f, 1.0f});
    controls.push_back({"OSC2 DETUNE", 5, 7, 26, 0.5f, 0.0f, 1.0f});
    controls.push_back({"OSC2 SCALE", 6, 7, 50, 0.0f, 0.0f, 1.0f});
    controls.push_back({"OSC2 AMP", 7, 7, 74, 0.8f, 0.0f, 1.0f});

    controls.push_back({"OSC MIX", 8, 11, 2, 0.5f, 0.0f, 1.0f});

    controls.push_back({"CUTOFF", 50, 3, 40, 0.8f, 0.0f, 1.0f});
    controls.push_back({"RESONANCE", 51, 3, 50, 0.2f, 0.0f, 1.0f});
    controls.push_back({"FENV DEPTH", 52, 3, 60, 0.5f, 0.0f, 1.0f});

    controls.push_back({"AENV ATTACK", 80, 9, 40, 0.01f, 0.0f, 1.0f});
    controls.push_back({"AENV DECAY", 81, 9, 50, 0.3f, 0.0f, 1.0f});
    controls.push_back({"AENV SUSTAIN", 82, 9, 60, 0.7f, 0.0f, 1.0f});
    controls.push_back({"AENV RELEASE", 83, 9, 70, 0.3f, 0.0f, 1.0f});

    controls.push_back({"FENV ATTACK", 90, 13, 40, 0.01f, 0.0f, 1.0f});
    controls.push_back({"FENV DECAY", 91, 13, 50, 0.3f, 0.0f, 1.0f});
    controls.push_back({"FENV SUSTAIN", 92, 13, 60, 0.5f, 0.0f, 1.0f});
    controls.push_back({"FENV RELEASE", 93, 13, 70, 0.3f, 0.0f, 1.0f});

    controls.push_back({"LFO1 WAVE", 20, 15, 2, 0.0f, 0.0f, 1.0f});
    controls.push_back({"LFO1 FREQ", 21, 15, 20, 0.1f, 0.0f, 1.0f});
    controls.push_back({"LFO1 DEPTH", 22, 15, 38, 0.0f, 0.0f, 1.0f});
}

void CursynthUI::drawColumnHeader(int col, const char* title) {
    attron(A_BOLD | A_UNDERLINE);
    mvprintw(6, col, "%s", title);
    attroff(A_BOLD | A_UNDERLINE);
}

void CursynthUI::drawControl(int index, bool selected) {
    if (index < 0 || index >= (int)controls.size()) return;

    const auto& c = controls[index];
    int row = c.row + 7;
    int col = c.col;

    char name[16];
    snprintf(name, sizeof(name), "%-11s", c.name.c_str());

    char valueStr[16];
    snprintf(valueStr, sizeof(valueStr), "%3d%%", (int)(c.value * 100));

    char bar[20];
    int barLen = (int)(c.value * 15);
    memset(bar, '#', barLen);
    memset(bar + barLen, '-', 15 - barLen);
    bar[15] = '\0';

    if (selected) attron(A_BOLD);
    mvprintw(row, col, "%s [%s] %s", name, bar, valueStr);
    if (selected) attroff(A_BOLD);
}

void CursynthUI::drawSlider(int row, int col, const char* name, float value, bool selected) {
    char bar[20];
    int barLen = (int)(value * 15);
    memset(bar, '#', barLen);
    memset(bar + barLen, '-', 15 - barLen);
    bar[15] = '\0';

    if (selected) attron(A_BOLD);
    mvprintw(row, col, "%-11s [%s] %3d%%", name, bar, (int)(value * 100));
    if (selected) attroff(A_BOLD);
}

void CursynthUI::updateControlValues() {
    if (!machine) return;

    for (auto& c : controls) {
        int raw = machine->getI(c.param);
        if (raw >= 0 && raw <= 128) {
            c.value = (float)raw / 128.0f;
            if (c.value > 1.0f) c.value = 1.0f;
            if (c.value < 0.0f) c.value = 0.0f;
        }
    }

    if (midiActivity) {
        midiActivity = false;
    }
}

void CursynthUI::draw() {
    erase();

    drawMenuBar();

    drawColumnHeader(2, "OSCILLATORS");
    drawColumnHeader(40, "FILTER");
    drawColumnHeader(78, "ENVELOPE");

    for (size_t i = 0; i < controls.size(); i++) {
        drawControl(i, (int)i == selectedControl);
    }

    attron(A_DIM);
    if (menuSelection == 0) {
        mvprintw(screenRows - 3, 2, "TAB: Menu | ARROWS: Navigate | 1-9: Set Value | Q: Quit");
    } else {
        mvprintw(screenRows - 3, 2, "LEFT/RIGHT: Switch | TAB: Params | Q: Quit");
    }
    attroff(A_DIM);

    drawMidiMonitor();

    refresh();
}

void CursynthUI::handleInput(int ch) {
    // Only handle parameter navigation when NOT in menu mode
    if (menuSelection == 1) {
        MachineUI::handleInput(ch);
        return;
    }

    if (ch == KEY_UP) {
        selectedControl = (selectedControl - 1 + controls.size()) % controls.size();
        return;
    }
    if (ch == KEY_DOWN) {
        selectedControl = (selectedControl + 1) % controls.size();
        return;
    }

    MachineUI::handleInput(ch);
}

void CursynthUI::updateValues() {
    updateControlValues();
}