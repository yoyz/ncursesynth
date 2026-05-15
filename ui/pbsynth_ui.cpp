#include "pbsynth_ui.h"
#include "../machine/Machine.h"
#include <cmath>
#include <cstring>

PBSynthUI::PBSynthUI(Machine* mach, MachineManager* mgr)
    : MachineUI(mach, mgr) {
    columnTitles[0] = "OSCILLATORS";
    columnTitles[1] = "FILTER";
    columnTitles[2] = "ENVELOPE / LFO";
    init();
}

PBSynthUI::~PBSynthUI() {
}

void PBSynthUI::initControls() {
    controls.clear();

    controls.push_back({"OSC1 WAVE", 20, 0, 2, 0, 0, 3});
    controls.push_back({"OSC1 DETUNE", 74, 1, 2, 0.5, 0, 1});
    controls.push_back({"OSC1 SCALE", 65, 2, 2, 0, -24, 24});

    controls.push_back({"OSC2 WAVE", 21, 4, 2, 0, 0, 3});
    controls.push_back({"OSC2 DETUNE", 75, 5, 2, 0.5, 0, 1});
    controls.push_back({"OSC2 SCALE", 66, 6, 2, 0, -24, 24});

    controls.push_back({"CUTOFF", 52, 0, 40, 0.5, 0, 1});
    controls.push_back({"RESONANCE", 53, 1, 40, 0.2, 0, 1});

    controls.push_back({"FENV ATTACK", 4, 3, 40, 0.01, 0, 1});
    controls.push_back({"FENV DECAY", 5, 4, 40, 0.3, 0, 1});
    controls.push_back({"FENV SUSTAIN", 6, 5, 40, 0.5, 0, 1});
    controls.push_back({"FENV RELEASE", 7, 6, 40, 0.3, 0, 1});
    controls.push_back({"FENV DEPTH", 94, 7, 40, 0.5, 0, 1});

    controls.push_back({"AENV ATTACK", 0, 0, 78, 0.01, 0, 1});
    controls.push_back({"AENV DECAY", 1, 1, 78, 0.3, 0, 1});
    controls.push_back({"AENV SUSTAIN", 2, 2, 78, 0.7, 0, 1});
    controls.push_back({"AENV RELEASE", 3, 3, 78, 0.3, 0, 1});

    controls.push_back({"LFO1 FREQ", 42, 5, 78, 0.1, 0, 1});
    controls.push_back({"LFO1 DEPTH", 41, 6, 78, 0, 0, 1});

    controls.push_back({"LFO2 FREQ", 44, 8, 78, 0.1, 0, 1});
    controls.push_back({"LFO2 DEPTH", 43, 9, 78, 0, 0, 1});

    controls.push_back({"OSC MIX", 28, 12, 78, 0.5, 0, 1});
    controls.push_back({"MASTER", 35, 13, 78, 0.7, 0, 1});
}
