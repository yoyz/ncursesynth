#include "ambika_ui.h"

// Ambika parameter IDs (match ambika_types.h)
#define AMB_OSC1_SHAPE      0
#define AMB_OSC1_PARAM      1
#define AMB_OSC1_RANGE      2
#define AMB_OSC1_DETUNE     3
#define AMB_OSC2_SHAPE      4
#define AMB_OSC2_PARAM      5
#define AMB_OSC2_RANGE      6
#define AMB_OSC2_DETUNE     7
#define AMB_FILTER_CUTOFF   8
#define AMB_FILTER_RES      9
#define AMB_FILTER_MODE     10
#define AMB_FILTER_TYPE     11
#define AMB_MIX_BALANCE     12
#define AMB_MIX_OP          13
#define AMB_MIX_SUB         14
#define AMB_MIX_NOISE       15
#define AMB_MIX_FUZZ        16
#define AMB_ENV1_ATTACK     17
#define AMB_ENV1_DECAY      18
#define AMB_ENV1_SUSTAIN    19
#define AMB_ENV1_RELEASE    20
#define AMB_ENV2_ATTACK     21
#define AMB_ENV2_DECAY      22
#define AMB_ENV2_SUSTAIN    23
#define AMB_ENV2_RELEASE    24
#define AMB_ENV3_ATTACK     25
#define AMB_ENV3_DECAY      26
#define AMB_ENV3_SUSTAIN    27
#define AMB_ENV3_RELEASE    28
#define AMB_LFO_SHAPE       29
#define AMB_LFO_RATE        30
#define AMB_VOLUME          31
#define AMB_PORTAMENTO      32
#define AMB_FENV_AMOUNT     80
#define AMB_FLFO_AMOUNT     81

void AmbikaUI::initControls()
{
    widgets.clear();

    // Column 1 - OSCILLATORS (x=2)
    widgets.push_back(Widget("OSC1 SHAPE",  AMB_OSC1_SHAPE,  0, 2, WidgetType::DISCRETE,   0.0f, 35));
    widgets.push_back(Widget("OSC1 PARAM",  AMB_OSC1_PARAM,  1, 2, WidgetType::CONTINUOUS, 0.25f));
    widgets.push_back(Widget("OSC1 RANGE",  AMB_OSC1_RANGE,  2, 2, WidgetType::BIPOLAR,    0.5f));
    widgets.push_back(Widget("OSC1 DETUNE", AMB_OSC1_DETUNE, 3, 2, WidgetType::BIPOLAR,    0.5f));
    widgets.push_back(Widget("OSC2 SHAPE",  AMB_OSC2_SHAPE,  4, 2, WidgetType::DISCRETE,   0.03f, 35));
    widgets.push_back(Widget("OSC2 PARAM",  AMB_OSC2_PARAM,  5, 2, WidgetType::CONTINUOUS, 0.0f));
    widgets.push_back(Widget("OSC2 RANGE",  AMB_OSC2_RANGE,  6, 2, WidgetType::BIPOLAR,    0.5f));
    widgets.push_back(Widget("OSC2 DETUNE", AMB_OSC2_DETUNE, 7, 2, WidgetType::BIPOLAR,    0.5f));
    widgets.push_back(Widget("VOLUME",      AMB_VOLUME,      8, 2, WidgetType::CONTINUOUS, 0.8f));

    // Column 2 - FILTER / MIX (x=40)
    widgets.push_back(Widget("CUTOFF",   AMB_FILTER_CUTOFF, 0, 40, WidgetType::CONTINUOUS, 0.75f));
    widgets.push_back(Widget("RESONANCE",AMB_FILTER_RES,    1, 40, WidgetType::CONTINUOUS, 0.0f));
    widgets.push_back(Widget("F MODE",   AMB_FILTER_MODE,   2, 40, WidgetType::DISCRETE,   0.0f, 4));
    widgets.push_back(Widget("F TYPE",   AMB_FILTER_TYPE,   3, 40, WidgetType::DISCRETE,   0.0f, 2));
    widgets.push_back(Widget("BALANCE",  AMB_MIX_BALANCE,   4, 40, WidgetType::CONTINUOUS, 0.5f));
    widgets.push_back(Widget("MIX OP",   AMB_MIX_OP,        5, 40, WidgetType::DISCRETE,   0.0f, 6));
    widgets.push_back(Widget("SUB OSC",  AMB_MIX_SUB,       6, 40, WidgetType::CONTINUOUS, 0.3f));
    widgets.push_back(Widget("NOISE",    AMB_MIX_NOISE,     7, 40, WidgetType::CONTINUOUS, 0.06f));
    widgets.push_back(Widget("FUZZ",     AMB_MIX_FUZZ,      8, 40, WidgetType::CONTINUOUS, 0.0f));
    widgets.push_back(Widget("FENV AMT", AMB_FENV_AMOUNT,    9, 40, WidgetType::BIPOLAR,    0.5f));
    widgets.push_back(Widget("FLFO AMT", AMB_FLFO_AMOUNT,   10, 40, WidgetType::BIPOLAR,    0.5f));

    // Column 3 - ENVELOPE / LFO (x=78)
    // Filter Envelope (Env1) - modulates filter cutoff via mod matrix slot 1
    widgets.push_back(Widget("FLT ATK",  AMB_ENV1_ATTACK,   0, 78, WidgetType::CONTINUOUS, 0.0f));
    widgets.push_back(Widget("FLT DEC",  AMB_ENV1_DECAY,    1, 78, WidgetType::CONTINUOUS, 0.6f));
    widgets.push_back(Widget("FLT SUS",  AMB_ENV1_SUSTAIN,  2, 78, WidgetType::CONTINUOUS, 0.7f));
    widgets.push_back(Widget("FLT REL",  AMB_ENV1_RELEASE,  3, 78, WidgetType::CONTINUOUS, 0.85f));

    // Amp Envelope (Env2) - controls VCA via mod matrix slot 5
    widgets.push_back(Widget("AMP ATK",  AMB_ENV2_ATTACK,   4, 78, WidgetType::CONTINUOUS, 0.0f));
    widgets.push_back(Widget("AMP DEC",  AMB_ENV2_DECAY,    5, 78, WidgetType::CONTINUOUS, 0.6f));
    widgets.push_back(Widget("AMP SUS",  AMB_ENV2_SUSTAIN,  6, 78, WidgetType::CONTINUOUS, 0.7f));
    widgets.push_back(Widget("AMP REL",  AMB_ENV2_RELEASE,  7, 78, WidgetType::CONTINUOUS, 0.85f));

    // LFO
    widgets.push_back(Widget("LFO SHAPE", AMB_LFO_SHAPE,    9, 78, WidgetType::DISCRETE,   0.0f, 4));
    widgets.push_back(Widget("LFO RATE",  AMB_LFO_RATE,    10, 78, WidgetType::CONTINUOUS, 0.12f));
    widgets.push_back(Widget("PORTAM",    AMB_PORTAMENTO,  11, 78, WidgetType::CONTINUOUS, 0.0f));
}
