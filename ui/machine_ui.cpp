#include "machine_ui.h"
#include "ui_layout.h"
#include "../midi/midi_input.h"
#include "../midi/midi_mapping.h"
#include "../audio/audio_level.h"
#include <cmath>
#include <cstring>
#include <cctype>
#include <chrono>

MachineUI::MachineUI(Machine* mach, MachineManager* mgr)
    : selectedControl(0), machine(mach), machineManager(mgr),
      midiInput(nullptr), screenRows(0), screenCols(0),
      lastMidiNote(-1), lastMidiVel(0), midiActivity(false),
      menuSelection(0), menuIndex(0), midiDeviceIndex(-1), mappingIndex(0),
      presetIndex(0), presetInputMode(false) {
    columnTitles[0] = "OSCILLATORS";
    columnTitles[1] = "FILTER";
    columnTitles[2] = "ENVELOPE";
    scanPresets();
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
        updateControlValues();
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
    if (presetInputMode) {
        mvprintw(screenRows - 2, 2, "Enter preset name: %s_", presetInputBuffer.c_str());
        mvprintw(screenRows - 3, 2, "Press ENTER to save, ESC to cancel");
    } else if (menuSelection == 0) {
        mvprintw(screenRows - 3, 2, "TAB: Menu | ARROWS: Navigate | 1-9: Set | PGUP/DN: Adjust | S:Save | C:Create | Q:Quit");
    } else if (menuSelection == 1) {
        mvprintw(screenRows - 3, 2, "LEFT/RIGHT: Switch | TAB: Params | Q: Quit");
    }
    attroff(A_DIM);

    drawMidiMonitor();

    int lvlRow = screenRows - 4;
    float peak = AudioLevel::getPeak();
    int barLen = (int)(peak * 16.0f);
    if (barLen < 0) barLen = 0;
    if (barLen > 16) barLen = 16;
    int warn = (peak > 0.85f) ? A_REVERSE : A_DIM;
    attron(warn);
    mvprintw(lvlRow, 2, "LVL [");
    for (int i = 0; i < 16; i++) {
        if (i < barLen)
            mvaddch(lvlRow, 7 + i, '#');
        else
            mvaddch(lvlRow, 7 + i, '-');
    }
    mvprintw(lvlRow, 23, "] %3d%%  peak", (int)(peak * 100));
    attroff(warn);

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
        int dc = midiInput->getDeviceCount();
        if (midiDeviceIndex < dc) {
            std::string n = midiInput->getDeviceName(midiDeviceIndex);
            if (!n.empty()) {
                midiName = n;
                if (midiName.length() > 16) midiName = midiName.substr(0, 13) + "...";
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
            if (mapName.length() > 16) mapName = mapName.substr(0, 13) + "...";
        }
    }
    attrset(A_NORMAL);
    if (menuSelection == 1 && menuIndex == 2) attron(A_REVERSE);
    mvprintw(row, col + 38, "[MAPPING: %s]", mapName.c_str());
    attrset(A_NORMAL);

    std::string presetName = "Init";
    if (!presets.empty() && presetIndex >= 0 && presetIndex < (int)presets.size())
        presetName = presets[presetIndex].name;
    if (presetName.length() > 16) presetName = presetName.substr(0, 13) + "...";
    attrset(A_NORMAL);
    if (menuSelection == 1 && menuIndex == 3) attron(A_REVERSE);
    mvprintw(row, col + 60, "[PRESET: %s]", presetName.c_str());
    attrset(A_NORMAL);

    attron(A_DIM);
    if (menuSelection == 0)
        mvprintw(row, col + 82, "(TAB: Menu | ARROWS: Navigate | S: Save)");
    else
        mvprintw(row, col + 82, "(LEFT/RIGHT: Switch | TAB: Params)");
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
        attron(A_BOLD);
        mvprintw(row, 2, "MIDI: CC%3d = %3d   ", lastCC, lastCCValue);
        attroff(A_BOLD);

        for (int i = 20; i < screenCols - 1; i++) {
            mvaddch(row, i, ' ');
        }
    } else if (midiActivity) {
        attron(A_BOLD);
        mvprintw(row, 2, "MIDI: Note=%d Vel=%d  ", lastMidiNote, lastMidiVel);
        attroff(A_BOLD);

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
    if (presetInputMode) {
        if (ch == '\n' || ch == KEY_ENTER) {
            if (!presetInputBuffer.empty()) {
                savePreset(presetInputBuffer);
            }
            presetInputMode = false;
            presetInputBuffer.clear();
        } else if (ch == 27 || ch == '\t') {
            presetInputMode = false;
            presetInputBuffer.clear();
        } else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
            if (!presetInputBuffer.empty())
                presetInputBuffer.pop_back();
        } else if (ch >= 32 && ch <= 126) {
            if (presetInputBuffer.length() < 40)
                presetInputBuffer += (char)ch;
        }
        return;
    }

    if (ch == '\t') {
        menuSelection = (menuSelection == 0) ? 1 : 0;
        return;
    }

    if (menuSelection == 1) {
        if (ch == KEY_UP) {
            menuIndex = (menuIndex - 1 + 4) % 4;
            return;
        } else if (ch == KEY_DOWN) {
            menuIndex = (menuIndex + 1) % 4;
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
            } else if (menuIndex == 3) {
                int pc = (int)presets.size();
                if (pc > 0) {
                    if (ch == KEY_LEFT) {
                        int newIdx = (presetIndex - 1 + pc) % pc;
                        loadPreset(newIdx);
                    } else {
                        int newIdx = (presetIndex + 1) % pc;
                        loadPreset(newIdx);
                    }
                }
            }
        }
        return;
    }

    if (menuSelection == 0) {
        if (ch == 's' || ch == 'S') {
            std::string name = getCurrentPresetName();
            if (!name.empty() && name != "Init") {
                savePreset(name);
            }
            return;
        }
        if (ch == 'c' || ch == 'C') {
            presetInputMode = true;
            presetInputBuffer.clear();
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
        if (raw > 127) raw = 127;
        machine->setI(c.param, raw);
        controls[selectedControl].value = (float)raw / 128.0f;
        return;
    }

    float& val = controls[selectedControl].value;
    float step = (c.maxVal - c.minVal) / 100.0f;

    if (ch == KEY_RIGHT) {
        val = std::min(val + step, c.maxVal);
    } else if (ch == KEY_LEFT) {
        val = std::max(val - step, c.minVal);
    }

    int rawValue = (int)(val * 128);
    if (rawValue > 127) rawValue = 127;
    if (rawValue < 0) rawValue = 0;
    
    machine->setI(c.param, rawValue);
    controls[selectedControl].value = (float)rawValue / 128.0f;
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
            controls[selectedControl].value = std::min(controls[selectedControl].value + 0.10f, controls[selectedControl].maxVal);
            machine->setI(controls[selectedControl].param, (int)(controls[selectedControl].value * 128));
        }
    } else if (ch == KEY_NPAGE) {
        if (selectedControl >= 0 && selectedControl < (int)controls.size()) {
            controls[selectedControl].value = std::max(controls[selectedControl].value - 0.10f, controls[selectedControl].minVal);
            machine->setI(controls[selectedControl].param, (int)(controls[selectedControl].value * 128));
        }
    }
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

