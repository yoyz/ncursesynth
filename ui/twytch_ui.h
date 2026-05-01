#ifndef PBSYNTH_UI_H
#define PBSYNTH_UI_H

#include <string>
#include <vector>
#include <ncurses.h>
#include "../machine/Machine.h"

namespace PBSynthParam {
    enum Osc1 {
        P_OSC1_WAVEFORM = 0,
        P_OSC1_DETUNE = 74,
        P_OSC1_SCALE = 65,
        P_OSC1_AMP = 31
    };
    enum Osc2 {
        P_OSC2_WAVEFORM = 1,
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

struct PBSynthControl {
    std::string name;
    int param;
    int row;
    int col;
    float value;
    float minVal;
    float maxVal;
};

class PBSynthUI {
private:
    int selectedControl;
    std::vector<PBSynthControl> controls;
    Machine* machine;
    int screenRows, screenCols;

    std::string midiMonitorText;
    int lastMidiNote;
    int lastMidiVel;
    bool midiActivity;

    void initControls();
    void drawColumnHeader(int col, const char* title);
    void drawControl(int index, bool selected);
    void drawSlider(int row, int col, const char* name, float value, bool selected);
    void drawMidiMonitor();

public:
    PBSynthUI(Machine* mach);
    ~PBSynthUI();

    void init();
    void draw();
    void handleInput(int ch);
    void updateValues();
    void setMidiNote(int note, int vel) { lastMidiNote = note; lastMidiVel = vel; midiActivity = true; }
    bool isActive() const { return true; }

    void setControlValue(int paramId, float value);
};

#endif