#include "twytch_ui.h"
#include "../machine/Machine.h"
#include <cmath>
#include <cstring>

TwytchUI::TwytchUI(Machine* mach, MachineManager* mgr)
    : MachineUI(mach, mgr) {
    columnTitles[0] = "OSCILLATORS";
    columnTitles[1] = "FILTER";
    columnTitles[2] = "ENVELOPE / LFO";
    init();
}

TwytchUI::~TwytchUI() {
}

void TwytchUI::initControls() {
    controls.clear();

    controls.push_back({"OSC1 WAVE", 20, 0, 2, 0, 0, 11});
    controls.push_back({"OSC1 DETUNE", 74, 1, 2, 0.5, 0, 1});
    controls.push_back({"OSC1 SCALE", 65, 2, 2, 0.5, 0, 1});
    controls.push_back({"OSC1 MOD", 29, 3, 2, 0, 0, 1});
    controls.push_back({"OSC1 UNISON", 76, 4, 2, 0, 0, 1});
    controls.push_back({"OSC1 UNIS DT", 78, 5, 2, 0.5, 0, 1});

    controls.push_back({"OSC2 WAVE", 21, 7, 2, 0, 0, 11});
    controls.push_back({"OSC2 DETUNE", 75, 8, 2, 0.5, 0, 1});
    controls.push_back({"OSC2 SCALE", 66, 9, 2, 0.5, 0, 1});
    controls.push_back({"OSC2 MOD", 29, 10, 2, 0, 0, 1});
    controls.push_back({"OSC2 UNISON", 77, 11, 2, 0, 0, 1});
    controls.push_back({"OSC2 UNIS DT", 79, 12, 2, 0.5, 0, 1});

    controls.push_back({"SUB WAVE", 22, 14, 2, 0, 0, 3});
    controls.push_back({"SUB AMP", 33, 15, 2, 0, 0, 1});
    controls.push_back({"NOISE WAVE", 23, 16, 2, 0, 0, 3});
    controls.push_back({"NOISE AMP", 34, 17, 2, 0, 0, 1});

    controls.push_back({"CUTOFF", 52, 0, 40, 0.5, 0, 1});
    controls.push_back({"RESONANCE", 53, 1, 40, 0.2, 0, 1});
    controls.push_back({"FILT DRIVE", 54, 2, 40, 0, 0, 1});
    controls.push_back({"DISTORTION", 56, 3, 40, 0, 0, 1});

    controls.push_back({"FENV ATTACK", 4, 5, 40, 0.01, 0, 1});
    controls.push_back({"FENV DECAY", 5, 6, 40, 0.3, 0, 1});
    controls.push_back({"FENV SUSTAIN", 6, 7, 40, 0.5, 0, 1});
    controls.push_back({"FENV RELEASE", 7, 8, 40, 0.3, 0, 1});
    controls.push_back({"FENV DEPTH", 94, 9, 40, 0.5, 0, 1});

    controls.push_back({"KEYTRACK", 55, 11, 40, 0.5, 0, 1});
    controls.push_back({"VELOCITY", 140, 12, 40, 0.5, 0, 1});

    controls.push_back({"AENV ATTACK", 0, 0, 78, 0.01, 0, 1});
    controls.push_back({"AENV DECAY", 1, 1, 78, 0.3, 0, 1});
    controls.push_back({"AENV SUSTAIN", 2, 2, 78, 0.7, 0, 1});
    controls.push_back({"AENV RELEASE", 3, 3, 78, 0.3, 0, 1});

    controls.push_back({"LFO1 WAVE", 2, 5, 78, 0, 0, 3});
    controls.push_back({"LFO1 FREQ", 42, 6, 78, 0.1, 0, 1});
    controls.push_back({"LFO1 DEPTH", 41, 7, 78, 0, 0, 1});

    controls.push_back({"LFO2 WAVE", 3, 9, 78, 0, 0, 3});
    controls.push_back({"LFO2 FREQ", 44, 10, 78, 0.1, 0, 1});
    controls.push_back({"LFO2 DEPTH", 43, 11, 78, 0, 0, 1});

    controls.push_back({"OSC MIX", 28, 14, 78, 0.5, 0, 1});
    controls.push_back({"MASTER", 35, 15, 78, 0.7, 0, 1});

    for (auto& c : controls) {
        machine->setI(c.param, (int)(c.value * 128));
    }
}
