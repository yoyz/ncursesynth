#ifndef NCURSESYNTH_UI_H
#define NCURSESYNTH_UI_H

#include "machine_ui.h"
#include "../machine/NcursesynthMachine.h"

namespace NcursesynthParam {
    enum Osc {
        P_OSC_1_WAVEFORM = 0,
        P_OSC_1_VOLUME = 1,
        P_OSC_1_PHASE = 2,
        P_OSC_1_PHASE_SYNC = 3,
        P_OSC_2_WAVEFORM = 4,
        P_OSC_2_VOLUME = 5,
        P_OSC_2_PHASE = 6,
        P_OSC_2_PHASE_SYNC = 7,
        P_OSC_CROSS_MOD = 8,
        P_OSC_MIX = 9,
        P_OSC_2_TRANSPOSE = 10,
        P_OSC_2_TUNE = 11
    };
    enum Filter {
        P_FILTER_TYPE = 50,
        P_FILTER_CUTOFF = 51,
        P_FILTER_RESONANCE = 52,
        P_FILTER_KEYTRACK = 53,
        P_FILTER_ENV_DEPTH = 54,
        P_FILTER_ATTACK = 55,
        P_FILTER_DECAY = 56,
        P_FILTER_SUSTAIN = 57,
        P_FILTER_RELEASE = 58
    };
    enum AmpEnv {
        P_AMP_ATTACK = 80,
        P_AMP_DECAY = 81,
        P_AMP_SUSTAIN = 82,
        P_AMP_RELEASE = 83
    };
    enum Lfo {
        P_LFO_1_WAVEFORM = 20,
        P_LFO_1_FREQ = 21,
        P_LFO_1_AMOUNT = 22,
        P_LFO_2_WAVEFORM = 23,
        P_LFO_2_FREQ = 24,
        P_LFO_2_AMOUNT = 25
    };
    enum Effects {
        P_DELAY_TIME = 100,
        P_DELAY_FEEDBACK = 101,
        P_DELAY_DRY_WET = 102,
        P_REVERB_AMOUNT = 103,
        P_REVERB_DRY_WET = 104
    };
    enum Performance {
        P_POLYPHONY = 110,
        P_LEGATO = 111,
        P_PORTAMENTO_TIME = 112
    };
}

class NcursesynthUI : public MachineUI {
private:
    void initControls() override;
    void drawColumnHeader(int col, const char* title) override;
    void drawControl(int index, bool selected) override;
    void drawSlider(int row, int col, const char* name, float value, bool selected) override;
    void updateControlValues() override;

public:
    NcursesynthUI(Machine* mach, MachineManager* mgr = nullptr);
    ~NcursesynthUI();

    void init() override;
    void draw() override;
    void handleInput(int ch) override;
    void updateValues() override;

    using MachineUI::setMidiNote;
    using MachineUI::setControlValue;
};

#endif