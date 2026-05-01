#include "pbsynth_ui.h"
#include "../machine/Machine.h"
#include "../midi/midi_input.h"
#include <cmath>
#include <cstring>

PBSynthUI::PBSynthUI(Machine* mach, MachineManager* mgr)
    : selectedControl(0), menuSelection(0), engineIndex(0), midiDeviceIndex(-1), machine(mach), machineManager(mgr), midiInput(nullptr), screenRows(0), screenCols(0),
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
    start_color();
    nodelay(stdscr, TRUE);  // Non-blocking input
    timeout(50);  // 50ms timeout for getch
    getmaxyx(stdscr, screenRows, screenCols);
}

void PBSynthUI::draw() {
    erase();

    // Get engine name from machine
    std::string engineName = machine ? machine->getName() : "SYNTH";
    
    // Menu bar at top (row 2)
    drawMenuBar();

    // Engine title below menu (row 4)
    attron(A_BOLD);
    mvprintw(4, 2, "%s ENGINE", engineName.c_str());
    attroff(A_BOLD);

    // Column headers (row 6)
    if (engineName == "PBSynth") {
        drawColumnHeader(2, "OSCILLATORS");
        drawColumnHeader(40, "FILTER");
        drawColumnHeader(78, "ENVELOPE / LFO");
    } else if (engineName == "Twytch") {
        drawColumnHeader(2, "OSCILLATORS");
        drawColumnHeader(40, "FILTER");
        drawColumnHeader(78, "ENVELOPE / LFO");
    } else if (engineName == "Cursynth") {
        drawColumnHeader(2, "OSCILLATORS");
        drawColumnHeader(40, "FILTER");
        drawColumnHeader(78, "ENVELOPE / LFO");
    } else if (engineName == "Ncursesynth") {
        drawColumnHeader(2, "OSCILLATORS");
        drawColumnHeader(40, "FILTER");
        drawColumnHeader(78, "ENVELOPE / LFO");
    } else {
        drawColumnHeader(2, "OSCILLATORS");
        drawColumnHeader(40, "FILTER");
        drawColumnHeader(78, "ENVELOPE / LFO");
    }

    // Draw controls
    for (size_t i = 0; i < controls.size(); i++) {
        drawControl(i, (int)i == selectedControl);
    }

    // Instructions
    attron(A_DIM);
    if (menuSelection == 0) {
        mvprintw(screenRows - 3, 2, "TAB: Menu | ARROWS: Navigate | 1-9: Set Value | PAGEUP/DOWN: Adjust | Q: Quit");
    } else if (menuSelection == 1) {
        mvprintw(screenRows - 3, 2, "LEFT/RIGHT: Switch Engine | TAB: Menu | Q: Quit");
    } else {
        mvprintw(screenRows - 3, 2, "LEFT/RIGHT: Switch MIDI | TAB: Menu | Q: Quit");
    }
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

void PBSynthUI::drawMenuBar() {
    int row = 2;
    int col = 2;
    
    // Highlight menu when selected
    if (menuSelection == 1) attron(A_REVERSE);
    mvprintw(row, col, "[ENGINE: %s]", machine ? machine->getName().c_str() : "None");
    if (menuSelection == 1) attroff(A_REVERSE);
    
    if (menuSelection == 2) attron(A_REVERSE);
    // Show MIDI device name
    std::string midiName = "None";
    if (midiInput && midiDeviceIndex >= 0) {
        int deviceCount = midiInput->getDeviceCount();
        // Only call getDeviceName if index is valid
        if (midiDeviceIndex < deviceCount) {
            std::string name = midiInput->getDeviceName(midiDeviceIndex);
            if (!name.empty()) {
                midiName = name;
                // Truncate long names
                if (midiName.length() > 20) {
                    midiName = midiName.substr(0, 17) + "...";
                }
            }
        }
    }
    mvprintw(row, col + 25, "[MIDI: %s]", midiName.c_str());
    if (menuSelection == 2) attroff(A_REVERSE);
    
    // Instructions
    attron(A_DIM);
    mvprintw(row, col + 50, "(TAB: Menu | ARROWS: Navigate)");
    attroff(A_DIM);
}

void PBSynthUI::drawColumnHeader(int col, const char* title) {
    attron(A_BOLD | A_UNDERLINE);
    mvprintw(6, col, "%s", title);
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
    char bar[20];
    char valueStr[20];
    memset(bar, ' ', 19);
    bar[19] = '\0';
    memset(valueStr, ' ', 19);
    valueStr[19] = '\0';

    int filled = (int)(value * 16);
    if (filled > 16) filled = 16;
    if (filled < 0) filled = 0;

    bar[0] = '[';
    for (int i = 1; i <= 16; i++) {
        if (i <= filled) {
            bar[i] = '#';
        } else {
            bar[i] = '-';
        }
    }
    bar[16] = ']';

    snprintf(valueStr, sizeof(valueStr), "%3d%%", (int)(value * 100));

    if (selected) {
        attron(A_BOLD);
    }
    mvprintw(row, col, "%-11s %-18s %s", name, bar, valueStr);
    if (selected) {
        attroff(A_BOLD);
    }
}

void PBSynthUI::handleInput(int ch) {
    // Menu navigation
    if (ch == '\t') {
        menuSelection = (menuSelection + 1) % 3;  // 0=params, 1=engine, 2=midi
        return;
    }
    
    // Handle menu selections
    if (menuSelection == 1 && machineManager) {
        switch (ch) {
            case KEY_LEFT:
            case KEY_RIGHT:
                int count = machineManager->getMachineCount();
                engineIndex = (ch == KEY_LEFT) ? (engineIndex - 1 + count) % count : (engineIndex + 1) % count;
                machineManager->setCurrentMachine(engineIndex);
                machine = machineManager->getMachine(engineIndex);
                initControls();
                break;
        }
        return;
    }
    
    if (menuSelection == 2) {
        // MIDI device selection
        if (!midiInput) return;
        
        int deviceCount = midiInput->getDeviceCount();
        
        switch (ch) {
            case KEY_LEFT:
                // Switch to previous device (wrapping around)
                if (deviceCount > 0) {
                    midiDeviceIndex = (midiDeviceIndex - 1 + deviceCount + 1) % (deviceCount + 1);
                    if (midiDeviceIndex == deviceCount) {
                        // "None" selected
                        midiInput->stop();
                        midiInput->selectDevice(-1);
                    } else {
                        midiInput->selectDevice(midiDeviceIndex);
                        midiInput->setMappingMachine(machine);
                        midiInput->start();
                    }
                }
                break;
            case KEY_RIGHT:
                // Switch to next device (wrapping around)
                if (deviceCount > 0) {
                    midiDeviceIndex = (midiDeviceIndex + 1) % (deviceCount + 1);
                    if (midiDeviceIndex == deviceCount) {
                        // "None" selected
                        midiInput->stop();
                        midiInput->selectDevice(-1);
                    } else {
                        midiInput->selectDevice(midiDeviceIndex);
                        midiInput->setMappingMachine(machine);
                        midiInput->start();
                    }
                }
                break;
        }
        return;
    }
    
    // Normal parameter navigation
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