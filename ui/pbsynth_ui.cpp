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

    controls.push_back({"OSC1 WAVE", PBSynthParam::P_OSC1_WAVEFORM, 0, 2, 0, 0, 3});
    controls.push_back({"OSC1 DETUNE", PBSynthParam::P_OSC1_DETUNE, 1, 2, 0.5, 0, 1});
    controls.push_back({"OSC1 SCALE", PBSynthParam::P_OSC1_SCALE, 2, 2, 0, -24, 24});

    controls.push_back({"OSC2 WAVE", PBSynthParam::P_OSC2_WAVEFORM, 4, 2, 0, 0, 3});
    controls.push_back({"OSC2 DETUNE", PBSynthParam::P_OSC2_DETUNE, 5, 2, 0.5, 0, 1});
    controls.push_back({"OSC2 SCALE", PBSynthParam::P_OSC2_SCALE, 6, 2, 0, -24, 24});

    controls.push_back({"CUTOFF", PBSynthParam::P_FILTER_CUTOFF, 0, 40, 0.5, 0, 1});
    controls.push_back({"RESONANCE", PBSynthParam::P_FILTER_RESONANCE, 1, 40, 0.2, 0, 1});

    controls.push_back({"FENV ATTACK", PBSynthParam::P_FILTER_ENV_ATTACK, 3, 40, 0.01, 0, 1});
    controls.push_back({"FENV DECAY", PBSynthParam::P_FILTER_ENV_DECAY, 4, 40, 0.3, 0, 1});
    controls.push_back({"FENV SUSTAIN", PBSynthParam::P_FILTER_ENV_SUSTAIN, 5, 40, 0.5, 0, 1});
    controls.push_back({"FENV RELEASE", PBSynthParam::P_FILTER_ENV_RELEASE, 6, 40, 0.3, 0, 1});
    controls.push_back({"FENV DEPTH", PBSynthParam::P_FILTER_ENV_DEPTH, 7, 40, 0.5, 0, 1});

    controls.push_back({"AENV ATTACK", PBSynthParam::P_AMP_ATTACK, 0, 78, 0.01, 0, 1});
    controls.push_back({"AENV DECAY", PBSynthParam::P_AMP_DECAY, 1, 78, 0.3, 0, 1});
    controls.push_back({"AENV SUSTAIN", PBSynthParam::P_AMP_SUSTAIN, 2, 78, 0.7, 0, 1});
    controls.push_back({"AENV RELEASE", PBSynthParam::P_AMP_RELEASE, 3, 78, 0.3, 0, 1});

    controls.push_back({"LFO1 FREQ", PBSynthParam::P_LFO1_FREQ, 5, 78, 0.1, 0, 1});
    controls.push_back({"LFO1 DEPTH", PBSynthParam::P_LFO1_DEPTH, 6, 78, 0, 0, 1});

    controls.push_back({"LFO2 FREQ", PBSynthParam::P_LFO2_FREQ, 8, 78, 0.1, 0, 1});
    controls.push_back({"LFO2 DEPTH", PBSynthParam::P_LFO2_DEPTH, 9, 78, 0, 0, 1});

    controls.push_back({"OSC MIX", PBSynthParam::P_OSC_MIX, 12, 78, 0.5, 0, 1});
    controls.push_back({"MASTER", PBSynthParam::P_MASTER_VOL, 13, 78, 0.7, 0, 1});
}
