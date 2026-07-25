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
    widgets.clear();

    widgets.emplace_back("OSC1 WAVE", 20, 0, 2, WidgetType::DISCRETE, 0.25f, 12);
    widgets.emplace_back("OSC1 DETUNE", 74, 1, 2, WidgetType::BIPOLAR, 0.5f);
    widgets.emplace_back("OSC1 SCALE", 65, 2, 2, WidgetType::DISCRETE, 0.5f, 5);
    widgets.emplace_back("OSC1 MOD", 29, 3, 2, WidgetType::CONTINUOUS, 0.0f);
    widgets.emplace_back("OSC1 UNISON", 76, 4, 2, WidgetType::CONTINUOUS, 0.0f);
    widgets.emplace_back("OSC1 UNIS DT", 78, 5, 2, WidgetType::CONTINUOUS, 0.5f);

    widgets.emplace_back("OSC2 WAVE", 21, 7, 2, WidgetType::DISCRETE, 0.25f, 12);
    widgets.emplace_back("OSC2 DETUNE", 75, 8, 2, WidgetType::BIPOLAR, 0.5f);
    widgets.emplace_back("OSC2 SCALE", 66, 9, 2, WidgetType::DISCRETE, 0.5f, 5);
    widgets.emplace_back("OSC2 MOD", 29, 10, 2, WidgetType::CONTINUOUS, 0.0f);
    widgets.emplace_back("OSC2 UNISON", 77, 11, 2, WidgetType::CONTINUOUS, 0.0f);
    widgets.emplace_back("OSC2 UNIS DT", 79, 12, 2, WidgetType::CONTINUOUS, 0.5f);

    widgets.emplace_back("SUB WAVE", 22, 14, 2, WidgetType::DISCRETE, 0.0f, 11);
    widgets.emplace_back("SUB AMP", 33, 15, 2, WidgetType::CONTINUOUS, 0.0f);
    widgets.emplace_back("NOISE WAVE", 23, 16, 2, WidgetType::DISCRETE, 0.0f, 1);
    widgets.emplace_back("NOISE AMP", 34, 17, 2, WidgetType::CONTINUOUS, 0.0f);

    widgets.emplace_back("CUTOFF", 52, 0, 40, WidgetType::CONTINUOUS, 0.8f);
    widgets.emplace_back("RESONANCE", 53, 1, 40, WidgetType::CONTINUOUS, 0.0f);
    widgets.emplace_back("FILT DRIVE", 54, 2, 40, WidgetType::CONTINUOUS, 0.0f);
    widgets.emplace_back("DISTORTION", 56, 3, 40, WidgetType::CONTINUOUS, 0.0f);

    widgets.emplace_back("FENV ATTACK", 4, 5, 40, WidgetType::CONTINUOUS, 0.01f);
    widgets.emplace_back("FENV DECAY", 5, 6, 40, WidgetType::CONTINUOUS, 0.3f);
    widgets.emplace_back("FENV SUSTAIN", 6, 7, 40, WidgetType::CONTINUOUS, 0.5f);
    widgets.emplace_back("FENV RELEASE", 7, 8, 40, WidgetType::CONTINUOUS, 0.3f);
    widgets.emplace_back("FENV DEPTH", 94, 9, 40, WidgetType::BIPOLAR, 0.5f);

    widgets.emplace_back("KEYTRACK", 55, 11, 40, WidgetType::CONTINUOUS, 0.5f);
    widgets.emplace_back("VELOCITY", 140, 12, 40, WidgetType::CONTINUOUS, 0.5f);

    widgets.emplace_back("AENV ATTACK", 0, 0, 78, WidgetType::CONTINUOUS, 0.01f);
    widgets.emplace_back("AENV DECAY", 1, 1, 78, WidgetType::CONTINUOUS, 0.6f);
    widgets.emplace_back("AENV SUSTAIN", 2, 2, 78, WidgetType::CONTINUOUS, 0.6f);
    widgets.emplace_back("AENV RELEASE", 3, 3, 78, WidgetType::CONTINUOUS, 0.5f);

    widgets.emplace_back("LFO1 WAVE", 2, 5, 78, WidgetType::DISCRETE, 0.0f, 12);
    widgets.emplace_back("LFO1 FREQ", 42, 6, 78, WidgetType::CONTINUOUS, 0.1f);
    widgets.emplace_back("LFO1 DEPTH", 41, 7, 78, WidgetType::CONTINUOUS, 0.0f);

    widgets.emplace_back("LFO2 WAVE", 3, 9, 78, WidgetType::DISCRETE, 0.0f, 12);
    widgets.emplace_back("LFO2 FREQ", 44, 10, 78, WidgetType::CONTINUOUS, 0.1f);
    widgets.emplace_back("LFO2 DEPTH", 43, 11, 78, WidgetType::CONTINUOUS, 0.0f);

    widgets.emplace_back("OSC MIX", 28, 14, 78, WidgetType::CONTINUOUS, 0.5f);
    widgets.emplace_back("MASTER", 35, 15, 78, WidgetType::CONTINUOUS, 0.8f);
}
