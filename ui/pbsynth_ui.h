#ifndef PBSYNTH_UI_H
#define PBSYNTH_UI_H

#include "machine_ui.h"

namespace PBSynthParam {
    enum Osc1 {
        P_OSC1_WAVEFORM = 20,
        P_OSC1_DETUNE = 74,
        P_OSC1_SCALE = 65,
        P_OSC1_AMP = 31
    };
    enum Osc2 {
        P_OSC2_WAVEFORM = 21,
        P_OSC2_DETUNE = 75,
        P_OSC2_SCALE = 66,
        P_OSC2_AMP = 32
    };
    enum AmpEnv {
        P_AMP_ATTACK = 0,
        P_AMP_DECAY = 1,
        P_AMP_SUSTAIN = 2,
        P_AMP_RELEASE = 3
    };
    enum FilterEnv {
        P_FILTER_ENV_ATTACK = 4,
        P_FILTER_ENV_DECAY = 5,
        P_FILTER_ENV_SUSTAIN = 6,
        P_FILTER_ENV_RELEASE = 7,
        P_FILTER_ENV_DEPTH = 94
    };
    enum Filter {
        P_FILTER_CUTOFF = 52,
        P_FILTER_RESONANCE = 53
    };
    enum Lfo {
        P_LFO1_WAVEFORM = 2,
        P_LFO1_FREQ = 42,
        P_LFO1_DEPTH = 41,
        P_LFO2_WAVEFORM = 3,
        P_LFO2_FREQ = 44,
        P_LFO2_DEPTH = 43
    };
    enum Mix {
        P_OSC_MIX = 28,
        P_MASTER_VOL = 35
    };
}

class PBSynthUI : public MachineUI {
private:
    void initControls() override;
    void drawColumnHeader(int col, const char* title) override;
    void drawControl(int index, bool selected) override;
    void drawSlider(int row, int col, const char* name, float value, bool selected) override;
    void updateControlValues() override;

public:
    PBSynthUI(Machine* mach, MachineManager* mgr = nullptr);
    ~PBSynthUI();

    void init() override;
    void draw() override;
    void handleInput(int ch) override;
    void updateValues() override;

    using MachineUI::setMidiNote;
    using MachineUI::setControlValue;
};

#endif