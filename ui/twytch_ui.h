#ifndef TWYTCH_UI_H
#define TWYTCH_UI_H

#include "machine_ui.h"
#include "../machine/Twytch/twytch_types.h"

namespace TwytchParam {
    enum Osc1 {
        P_OSC1_WAVEFORM = 20,
        P_OSC1_DETUNE = 74,
        P_OSC1_SCALE = 65,
        P_OSC1_MOD = 29,
        P_OSC1_UNISON = 76,
        P_OSC1_UNISONDT = 78,
        P_OSC1_AMP = 31
    };
    enum Osc2 {
        P_OSC2_WAVEFORM = 21,
        P_OSC2_DETUNE = 75,
        P_OSC2_SCALE = 66,
        P_OSC2_MOD = 29,
        P_OSC2_UNISON = 77,
        P_OSC2_UNISONDT = 79,
        P_OSC2_AMP = 32
    };
    enum Osc3 {
        P_OSC3_WAVEFORM = 22,
        P_OSC3_AMP = 33
    };
    enum Osc4 {
        P_OSC4_WAVEFORM = 23,
        P_OSC4_AMP = 34
    };
    enum Filter {
        P_FILTER_CUTOFF = 52,
        P_FILTER_RESONANCE = 53,
        P_FILTER_SATURATION = 54,
        P_FILTER_FEEDBACK = 56
    };
    enum FilterEnv {
        P_FILTER_ENV_ATTACK = 4,
        P_FILTER_ENV_DECAY = 5,
        P_FILTER_ENV_SUSTAIN = 6,
        P_FILTER_ENV_RELEASE = 7,
        P_FILTER_ENV_DEPTH = 94
    };
    enum AmpEnv {
        P_AMP_ATTACK = 0,
        P_AMP_DECAY = 1,
        P_AMP_SUSTAIN = 2,
        P_AMP_RELEASE = 3
    };
    enum Lfo1 {
        P_LFO1_WAVEFORM = 2,
        P_LFO1_FREQ = 42,
        P_LFO1_DEPTH = 41
    };
    enum Lfo2 {
        P_LFO2_WAVEFORM = 3,
        P_LFO2_FREQ = 44,
        P_LFO2_DEPTH = 43
    };
    enum Performance {
        P_KEYTRACK = 55,
        P_VELOCITY = 140,
        P_OSC_MIX = 28,
        P_MASTER_VOL = 35
    };
}

class TwytchUI : public MachineUI {
private:
    void initControls() override;
    void drawColumnHeader(int col, const char* title) override;
    void drawControl(int index, bool selected) override;
    void drawSlider(int row, int col, const char* name, float value, bool selected) override;
    void updateControlValues() override;

public:
    TwytchUI(Machine* mach, MachineManager* mgr = nullptr);
    ~TwytchUI();

    void init() override;
    void draw() override;
    void handleInput(int ch) override;
    void updateValues() override;

    using MachineUI::setMidiNote;
    using MachineUI::setControlValue;
};

#endif