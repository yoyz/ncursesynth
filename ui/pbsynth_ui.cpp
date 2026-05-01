#include "pbsynth_ui.h"
#include "../machine/Machine.h"
#include "../midi/midi_input.h"
#include <cmath>
#include <cstring>

PBSynthUI::PBSynthUI(Machine* mach, MachineManager* mgr)
    : MachineUI(mach, mgr) {
    initControls();
}

PBSynthUI::~PBSynthUI() {
}

void PBSynthUI::initControls() {
    controls.clear();

    controls.push_back({"OSC1 WAVE", PBSynthParam::P_OSC1_WAVEFORM, 6, 2, 0, 0, 3});
    controls.push_back({"OSC1 DETUNE", PBSynthParam::P_OSC1_DETUNE, 7, 2, 0.5, 0, 1});
    controls.push_back({"OSC1 SCALE", PBSynthParam::P_OSC1_SCALE, 8, 2, 0, -24, 24});
    controls.push_back({"OSC1 AMP", PBSynthParam::P_OSC1_AMP, 9, 2, 0.8, 0, 1});

    controls.push_back({"OSC2 WAVE", PBSynthParam::P_OSC2_WAVEFORM, 11, 2, 0, 0, 3});
    controls.push_back({"OSC2 DETUNE", PBSynthParam::P_OSC2_DETUNE, 12, 2, 0.5, 0, 1});
    controls.push_back({"OSC2 SCALE", PBSynthParam::P_OSC2_SCALE, 13, 2, 0, -24, 24});
    controls.push_back({"OSC2 AMP", PBSynthParam::P_OSC2_AMP, 14, 2, 0.8, 0, 1});

    controls.push_back({"CUTOFF", PBSynthParam::P_FILTER_CUTOFF, 6, 40, 0.5, 0, 1});
    controls.push_back({"RESONANCE", PBSynthParam::P_FILTER_RESONANCE, 7, 40, 0.2, 0, 1});

    controls.push_back({"FENV ATTACK", PBSynthParam::P_FILTER_ENV_ATTACK, 9, 40, 0.01, 0, 1});
    controls.push_back({"FENV DECAY", PBSynthParam::P_FILTER_ENV_DECAY, 10, 40, 0.3, 0, 1});
    controls.push_back({"FENV SUSTAIN", PBSynthParam::P_FILTER_ENV_SUSTAIN, 11, 40, 0.5, 0, 1});
    controls.push_back({"FENV RELEASE", PBSynthParam::P_FILTER_ENV_RELEASE, 12, 40, 0.3, 0, 1});
    controls.push_back({"FENV DEPTH", PBSynthParam::P_FILTER_ENV_DEPTH, 13, 40, 0.5, 0, 1});

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

    controls.push_back({"OSC MIX", PBSynthParam::P_OSC_MIX, 20, 78, 0.5, 0, 1});
    controls.push_back({"MASTER", PBSynthParam::P_MASTER_VOL, 21, 78, 0.7, 0, 1});

    for (auto& c : controls) {
        machine->setI(c.param, (int)(c.value * 128));
    }
}

void PBSynthUI::init() {
    MachineUI::init();
}

void PBSynthUI::draw() {
    erase();

    std::string engineName = machine ? machine->getName() : "SYNTH";

    drawMenuBar();

    attron(A_BOLD);
    mvprintw(4, 2, "%s ENGINE", engineName.c_str());
    attroff(A_BOLD);

    drawColumnHeader(2, "OSCILLATORS");
    drawColumnHeader(40, "FILTER");
    drawColumnHeader(78, "ENVELOPE / LFO");

    for (size_t i = 0; i < controls.size(); i++) {
        drawControl(i, (int)i == selectedControl);
    }

    attron(A_DIM);
    if (menuSelection == 0) {
        mvprintw(screenRows - 3, 2, "TAB: Menu | ARROWS: Navigate | 1-9: Set Value | PAGEUP/DOWN: Adjust | Q: Quit");
    } else if (menuSelection == 1) {
        mvprintw(screenRows - 3, 2, "LEFT/RIGHT: Switch Engine | TAB: Menu | Q: Quit");
    } else {
        mvprintw(screenRows - 3, 2, "LEFT/RIGHT: Switch MIDI | TAB: Menu | Q: Quit");
    }
    attroff(A_DIM);

    drawMidiMonitor();

    refresh();
}

void PBSynthUI::drawColumnHeader(int col, const char* title) {
    attron(A_BOLD | A_UNDERLINE);
    mvprintw(6, col, "%s", title);
    attroff(A_BOLD | A_UNDERLINE);
}

void PBSynthUI::drawControl(int index, bool selected) {
    const auto& c = controls[index];

    if (selected) {
        attron(A_REVERSE);
    }

    drawSlider(c.row, c.col, c.name.c_str(), c.value, selected);

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
        bar[i] = (i <= filled) ? '#' : '-';
    }
    bar[16] = ']';

    snprintf(valueStr, sizeof(valueStr), "%3d%%", (int)(value * 100));

    if (selected) attron(A_BOLD);
    mvprintw(row, col, "%-11s %-18s %s", name, bar, valueStr);
    if (selected) attroff(A_BOLD);
}

void PBSynthUI::handleInput(int ch) {
    if (ch == '\t') {
        menuSelection = (menuSelection + 1) % 3;
        return;
    }

    if (menuSelection == 1 && machineManager) {
        if (ch == KEY_LEFT || ch == KEY_RIGHT) {
            int count = machineManager->getMachineCount();
            int newIndex = (ch == KEY_LEFT) ? (engineIndex - 1 + count) % count : (engineIndex + 1) % count;
            machineManager->setCurrentMachine(newIndex);
            menuSelection = 0;
            selectedControl = 0;
        }
        return;
    }

    if (menuSelection == 2) {
        if (!midiInput) return;

        int deviceCount = midiInput->getDeviceCount();

        if (ch == KEY_LEFT || ch == KEY_RIGHT) {
            midiDeviceIndex = (midiDeviceIndex + (ch == KEY_RIGHT ? 1 : -1) + deviceCount + 1) % (deviceCount + 1);
            if (midiDeviceIndex == deviceCount) {
                midiInput->stop();
                midiInput->selectDevice(-1);
            } else {
                midiInput->selectDevice(midiDeviceIndex);
                midiInput->setMappingMachine(machine);
                midiInput->start();
            }
        }
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

void PBSynthUI::updateValues() {
    updateControlValues();
}

void PBSynthUI::updateControlValues() {
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