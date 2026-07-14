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
    widgets.clear();

    widgets.emplace_back("OSC1 WAVE", 20, 0, 2, 0.0f);
    widgets.emplace_back("OSC1 DETUNE", 74, 1, 2, 0.5f);
    widgets.emplace_back("OSC1 SCALE", 65, 2, 2, 0.5f);

    widgets.emplace_back("OSC2 WAVE", 21, 4, 2, 0.0f);
    widgets.emplace_back("OSC2 DETUNE", 75, 5, 2, 0.5f);
    widgets.emplace_back("OSC2 SCALE", 66, 6, 2, 0.5f);

    widgets.emplace_back("CUTOFF", 52, 0, 40, 0.5f);
    widgets.emplace_back("RESONANCE", 53, 1, 40, 0.2f);

    widgets.emplace_back("FENV ATTACK", 4, 3, 40, 0.01f);
    widgets.emplace_back("FENV DECAY", 5, 4, 40, 0.3f);
    widgets.emplace_back("FENV SUSTAIN", 6, 5, 40, 0.5f);
    widgets.emplace_back("FENV RELEASE", 7, 6, 40, 0.3f);
    widgets.emplace_back("FENV DEPTH", 94, 7, 40, 0.5f);

    widgets.emplace_back("AENV ATTACK", 0, 0, 78, 0.01f);
    widgets.emplace_back("AENV DECAY", 1, 1, 78, 0.3f);
    widgets.emplace_back("AENV SUSTAIN", 2, 2, 78, 0.7f);
    widgets.emplace_back("AENV RELEASE", 3, 3, 78, 0.3f);

    widgets.emplace_back("LFO1 FREQ", 42, 5, 78, 0.1f);
    widgets.emplace_back("LFO1 DEPTH", 41, 6, 78, 0.0f);

    widgets.emplace_back("LFO2 FREQ", 44, 8, 78, 0.1f);
    widgets.emplace_back("LFO2 DEPTH", 43, 9, 78, 0.0f);

    widgets.emplace_back("OSC MIX", 28, 12, 78, 0.5f);
    widgets.emplace_back("MASTER", 35, 13, 78, 0.7f);
}
