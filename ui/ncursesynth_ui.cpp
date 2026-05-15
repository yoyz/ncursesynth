#include "ncursesynth_ui.h"
#include "ui_layout.h"
#include <cmath>
#include <cstring>

NcursesynthUI::NcursesynthUI(Machine* mach, MachineManager* mgr)
    : MachineUI(mach, mgr) {
    columnTitles[0] = "OSCILLATORS";
    columnTitles[1] = "FILTER";
    columnTitles[2] = "ENVELOPE / LFO";
    init();
}

NcursesynthUI::~NcursesynthUI() {}

void NcursesynthUI::initControls() {
    controls.clear();

    controls.push_back({"OSC1 WAVE", 0, 0, 2, 0.5f, 0.0f, 1.0f});
    controls.push_back({"OSC1 DETUNE", 74, 1, 2, 0.5f, 0.0f, 1.0f});
    controls.push_back({"OSC1 SCALE", 65, 2, 2, 0.0f, 0.0f, 1.0f});
    controls.push_back({"OSC1 AMP", 31, 3, 2, 0.8f, 0.0f, 1.0f});

    controls.push_back({"OSC2 WAVE", 4, 5, 2, 0.5f, 0.0f, 1.0f});
    controls.push_back({"OSC2 DETUNE", 75, 6, 2, 0.5f, 0.0f, 1.0f});
    controls.push_back({"OSC2 SCALE", 66, 7, 2, 0.0f, 0.0f, 1.0f});
    controls.push_back({"OSC2 AMP", 32, 8, 2, 0.8f, 0.0f, 1.0f});

    controls.push_back({"CUTOFF", 51, 0, 40, 0.8f, 0.0f, 1.0f});
    controls.push_back({"RESONANCE", 52, 1, 40, 0.2f, 0.0f, 1.0f});

    controls.push_back({"FENV ATTACK", 90, 3, 40, 0.1f, 0.0f, 1.0f});
    controls.push_back({"FENV DECAY", 91, 4, 40, 0.3f, 0.0f, 1.0f});
    controls.push_back({"FENV SUSTAIN", 92, 5, 40, 0.5f, 0.0f, 1.0f});
    controls.push_back({"FENV RELEASE", 93, 6, 40, 0.3f, 0.0f, 1.0f});

    controls.push_back({"AENV ATTACK", 80, 8, 40, 0.01f, 0.0f, 1.0f});
    controls.push_back({"AENV DECAY", 81, 9, 40, 0.3f, 0.0f, 1.0f});
    controls.push_back({"AENV SUSTAIN", 82, 10, 40, 0.7f, 0.0f, 1.0f});
    controls.push_back({"AENV RELEASE", 83, 11, 40, 0.3f, 0.0f, 1.0f});

    controls.push_back({"LFO1 WAVE", 20, 0, 78, 0.0f, 0.0f, 1.0f});
    controls.push_back({"LFO1 FREQ", 21, 1, 78, 0.1f, 0.0f, 1.0f});
    controls.push_back({"LFO1 DEPTH", 22, 2, 78, 0.0f, 0.0f, 1.0f});

    controls.push_back({"VOLUME", 35, 4, 78, 0.7f, 0.0f, 1.0f});
}
