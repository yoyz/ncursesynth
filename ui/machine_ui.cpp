#include "machine_ui.h"
#include "ui_layout.h"
#include "../midi/midi_input.h"
#include <cmath>
#include <cstring>
#include <chrono>

MachineUI::MachineUI(Machine* mach, MachineManager* mgr)
    : selectedControl(0), machine(mach), machineManager(mgr),
      midiInput(nullptr), screenRows(0), screenCols(0),
      lastMidiNote(-1), lastMidiVel(0), midiActivity(false),
      menuSelection(0), engineIndex(0), midiDeviceIndex(-1) {
}

MachineUI::~MachineUI() {
    if (stdscr) {
        endwin();
    }
}

void MachineUI::init() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    start_color();
    nodelay(stdscr, TRUE);
    timeout(50);
    getmaxyx(stdscr, screenRows, screenCols);
}

void MachineUI::draw() {
    erase();

    drawMenuBar();
    drawMidiMonitor();

    refresh();
}

void MachineUI::drawMenuBar() {
    int row = 2;
    int col = 2;

    if (menuSelection == 1) attron(A_REVERSE);
    mvprintw(row, col, "[ENGINE: %s]", machine ? machine->getName().c_str() : "None");
    if (menuSelection == 1) attroff(A_REVERSE);

    if (menuSelection == 2) attron(A_REVERSE);
    std::string midiName = "None";
    if (midiInput && midiDeviceIndex >= 0) {
        int deviceCount = midiInput->getDeviceCount();
        if (midiDeviceIndex < deviceCount) {
            std::string name = midiInput->getDeviceName(midiDeviceIndex);
            if (!name.empty()) {
                midiName = name;
                if (midiName.length() > 20) {
                    midiName = midiName.substr(0, 17) + "...";
                }
            }
        }
    }
    mvprintw(row, col + 25, "[MIDI: %s]", midiName.c_str());
    if (menuSelection == 2) attroff(A_REVERSE);

    attron(A_DIM);
    mvprintw(row, col + 50, "(TAB: Menu | ARROWS: Navigate)");
    attroff(A_DIM);
}

void MachineUI::drawMidiMonitor() {
    int row = screenRows - 5;

    bool hasCC = false;
    int lastCC = -1;
    int lastCCValue = 0;
    int elapsed = 0;

    if (midiInput) {
        auto now = std::chrono::steady_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - midiInput->getLastActivity()).count();
        lastCC = midiInput->getLastCC();
        lastCCValue = midiInput->getLastCCValue();
        hasCC = (elapsed < 2000 && lastCC >= 0 && lastCC < 128);
    }

    if (hasCC) {
        attron(A_REVERSE | A_BOLD);
        mvprintw(row, 2, "MIDI: CC%3d = %3d   ", lastCC, lastCCValue);
        attroff(A_REVERSE | A_BOLD);

        for (int i = 20; i < screenCols - 1; i++) {
            mvaddch(row, i, ' ');
        }
    } else if (midiActivity) {
        attron(A_REVERSE | A_BOLD);
        mvprintw(row, 2, "MIDI: Note=%d Vel=%d  ", lastMidiNote, lastMidiVel);
        attroff(A_REVERSE | A_BOLD);

        for (int i = 25; i < screenCols - 1; i++) {
            mvaddch(row, i, ' ');
        }
    } else {
        attron(A_DIM);
        mvprintw(row, 2, "MIDI: Waiting for input...");
        attroff(A_DIM);
    }
}

void MachineUI::handleInput(int ch) {
    if (ch == '\t') {
        menuSelection = (menuSelection + 1) % 3;
        return;
    }

    if (menuSelection == 1) {
        if (ch == KEY_LEFT && machineManager) {
            engineIndex = (engineIndex - 1 + machineManager->getMachineCount()) % machineManager->getMachineCount();
        } else if (ch == KEY_RIGHT && machineManager) {
            engineIndex = (engineIndex + 1) % machineManager->getMachineCount();
        }
        return;
    }

    if (menuSelection == 2) {
        if (ch == KEY_LEFT && midiInput) {
            midiDeviceIndex = (midiDeviceIndex - 1 + midiInput->getDeviceCount()) % midiInput->getDeviceCount();
        } else if (ch == KEY_RIGHT && midiInput) {
            midiDeviceIndex = (midiDeviceIndex + 1) % midiInput->getDeviceCount();
        }
        return;
    }

    handleNavigation(ch);
    if (ch >= '0' && ch <= '9') {
        float val = (ch == '0') ? 1.0f : (ch - '0') / 10.0f;
        if (selectedControl >= 0 && selectedControl < (int)controls.size()) {
            controls[selectedControl].value = val;
            machine->setI(controls[selectedControl].param, (int)(val * 128));
        }
    }
}

void MachineUI::handleNavigation(int ch) {
    if (ch == KEY_UP) {
        selectedControl = (selectedControl - 1 + controls.size()) % controls.size();
    } else if (ch == KEY_DOWN) {
        selectedControl = (selectedControl + 1) % controls.size();
    } else if (ch == KEY_LEFT || ch == KEY_RIGHT) {
        handleValueChange(ch);
    } else if (ch == KEY_PPAGE) {
        if (selectedControl >= 0 && selectedControl < (int)controls.size()) {
            controls[selectedControl].value = std::min(controls[selectedControl].value + 0.05f, controls[selectedControl].maxVal);
            machine->setI(controls[selectedControl].param, (int)(controls[selectedControl].value * 128));
        }
    } else if (ch == KEY_NPAGE) {
        if (selectedControl >= 0 && selectedControl < (int)controls.size()) {
            controls[selectedControl].value = std::max(controls[selectedControl].value - 0.05f, controls[selectedControl].minVal);
            machine->setI(controls[selectedControl].param, (int)(controls[selectedControl].value * 128));
        }
    }
}

void MachineUI::handleValueChange(int ch) {
    if (selectedControl < 0 || selectedControl >= (int)controls.size()) return;

    float& val = controls[selectedControl].value;
    float step = (controls[selectedControl].maxVal - controls[selectedControl].minVal) / 20.0f;

    if (ch == KEY_RIGHT) {
        val = std::min(val + step, controls[selectedControl].maxVal);
    } else if (ch == KEY_LEFT) {
        val = std::max(val - step, controls[selectedControl].minVal);
    }

    machine->setI(controls[selectedControl].param, (int)(val * 128));
}

void MachineUI::updateValues() {
    updateControlValues();
}

void MachineUI::setControlValue(int paramId, float value) {
    for (auto& c : controls) {
        if (c.param == paramId) {
            c.value = value;
            break;
        }
    }
}