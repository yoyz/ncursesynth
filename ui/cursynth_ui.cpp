#include "cursynth_ui.h"
#include "ui_layout.h"
#include <cmath>
#include <cstring>

CursynthUI::CursynthUI(Machine* mach, MachineManager* mgr)
    : MachineUI(mach, mgr) {
    columnTitles[0] = "OSCILLATORS";
    columnTitles[1] = "FILTER / ENVELOPES";
    columnTitles[2] = "LFO";
    init();
}

CursynthUI::~CursynthUI() {}

void CursynthUI::initControls() {
    controls.clear();

    controls.push_back({"OSC1 WAVE", 20, 0, 2, 4.0f / 11.0f, 0.0f, 11.0f});
    controls.push_back({"OSC1 DETUNE", 74, 1, 2, 0.5f, 0.0f, 1.0f});
    controls.push_back({"OSC1 SCALE", 65, 2, 2, 0.0f, 0.0f, 1.0f});

    controls.push_back({"OSC2 WAVE", 21, 4, 2, 4.0f / 11.0f, 0.0f, 11.0f});
    controls.push_back({"OSC2 DETUNE", 75, 5, 2, 0.5f, 0.0f, 1.0f});
    controls.push_back({"OSC2 SCALE", 66, 6, 2, 0.0f, 0.0f, 1.0f});

    controls.push_back({"OSC MIX", 28, 8, 2, 0.5f, 0.0f, 1.0f});

    controls.push_back({"CUTOFF", 52, 0, 40, 0.8f, 0.0f, 1.0f});
    controls.push_back({"RESONANCE", 53, 1, 40, 0.2f, 0.0f, 1.0f});
    controls.push_back({"FENV DEPTH", 94, 2, 40, 0.5f, 0.0f, 1.0f});

    controls.push_back({"AENV ATTACK", 0, 4, 40, 0.01f, 0.0f, 1.0f});
    controls.push_back({"AENV DECAY", 1, 5, 40, 0.3f, 0.0f, 1.0f});
    controls.push_back({"AENV SUSTAIN", 2, 6, 40, 0.7f, 0.0f, 1.0f});
    controls.push_back({"AENV RELEASE", 3, 7, 40, 0.3f, 0.0f, 1.0f});

    controls.push_back({"FENV ATTACK", 4, 9, 40, 0.01f, 0.0f, 1.0f});
    controls.push_back({"FENV DECAY", 5, 10, 40, 0.3f, 0.0f, 1.0f});
    controls.push_back({"FENV SUSTAIN", 6, 11, 40, 0.5f, 0.0f, 1.0f});
    controls.push_back({"FENV RELEASE", 7, 12, 40, 0.3f, 0.0f, 1.0f});

    controls.push_back({"LFO1 WAVE", 2, 0, 78, 0.0f, 0.0f, 11.0f});
    controls.push_back({"LFO1 FREQ", 42, 1, 78, 0.1f, 0.0f, 1.0f});
    controls.push_back({"LFO1 DEPTH", 41, 2, 78, 0.0f, 0.0f, 1.0f});

    controls.push_back({"VOLUME", 35, 4, 78, 0.7f, 0.0f, 1.0f});
}
