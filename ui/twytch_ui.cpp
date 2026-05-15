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

    controls.push_back({"OSC1 WAVE", TwytchParam::P_OSC1_WAVEFORM, 0, 2, 0, 0, 11});
    controls.push_back({"OSC1 DETUNE", TwytchParam::P_OSC1_DETUNE, 1, 2, 0.5, 0, 1});
    controls.push_back({"OSC1 SCALE", TwytchParam::P_OSC1_SCALE, 2, 2, 0.5, 0, 1});
    controls.push_back({"OSC1 MOD", TwytchParam::P_OSC1_MOD, 3, 2, 0, 0, 1});
    controls.push_back({"OSC1 UNISON", TwytchParam::P_OSC1_UNISON, 4, 2, 0, 0, 1});
    controls.push_back({"OSC1 UNIS DT", TwytchParam::P_OSC1_UNISONDT, 5, 2, 0.5, 0, 1});

    controls.push_back({"OSC2 WAVE", TwytchParam::P_OSC2_WAVEFORM, 7, 2, 0, 0, 11});
    controls.push_back({"OSC2 DETUNE", TwytchParam::P_OSC2_DETUNE, 8, 2, 0.5, 0, 1});
    controls.push_back({"OSC2 SCALE", TwytchParam::P_OSC2_SCALE, 9, 2, 0.5, 0, 1});
    controls.push_back({"OSC2 MOD", TwytchParam::P_OSC2_MOD, 10, 2, 0, 0, 1});
    controls.push_back({"OSC2 UNISON", TwytchParam::P_OSC2_UNISON, 11, 2, 0, 0, 1});
    controls.push_back({"OSC2 UNIS DT", TwytchParam::P_OSC2_UNISONDT, 12, 2, 0.5, 0, 1});

    controls.push_back({"SUB WAVE", TwytchParam::P_OSC3_WAVEFORM, 14, 2, 0, 0, 3});
    controls.push_back({"SUB AMP", TwytchParam::P_OSC3_AMP, 15, 2, 0, 0, 1});
    controls.push_back({"NOISE WAVE", TwytchParam::P_OSC4_WAVEFORM, 16, 2, 0, 0, 3});
    controls.push_back({"NOISE AMP", TwytchParam::P_OSC4_AMP, 17, 2, 0, 0, 1});

    controls.push_back({"CUTOFF", TwytchParam::P_FILTER_CUTOFF, 0, 40, 0.5, 0, 1});
    controls.push_back({"RESONANCE", TwytchParam::P_FILTER_RESONANCE, 1, 40, 0.2, 0, 1});
    controls.push_back({"FILT DRIVE", TwytchParam::P_FILTER_SATURATION, 2, 40, 0, 0, 1});
    controls.push_back({"DISTORTION", TwytchParam::P_FILTER_FEEDBACK, 3, 40, 0, 0, 1});

    controls.push_back({"FENV ATTACK", TwytchParam::P_FILTER_ENV_ATTACK, 5, 40, 0.01, 0, 1});
    controls.push_back({"FENV DECAY", TwytchParam::P_FILTER_ENV_DECAY, 6, 40, 0.3, 0, 1});
    controls.push_back({"FENV SUSTAIN", TwytchParam::P_FILTER_ENV_SUSTAIN, 7, 40, 0.5, 0, 1});
    controls.push_back({"FENV RELEASE", TwytchParam::P_FILTER_ENV_RELEASE, 8, 40, 0.3, 0, 1});
    controls.push_back({"FENV DEPTH", TwytchParam::P_FILTER_ENV_DEPTH, 9, 40, 0.5, 0, 1});

    controls.push_back({"KEYTRACK", TwytchParam::P_KEYTRACK, 11, 40, 0.5, 0, 1});
    controls.push_back({"VELOCITY", TwytchParam::P_VELOCITY, 12, 40, 0.5, 0, 1});

    controls.push_back({"AENV ATTACK", TwytchParam::P_AMP_ATTACK, 0, 78, 0.01, 0, 1});
    controls.push_back({"AENV DECAY", TwytchParam::P_AMP_DECAY, 1, 78, 0.3, 0, 1});
    controls.push_back({"AENV SUSTAIN", TwytchParam::P_AMP_SUSTAIN, 2, 78, 0.7, 0, 1});
    controls.push_back({"AENV RELEASE", TwytchParam::P_AMP_RELEASE, 3, 78, 0.3, 0, 1});

    controls.push_back({"LFO1 WAVE", TwytchParam::P_LFO1_WAVEFORM, 5, 78, 0, 0, 3});
    controls.push_back({"LFO1 FREQ", TwytchParam::P_LFO1_FREQ, 6, 78, 0.1, 0, 1});
    controls.push_back({"LFO1 DEPTH", TwytchParam::P_LFO1_DEPTH, 7, 78, 0, 0, 1});

    controls.push_back({"LFO2 WAVE", TwytchParam::P_LFO2_WAVEFORM, 9, 78, 0, 0, 3});
    controls.push_back({"LFO2 FREQ", TwytchParam::P_LFO2_FREQ, 10, 78, 0.1, 0, 1});
    controls.push_back({"LFO2 DEPTH", TwytchParam::P_LFO2_DEPTH, 11, 78, 0, 0, 1});

    controls.push_back({"OSC MIX", TwytchParam::P_OSC_MIX, 14, 78, 0.5, 0, 1});
    controls.push_back({"MASTER", TwytchParam::P_MASTER_VOL, 15, 78, 0.7, 0, 1});

    for (auto& c : controls) {
        machine->setI(c.param, (int)(c.value * 128));
    }
}