static std::string engineDir(const std::string& name) {
    std::string r = "bank/";
    for (char c : name) r += std::tolower(c);
    return r;
}

void MachineUI::scanPresets() {
    presets.clear();
    if (!machine) return;

    PresetInfo initPreset;
    initPreset.name = "Init";
    initPreset.path = "";
    presets.push_back(initPreset);

    std::string dir = engineDir(machine->getName());
    std::vector<std::string> files = Machine::getPresetList(machine->getName());
    for (const auto& f : files) {
        PresetInfo pi;
        pi.name = f;
        pi.path = dir + "/" + f;
        presets.push_back(pi);
    }

    if (presetIndex < 0) presetIndex = 0;
    if (presetIndex >= (int)presets.size())
        presetIndex = 0;
}

bool MachineUI::loadPreset(int index) {
    if (!machine) return false;
    if (index < 0 || index >= (int)presets.size()) return false;

    bool ok = true;
    if (index == 0 && presets[index].path.empty()) {
        machine->init();
        updateControlValues();
        presetIndex = 0;
    } else {
        ok = machine->loadPreset(presets[index].path);
        if (ok) {
            presetIndex = index;
            updateControlValues();
        } else {
            presetIndex = index;
        }
    }
    return ok;
}

bool MachineUI::savePreset(const std::string& name) {
    if (!machine) return false;

    std::string path = engineDir(machine->getName()) + "/" + name;
    bool ok = machine->savePreset(path);
    if (ok) {
        scanPresets();
        for (size_t i = 0; i < presets.size(); i++) {
            if (presets[i].name == name) {
                presetIndex = (int)i;
                break;
            }
        }
    }
    return ok;
}

const std::string& MachineUI::getCurrentPresetName() const {
    if (presets.empty()) {
        static std::string init = "Init";
        return init;
    }
    if (presetIndex >= 0 && presetIndex < (int)presets.size()) {
        return presets[presetIndex].name;
    }
    static std::string init = "Init";
    return init;
}

void MachineUI::stop() {
    endwin();
}
