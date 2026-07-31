#include "ambika_ui.h"
#include "../machine/ParamID.h"
#include "../machine/Ambika/ambika_types.h"

// Ambika parameter IDs use the shared unified scheme (ParamID) plus the
// Ambika extended IDs from ambika_types.h (100-109). AmbikaMachine::mapParam()
// translates these to native Ambika params.

void AmbikaUI::initControls()
{
    widgets.clear();

    // Column 1 - OSCILLATORS (x=2)
    widgets.push_back(Widget("OSC1 SHAPE",  ParamID::osc1_wave,  0, 2, WidgetType::DISCRETE,   1.0f / 34.0f, 35));
    widgets.push_back(Widget("OSC1 PARAM",  AMB_OSC1_PARAM,      1, 2, WidgetType::CONTINUOUS, 0.25f));
    widgets.push_back(Widget("OSC1 RANGE",  ParamID::osc1_scale, 2, 2, WidgetType::BIPOLAR,    0.5f));
    widgets.push_back(Widget("OSC1 DETUNE", ParamID::osc1_detune,3, 2, WidgetType::BIPOLAR,    0.5f));
    widgets.push_back(Widget("OSC2 SHAPE",  ParamID::osc2_wave,  4, 2, WidgetType::DISCRETE,   0.0f, 35));
    widgets.push_back(Widget("OSC2 PARAM",  AMB_OSC2_PARAM,      5, 2, WidgetType::CONTINUOUS, 0.0f));
    widgets.push_back(Widget("OSC2 RANGE",  ParamID::osc2_scale, 6, 2, WidgetType::BIPOLAR,    0.5f));
    widgets.push_back(Widget("OSC2 DETUNE", ParamID::osc2_detune,7, 2, WidgetType::BIPOLAR,    0.5f));
    widgets.push_back(Widget("VOLUME",      ParamID::volume,     8, 2, WidgetType::CONTINUOUS, 0.8f));

    // LFO (col 1, below the oscillators)
    widgets.push_back(Widget("LFO SHAPE", AMB_LFO_SHAPE,    9, 2, WidgetType::DISCRETE,   0.0f, 4));
    widgets.push_back(Widget("LFO RATE",  ParamID::lfo1_freq,10, 2, WidgetType::CONTINUOUS, 0.12f));

    // Column 2 - FILTER / MIX (x=40)
    widgets.push_back(Widget("CUTOFF",   ParamID::cutoff,       0, 40, WidgetType::CONTINUOUS, 0.75f));
    widgets.push_back(Widget("RESONANCE",ParamID::resonance,    1, 40, WidgetType::CONTINUOUS, 0.0f));
    widgets.push_back(Widget("F MODE",   AMB_FILTER_MODE,       2, 40, WidgetType::DISCRETE,   0.0f, 4));
    widgets.push_back(Widget("F TYPE",   ParamID::filter_type,  3, 40, WidgetType::DISCRETE,   0.0f, 2));
    widgets.push_back(Widget("BALANCE",  ParamID::mix,          4, 40, WidgetType::CONTINUOUS, 0.5f));
    widgets.push_back(Widget("MIX OP",   AMB_MIX_OP,            5, 40, WidgetType::DISCRETE,   0.0f, 6));
    widgets.push_back(Widget("SUB OSC",  AMB_MIX_SUB,           6, 40, WidgetType::CONTINUOUS, 0.0f));
    widgets.push_back(Widget("NOISE",    AMB_MIX_NOISE,         7, 40, WidgetType::CONTINUOUS, 0.0f));
    widgets.push_back(Widget("FUZZ",     AMB_MIX_FUZZ,          8, 40, WidgetType::CONTINUOUS, 0.0f));
    widgets.push_back(Widget("FENV AMT", ParamID::flt_env_depth, 9, 40, WidgetType::BIPOLAR,   0.5f));
    widgets.push_back(Widget("FLFO AMT", AMB_FLFO_AMT,         10, 40, WidgetType::BIPOLAR,   0.5f));
    widgets.push_back(Widget("PORTAM",   AMB_PORTAMENTO,       11, 40, WidgetType::CONTINUOUS, 0.0f));

    // Column 3 - ENVELOPES (x=78)
    // Filter Envelope (Env1) - modulates filter cutoff via mod matrix slot 1
    widgets.push_back(Widget("FLT ATK",  ParamID::flt_attack,   0, 78, WidgetType::CONTINUOUS, 0.0f));
    widgets.push_back(Widget("FLT DEC",  ParamID::flt_decay,    1, 78, WidgetType::CONTINUOUS, 0.6f));
    widgets.push_back(Widget("FLT SUS",  ParamID::flt_sustain,  2, 78, WidgetType::CONTINUOUS, 0.7f));
    widgets.push_back(Widget("FLT REL",  ParamID::flt_release,  3, 78, WidgetType::CONTINUOUS, 0.85f));

    // Amp Envelope (Env2) - controls VCA via mod matrix slot 5
    widgets.push_back(Widget("AMP ATK",  ParamID::amp_attack,   4, 78, WidgetType::CONTINUOUS, 0.0f));
    widgets.push_back(Widget("AMP DEC",  ParamID::amp_decay,    5, 78, WidgetType::CONTINUOUS, 0.6f));
    widgets.push_back(Widget("AMP SUS",  ParamID::amp_sustain,  6, 78, WidgetType::CONTINUOUS, 0.7f));
    widgets.push_back(Widget("AMP REL",  ParamID::amp_release,  7, 78, WidgetType::CONTINUOUS, 0.85f));

    // Mod Envelope (Env3) - routed to OSC1 PARAM (PWM) via mod matrix slot 0
    widgets.push_back(Widget("MOD ATK",  AMB_MOD_ATTACK,   8, 78, WidgetType::CONTINUOUS, 0.0f));
    widgets.push_back(Widget("MOD DEC",  AMB_MOD_DECAY,    9, 78, WidgetType::CONTINUOUS, 0.6f));
    widgets.push_back(Widget("MOD SUS",  AMB_MOD_SUSTAIN, 10, 78, WidgetType::CONTINUOUS, 0.7f));
    widgets.push_back(Widget("MOD REL",  AMB_MOD_RELEASE, 11, 78, WidgetType::CONTINUOUS, 0.85f));
}
