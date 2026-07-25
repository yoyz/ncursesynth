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
    widgets.clear();

    widgets.emplace_back("OSC1 WAVE", 20, 0, 2, WidgetType::DISCRETE, 4.0f / 11.0f, 12);
    widgets.emplace_back("OSC1 SCALE", 65, 1, 2, WidgetType::DISCRETE, 0.5f, 5);

    widgets.emplace_back("OSC2 WAVE", 21, 3, 2, WidgetType::DISCRETE, 4.0f / 11.0f, 12);
    widgets.emplace_back("OSC2 DETUNE", 75, 4, 2, WidgetType::BIPOLAR, 0.5f);
    widgets.emplace_back("OSC2 SCALE", 66, 5, 2, WidgetType::DISCRETE, 0.5f, 5);

    widgets.emplace_back("OSC MIX", 28, 7, 2, WidgetType::CONTINUOUS, 0.5f);

    widgets.emplace_back("CUTOFF", 52, 0, 40, WidgetType::CONTINUOUS, 0.8f);
    widgets.emplace_back("RESONANCE", 53, 1, 40, WidgetType::CONTINUOUS, 0.0f);
    widgets.emplace_back("FENV DEPTH", 94, 2, 40, WidgetType::BIPOLAR, 0.5f);

    widgets.emplace_back("AENV ATTACK", 0, 4, 40, WidgetType::CONTINUOUS, 0.01f);
    widgets.emplace_back("AENV DECAY", 1, 5, 40, WidgetType::CONTINUOUS, 0.6f);
    widgets.emplace_back("AENV SUSTAIN", 2, 6, 40, WidgetType::CONTINUOUS, 0.6f);
    widgets.emplace_back("AENV RELEASE", 3, 7, 40, WidgetType::CONTINUOUS, 0.5f);

    widgets.emplace_back("FENV ATTACK", 4, 9, 40, WidgetType::CONTINUOUS, 0.01f);
    widgets.emplace_back("FENV DECAY", 5, 10, 40, WidgetType::CONTINUOUS, 0.3f);
    widgets.emplace_back("FENV SUSTAIN", 6, 11, 40, WidgetType::CONTINUOUS, 0.5f);
    widgets.emplace_back("FENV RELEASE", 7, 12, 40, WidgetType::CONTINUOUS, 0.3f);

    widgets.emplace_back("LFO1 WAVE", 2, 0, 78, WidgetType::DISCRETE, 0.0f, 12);
    widgets.emplace_back("LFO1 FREQ", 42, 1, 78, WidgetType::CONTINUOUS, 0.1f);
    widgets.emplace_back("LFO1 DEPTH", 41, 2, 78, WidgetType::CONTINUOUS, 0.0f);

    widgets.emplace_back("VOLUME", 35, 4, 78, WidgetType::CONTINUOUS, 0.8f);
}
