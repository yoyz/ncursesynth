#ifndef UI_PARAMETERS_H
#define UI_PARAMETERS_H

#include "../synth/synth_architecture.h"

// Parameter enum definition - ordered to match UI layout
// LEFT COLUMN: Filter Section (top), then Amp Envelope, then Filter Envelope
// RIGHT COLUMN: Oscillator, Delay, Reverb, Chorus, Distortion, Master, MIDI
enum class Parameter {
    // LEFT COLUMN - Filter Section (7 params)
    POLYPHONY,
    FILTER_TYPE,
    CUTOFF,
    RESONANCE,
    FILTER_ENV_AMOUNT,
    HPF_FREQ,
    
    // LEFT COLUMN - Amplitude Envelope (5 params)
    AMP_ATTACK,
    AMP_DECAY,
    AMP_SUSTAIN,
    AMP_RELEASE,
    AMP_ENV_CURVE,
    
    // LEFT COLUMN - Filter Envelope (5 params)
    FILTER_ATTACK,
    FILTER_DECAY,
    FILTER_SUSTAIN,
    FILTER_RELEASE,
    FILTER_ENV_CURVE,
    
    // RIGHT COLUMN - Oscillator (3 params)
    WAVEFORM,
    OSC_MIX,
    OSC2_DETUNE,
    
    // RIGHT COLUMN - Preset (2 params)
    PRESET_LOAD,
    PRESET_SAVE,
    
    // RIGHT COLUMN - Delay (4 params)
    DELAY_ENABLE,
    DELAY_TIME,
    DELAY_FEEDBACK,
    DELAY_MIX,
    
    // RIGHT COLUMN - Reverb (3 params)
    REVERB_ENABLE,
    REVERB_DECAY,
    REVERB_MIX,
    
    // RIGHT COLUMN - Chorus (4 params)
    CHORUS_ENABLE,
    CHORUS_DEPTH,
    CHORUS_RATE,
    CHORUS_MIX,
    
    // RIGHT COLUMN - Distortion (3 params)
    DISTORTION_ENABLE,
    DISTORTION_DRIVE,
    DISTORTION_MIX,
    
    // RIGHT COLUMN - Master (1 param)
    VOLUME,
    
    // RIGHT COLUMN - MIDI (3 params)
    MIDI_ENABLE,
    MIDI_DEVICE,
    MIDI_MAPPING,
    
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
