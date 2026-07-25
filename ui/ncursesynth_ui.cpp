#include "ncursesynth_ui.h"
#include "ui_layout.h"
#include "../machine/ParamID.h"
#include <cmath>
#include <cstring>

NcursesynthUI::NcursesynthUI(Machine* mach, MachineManager* mgr)
    : MachineUI(mach, mgr) {
    columnTitles[0] = "OSCILLATORS";
    columnTitles[1] = "FILTER";
    columnTitles[2] = "ENVELOPE";
    init();
}

NcursesynthUI::~NcursesynthUI() {}

void NcursesynthUI::initControls() {
    widgets.clear();

    widgets.emplace_back("OSC1 WAVE", ParamID::osc1_wave, 0, 2, WidgetType::DISCRETE, 0.0f, 3);
    widgets.emplace_back("OSC1 DETUNE", ParamID::osc1_detune, 1, 2, WidgetType::BIPOLAR, 0.5f);
    widgets.emplace_back("OSC1 SCALE", ParamID::osc1_scale, 2, 2, WidgetType::DISCRETE, 0.5f, 5);
    widgets.emplace_back("OSC1 AMP", ParamID::osc1_amp, 3, 2, WidgetType::CONTINUOUS, 1.0f);

    widgets.emplace_back("OSC2 WAVE", ParamID::osc2_wave, 5, 2, WidgetType::DISCRETE, 0.0f, 3);
    widgets.emplace_back("OSC2 DETUNE", ParamID::osc2_detune, 6, 2, WidgetType::BIPOLAR, 0.5f);
    widgets.emplace_back("OSC2 SCALE", ParamID::osc2_scale, 7, 2, WidgetType::DISCRETE, 0.5f, 5);
    widgets.emplace_back("OSC2 AMP", ParamID::osc2_amp, 8, 2, WidgetType::CONTINUOUS, 0.0f);

    widgets.emplace_back("CUTOFF", ParamID::cutoff, 0, 40, WidgetType::CONTINUOUS, 0.8f);
    widgets.emplace_back("RESONANCE", ParamID::resonance, 1, 40, WidgetType::CONTINUOUS, 0.0f);
    widgets.emplace_back("FILTER TYPE", ParamID::filter_type, 2, 40, WidgetType::DISCRETE, 0.0f, 15);

    widgets.emplace_back("FENV ATTACK", ParamID::flt_attack, 3, 40, WidgetType::CONTINUOUS, 0.1f);
    widgets.emplace_back("FENV DECAY", ParamID::flt_decay, 4, 40, WidgetType::CONTINUOUS, 0.3f);
    widgets.emplace_back("FENV SUSTAIN", ParamID::flt_sustain, 5, 40, WidgetType::CONTINUOUS, 0.5f);
    widgets.emplace_back("FENV RELEASE", ParamID::flt_release, 6, 40, WidgetType::CONTINUOUS, 0.3f);
    widgets.emplace_back("FENV DEPTH", ParamID::flt_env_depth, 7, 40, WidgetType::BIPOLAR, 0.5f);

    widgets.emplace_back("AENV ATTACK", ParamID::amp_attack, 8, 40, WidgetType::CONTINUOUS, 0.01f);
    widgets.emplace_back("AENV DECAY", ParamID::amp_decay, 9, 40, WidgetType::CONTINUOUS, 0.6f);
    widgets.emplace_back("AENV SUSTAIN", ParamID::amp_sustain, 10, 40, WidgetType::CONTINUOUS, 0.6f);
    widgets.emplace_back("AENV RELEASE", ParamID::amp_release, 11, 40, WidgetType::CONTINUOUS, 0.5f);

    widgets.emplace_back("VOLUME", ParamID::volume, 4, 78, WidgetType::CONTINUOUS, 0.8f);
}
