#ifndef UI_PARAMETERS_H
#define UI_PARAMETERS_H

#include "../synth/synth_architecture.h"

enum class Parameter {
    // LEFT COLUMN - Oscillator (row 6)
    WAVEFORM,
    OSC_MIX,
    OSC2_DETUNE,

    // LEFT COLUMN - LFO (row 14)
    LFO_WAVEFORM,
    LFO_FREQUENCY,
    LFO_DEPTH,

    // LEFT COLUMN - Volume (row 26)
    VOLUME,

    // LEFT COLUMN - Modulation Matrix (row 32)
    MOD_SOURCE_1,
    MOD_SCALE_1,
    MOD_DESTINATION_1,
    MOD_SOURCE_2,
    MOD_SCALE_2,
    MOD_DESTINATION_2,

    // CENTER COLUMN - Filter (row 6)
    FILTER_TYPE,
    CUTOFF,
    RESONANCE,
    FILTER_ENV_AMOUNT,
    HPF_FREQ,

    // CENTER COLUMN - Filter Envelope (row 20)
    FILTER_ATTACK,
    FILTER_DECAY,
    FILTER_SUSTAIN,
    FILTER_RELEASE,
    FILTER_ENV_CURVE,

    // RIGHT COLUMN - Performance (row 6)
    POLYPHONY,
    LEGATO,
    PORTAMENTO,

    // RIGHT COLUMN - Amp Envelope (row 14)
    AMP_ATTACK,
    AMP_DECAY,
    AMP_SUSTAIN,
    AMP_RELEASE,
    AMP_ENV_CURVE,

    // RIGHT COLUMN - Delay (row 14)
    DELAY_ENABLE,
    DELAY_TIME,
    DELAY_FEEDBACK,
    DELAY_MIX,

    // RIGHT COLUMN - Reverb (row 20)
    REVERB_ENABLE,
    REVERB_DECAY,
    REVERB_MIX,

    // RIGHT COLUMN - Chorus (row 26)
    CHORUS_ENABLE,
    CHORUS_DEPTH,
    CHORUS_RATE,
    CHORUS_MIX,

    // RIGHT COLUMN - Distortion (row 29)
    DISTORTION_ENABLE,
    DISTORTION_DRIVE,
    DISTORTION_MIX,

    // RIGHT COLUMN - Master (row 29)
    MASTER_VOLUME,

    // Preset (special - can be anywhere)
    PRESET_LOAD,
    PRESET_SAVE,

    COUNT
};

namespace UIParameters {
    const char* getParameterName(Parameter param);
    const char* getFilterTypeName(SynthArchitecture* synth);
    const char* getWaveformName(SynthArchitecture* synth);
    const char* getBoolText(bool value);
    const char* getCurveName(EnvelopeCurve curve);
    
    // Parameter adjustment functions
    void increaseParameter(SynthArchitecture* synth, Parameter param);
    void decreaseParameter(SynthArchitecture* synth, Parameter param);
    
    // Preset navigation
    void loadNextPreset(SynthArchitecture* synth);
    void loadPrevPreset(SynthArchitecture* synth);
}

#endif
