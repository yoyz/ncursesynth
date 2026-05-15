#include "machine_ui.h"
#include "ui_layout.h"
#include "../midi/midi_input.h"
#include "../midi/midi_mapping.h"
#include <cmath>
#include <cstring>
#include <chrono>

MachineUI::MachineUI(Machine* mach, MachineManager* mgr)
    : selectedControl(0), machine(mach), machineManager(mgr),
      midiInput(nullptr), screenRows(0), screenCols(0),
      lastMidiNote(-1), lastMidiVel(0), midiActivity(false),
      menuSelection(0), menuIndex(0), midiDeviceIndex(-1), mappingIndex(0) {
    columnTitles[0] = "OSCILLATORS";
    columnTitles[1] = "FILTER";
    columnTitles[2] = "ENVELOPE";
}

MachineUI::~MachineUI() {
    if (stdscr) {
        endwin();
    }
}

void MachineUI::init() {
    initControls();
    if (!stdscr) {
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        start_color();
        nodelay(stdscr, TRUE);
        timeout(50);
    }
    getmaxyx(stdscr, screenRows, screenCols);

    if (machine) {
        for (const auto& c : controls) {
            int val = (int)((c.value - c.minVal) / (c.maxVal - c.minVal) * 127.0f);
            machine->setI(c.param, val);
        }
    }
}

void MachineUI::drawColumnHeader(int col, const char* title) {
    attron(A_BOLD | A_UNDERLINE);
    mvprintw(HEADER_ROW, col, "%s", title);
    attroff(A_BOLD | A_UNDERLINE);
}

void MachineUI::drawControl(int index, bool selected) {
    if (index < 0 || index >= (int)controls.size()) return;

    const auto& c = controls[index];
    int row = c.row + CONTROL_ROW_OFFSET;
    int col = c.col;

    char name[16];
    snprintf(name, sizeof(name), "%-11s", c.name.c_str());

    if (selected) attron(A_REVERSE);

    const char* displayStr = machine ? machine->getDisplayString(c.param) : nullptr;
    if (displayStr && displayStr[0] != '\0') {
        mvprintw(row, col, "%s [%s]", name, displayStr);
        int pad = col + 13 + 16 - (col + 13 + strlen(displayStr));
        for (int i = 0; i < pad && i < 10; i++) mvaddch(row, col + 13 + strlen(displayStr) + i, ' ');
    } else {
        char bar[24];
        int barLen = (int)((c.value - c.minVal) / (c.maxVal - c.minVal) * CONTROL_BAR_LEN);
        if (barLen < 0) barLen = 0;
        if (barLen > CONTROL_BAR_LEN) barLen = CONTROL_BAR_LEN;
        memset(bar, '#', barLen);
        memset(bar + barLen, '-', CONTROL_BAR_LEN - barLen);
        bar[CONTROL_BAR_LEN] = '\0';

        int pct = (int)((c.value - c.minVal) / (c.maxVal - c.minVal) * 100);
        mvprintw(row, col, "%s [%s] %3d%%", name, bar, pct);
    }

    if (selected) attroff(A_REVERSE);
}

void MachineUI::drawSlider(int row, int col, const char* name, float value, bool selected) {
    char bar[24];
    int barLen = (int)(value * CONTROL_BAR_LEN);
    if (barLen < 0) barLen = 0;
    if (barLen > CONTROL_BAR_LEN) barLen = CONTROL_BAR_LEN;
    memset(bar, '#', barLen);
    memset(bar + barLen, '-', CONTROL_BAR_LEN - barLen);
    bar[CONTROL_BAR_LEN] = '\0';

    if (selected) attron(A_REVERSE);
    mvprintw(row, col, "%-11s [%s] %3d%%", name, bar, (int)(value * 100));
    if (selected) attroff(A_REVERSE);
}

void MachineUI::updateControlValues() {
    if (!machine) return;

    for (auto& c : controls) {
        int raw = machine->getI(c.param);
        if (raw >= 0 && raw <= 128) {
            c.value = (float)raw / 128.0f;
            if (c.value > c.maxVal) c.value = c.maxVal;
            if (c.value < c.minVal) c.value = c.minVal;
        }
    }

    midiActivity = false;
}

void MachineUI::draw() {
    erase();
    attrset(A_NORMAL);

    drawMenuBar();

    std::string engineName = machine ? machine->getName() : "SYNTH";
    attron(A_BOLD);
    mvprintw(4, 2, "%s ENGINE", engineName.c_str());
    attroff(A_BOLD);

    drawColumnHeader(2, columnTitles[0].c_str());
    drawColumnHeader(40, columnTitles[1].c_str());
    drawColumnHeader(78, columnTitles[2].c_str());

    for (size_t i = 0; i < controls.size(); i++) {
        drawControl(i, (int)i == selectedControl);
    }

    attron(A_DIM);
    if (menuSelection == 0) {
        mvprintw(screenRows - 3, 2, "TAB: Menu | ARROWS: Navigate | 1-9: Set Value | PAGEUP/DOWN: Adjust | Q: Quit");
    } else if (menuSelection == 1) {
        mvprintw(screenRows - 3, 2, "LEFT/RIGHT: Switch Engine | TAB: Menu | Q: Quit");
    } else if (menuSelection == 2) {
        mvprintw(screenRows - 3, 2, "LEFT/RIGHT: Switch MIDI | TAB: Menu | Q: Quit");
    } else {
        mvprintw(screenRows - 3, 2, "LEFT/RIGHT: Switch Mapping | TAB: Menu | Q: Quit");
    }
    attroff(A_DIM);

    drawMidiMonitor();

    refresh();
}

