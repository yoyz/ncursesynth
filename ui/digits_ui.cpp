#include "digits_ui.h"
#include "../machine/Digits/digits_types.h"

void DigitsUI::initControls()
{
    widgets.clear();

    // Column 1 - OSCILLATORS (x=2)
    widgets.push_back(Widget("OSC1 SHAPER", OSC1_SHAPER, 0, 2, WidgetType::CONTINUOUS, 0.5f));
    widgets.push_back(Widget("OSC1 SKEW", OSC1_SKEW, 1, 2, WidgetType::CONTINUOUS, 0.5f));
    widgets.push_back(Widget("OSC1 BASIS", OSC1_BASIS, 2, 2, WidgetType::DISCRETE, 0.0f, 2));
    widgets.push_back(Widget("OSC2 SHAPER", OSC2_SHAPER, 3, 2, WidgetType::CONTINUOUS, 0.5f));
    widgets.push_back(Widget("OSC2 SKEW", OSC2_SKEW, 4, 2, WidgetType::CONTINUOUS, 0.5f));
    widgets.push_back(Widget("OSC2 BASIS", OSC2_BASIS, 5, 2, WidgetType::DISCRETE, 0.0f, 2));
    widgets.push_back(Widget("OSC MIX", OSC12_MIX, 6, 2, WidgetType::CONTINUOUS, 0.35f));
    widgets.push_back(Widget("DETUNE", OSC1_DETUNE, 7, 2, WidgetType::BIPOLAR, 0.5f));
    widgets.push_back(Widget("VOLUME", AMP, 8, 2, WidgetType::CONTINUOUS, 0.8f));
    widgets.push_back(Widget("RESO VOL", RESO_VOL, 9, 2, WidgetType::CONTINUOUS, 0.0f));

    // Column 2 - FILTER (x=40)
    widgets.push_back(Widget("CUTOFF", FILTER1_CUTOFF, 0, 40, WidgetType::CONTINUOUS, 0.5f));
    widgets.push_back(Widget("RESONANCE", FILTER1_RESONANCE, 1, 40, WidgetType::CONTINUOUS, 0.0f));
    widgets.push_back(Widget("FENV DEPTH", ENV1_DEPTH, 2, 40, WidgetType::BIPOLAR, 0.5f));

    // Filter Envelope
    widgets.push_back(Widget("FLT ATTACK", ADSR_ENV1_ATTACK, 4, 40, WidgetType::CONTINUOUS, 0.0f));
    widgets.push_back(Widget("FLT DECAY", ADSR_ENV1_DECAY, 5, 40, WidgetType::CONTINUOUS, 0.5f));
    widgets.push_back(Widget("FLT SUSTAIN", ADSR_ENV1_SUSTAIN, 6, 40, WidgetType::CONTINUOUS, 1.0f));
    widgets.push_back(Widget("FLT RELEASE", ADSR_ENV1_RELEASE, 7, 40, WidgetType::CONTINUOUS, 0.5f));

    // Column 3 - ENVELOPE / LFO (x=78)
    // Amp Envelope
    widgets.push_back(Widget("AMP ATTACK", ADSR_ENV0_ATTACK, 0, 78, WidgetType::CONTINUOUS, 0.0f));
    widgets.push_back(Widget("AMP DECAY", ADSR_ENV0_DECAY, 1, 78, WidgetType::CONTINUOUS, 0.5f));
    widgets.push_back(Widget("AMP SUSTAIN", ADSR_ENV0_SUSTAIN, 2, 78, WidgetType::CONTINUOUS, 1.0f));
    widgets.push_back(Widget("AMP RELEASE", ADSR_ENV0_RELEASE, 3, 78, WidgetType::CONTINUOUS, 0.5f));

    // LFO
    widgets.push_back(Widget("LFO RATE", LFO1_FREQ, 5, 78, WidgetType::CONTINUOUS, 0.25f));
    widgets.push_back(Widget("LFO DEPTH", LFO1_DEPTH, 6, 78, WidgetType::CONTINUOUS, 0.0f));
    widgets.push_back(Widget("LFO SHAPE", LFO1_SHAPE, 7, 78, WidgetType::DISCRETE, 0.0f, 4));
}
