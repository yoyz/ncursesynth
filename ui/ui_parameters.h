#ifndef UI_PARAMETERS_H
#define UI_PARAMETERS_H

#include "../synth/synth_architecture.h"

// Parameter enum definition
enum class Parameter {
    // Main synth parameters
    POLYPHONY,
    FILTER_TYPE,
    CUTOFF,
    RESONANCE,
    FILTER_ENV_AMOUNT,
    
    // Amplitude Envelope
    AMP_ATTACK,
    AMP_DECAY,
    AMP_SUSTAIN,
    AMP_RELEASE,
    AMP_ENV_CURVE,
    
    // Filter Envelope
    FILTER_ATTACK,
    FILTER_DECAY,
    FILTER_SUSTAIN,
    FILTER_RELEASE,
    FILTER_ENV_CURVE,
    
    // Oscillator
    WAVEFORM,
    
    // MIDI
    MIDI_DEVICE,
    MIDI_ENABLE,
    
    // Effects
    DELAY_ENABLE,
    DELAY_TIME,
    DELAY_FEEDBACK,
    DELAY_MIX,
    REVERB_ENABLE,
    REVERB_DECAY,
    REVERB_MIX,
    CHORUS_ENABLE,
    CHORUS_DEPTH,
    CHORUS_RATE,
    CHORUS_MIX,
    DISTORTION_ENABLE,
    DISTORTION_DRIVE,
    DISTORTION_MIX,
    
    VOLUME,
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
}

#endif
