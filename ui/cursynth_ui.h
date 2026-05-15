#ifndef CURSYNTH_UI_H
#define CURSYNTH_UI_H

#include "machine_ui.h"
#include "../machine/Cursynth/CursynthMachine.h"

namespace CursynthParam {
    enum Osc {
        P_OSC1_WAVEFORM = 0,
        P_OSC1_DETUNE = 1,
        P_OSC1_SCALE = 2,
        P_OSC1_AMP = 3,
        P_OSC2_WAVEFORM = 4,
        P_OSC2_DETUNE = 5,
        P_OSC2_SCALE = 6,
        P_OSC2_AMP = 7,
        P_OSC_MIX = 8
    };
    enum Filter {
        P_FILTER_CUTOFF = 50,
        P_FILTER_RESONANCE = 51,
        P_FILTER_ENV_DEPTH = 52
    };
    enum AmpEnv {
        P_AMP_ATTACK = 80,
        P_AMP_DECAY = 81,
        P_AMP_SUSTAIN = 82,
        P_AMP_RELEASE = 83
    };
    enum FilterEnv {
        P_FILTER_ATTACK = 90,
        P_FILTER_DECAY = 91,
        P_FILTER_SUSTAIN = 92,
        P_FILTER_RELEASE = 93
    };
    enum Lfo {
        P_LFO1_WAVEFORM = 20,
        P_LFO1_FREQ = 21,
        P_LFO1_DEPTH = 22,
        P_LFO2_WAVEFORM = 23,
        P_LFO2_FREQ = 24,
        P_LFO2_DEPTH = 25
    };
    enum Performance {
        P_POLYPHONY = 110,
        P_LEGATO = 111,
        P_PORTAMENTO_TIME = 112
    };
    enum Effects {
        P_DELAY_TIME = 100,
        P_DELAY_FEEDBACK = 101,
        P_DELAY_MIX = 102
    };
}

class CursynthUI : public MachineUI {
private:
    void initControls() override;

public:
    CursynthUI(Machine* mach, MachineManager* mgr = nullptr);
    ~CursynthUI();

    using MachineUI::setMidiNote;
    using MachineUI::setControlValue;
};

#endif