void MachineUI::drawMenuBar() {
    int row = 2;
    int col = 2;

    attrset(A_NORMAL);
    if (menuSelection == 1 && menuIndex == 0) attron(A_REVERSE);
    mvprintw(row, col, "[ENGINE: %s]", machine ? machine->getName().c_str() : "None");
    attrset(A_NORMAL);

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
    attrset(A_NORMAL);
    if (menuSelection == 1 && menuIndex == 1) attron(A_REVERSE);
    mvprintw(row, col + 18, "[MIDI: %s]", midiName.c_str());
    attrset(A_NORMAL);

    std::string mapName = "None";
    if (midiInput) {
        auto* mm = midiInput->getMappingManager();
        if (mm && mm->getMappingCount() > 0) {
            mapName = mm->getMappingName(mm->getCurrentMappingIndex());
            if (mapName.length() > 20) {
                mapName = mapName.substr(0, 17) + "...";
            }
        }
    }
    attrset(A_NORMAL);
    if (menuSelection == 1 && menuIndex == 2) attron(A_REVERSE);
    mvprintw(row, col + 38, "[MAPPING: %s]", mapName.c_str());
    attrset(A_NORMAL);

    attron(A_DIM);
    if (menuSelection == 0) {
        mvprintw(row, col + 62, "(TAB: Menu | ARROWS: Navigate)");
    } else {
        mvprintw(row, col + 62, "(LEFT/RIGHT: Switch | TAB: Params)");
    }
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
        menuSelection = (menuSelection == 0) ? 1 : 0;
        return;
    }

    if (menuSelection == 1) {
        if (ch == KEY_UP) {
            menuIndex = (menuIndex - 1 + 3) % 3;
            return;
        } else if (ch == KEY_DOWN) {
            menuIndex = (menuIndex + 1) % 3;
            return;
        }

        if (ch == KEY_LEFT || ch == KEY_RIGHT) {
            if (menuIndex == 0 && machineManager) {
                int mc = machineManager->getMachineCount();
                int curr = machineManager->getCurrentMachineIndex();
                if (mc > 0) {
                    if (ch == KEY_LEFT) {
                        machineManager->setCurrentMachine((curr - 1 + mc) % mc);
                    } else {
                        machineManager->setCurrentMachine((curr + 1) % mc);
                    }
                }
            } else if (menuIndex == 1 && midiInput) {
                int dc = midiInput->getDeviceCount();
                if (dc > 0) {
                    if (ch == KEY_LEFT) {
                        midiDeviceIndex = (midiDeviceIndex - 1 + dc) % dc;
                    } else {
                        midiDeviceIndex = (midiDeviceIndex + 1) % dc;
                    }
                    midiInput->selectDevice(midiDeviceIndex);
                    midiInput->setMappingMachine(machine);
                    if (!midiInput->isRunning()) {
                        midiInput->start();
                    }
                }
            } else if (menuIndex == 2 && midiInput) {
                auto* mm = midiInput->getMappingManager();
                if (mm && mm->getMappingCount() > 0) {
                    int mc = mm->getMappingCount();
                    if (ch == KEY_LEFT) {
                        mm->setCurrentMapping((mm->getCurrentMappingIndex() - 1 + mc) % mc);
                    } else {
                        mm->setCurrentMapping((mm->getCurrentMappingIndex() + 1) % mc);
                    }
                }
            }
        }
        return;
    }

    if (menuSelection == 0) {
        handleNavigation(ch);
        if (ch >= '0' && ch <= '9') {
            float val = (ch == '0') ? 1.0f : (ch - '0') / 10.0f;
            if (selectedControl >= 0 && selectedControl < (int)controls.size()) {
                controls[selectedControl].value = val;
                machine->setI(controls[selectedControl].param, (int)(val * 128));
            }
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

    const auto& c = controls[selectedControl];
    const char* displayStr = machine ? machine->getDisplayString(c.param) : nullptr;

    if (displayStr && displayStr[0] != '\0') {
        int raw = machine->getI(c.param);
        if (ch == KEY_RIGHT) raw++;
        else if (ch == KEY_LEFT) raw--;
        if (raw < 0) raw = 0;
        machine->setI(c.param, raw);
        controls[selectedControl].value = (float)raw / 128.0f;
        return;
    }

    float& val = controls[selectedControl].value;
    float step = (c.maxVal - c.minVal) / 20.0f;

    if (ch == KEY_RIGHT) {
        val = std::min(val + step, c.maxVal);
    } else if (ch == KEY_LEFT) {
        val = std::max(val - step, c.minVal);
    }

    machine->setI(c.param, (int)(val * 128));
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

MappingManager* MachineUI::getMappingManager() {
    return midiInput ? midiInput->getMappingManager() : nullptr;
}

void MachineUI::stop() {
    endwin();
}
