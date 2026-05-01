#ifndef PARAMETER_H
#define PARAMETER_H

namespace MachineParam {
    enum Section {
        OSC = 0,
        LFO = 20,
        FILTER = 50,
        AMP_ENV = 80,
        FILTER_ENV = 90,
        NOTE = 70,
        EFFECTS = 100,
        PERFORMANCE = 110,
        MODULATION = 130,
        MOD_MATRIX = 130,
        COUNT = 150
    };

    enum Osc {
        OSC_1_WAVEFORM = OSC,
        OSC_1_VOLUME,
        OSC_1_PHASE,
        OSC_1_PHASE_SYNC,
        OSC_2_WAVEFORM,
        OSC_2_VOLUME,
        OSC_2_PHASE,
        OSC_2_PHASE_SYNC,
        OSC_CROSS_MOD,
        OSC_MIX,
        OSC_2_TRANSPOSE,
        OSC_2_TUNE,
        OSC_COUNT
    };

    enum Lfo {
        LFO_1_WAVEFORM = LFO,
        LFO_1_FREQUENCY,
        LFO_1_AMOUNT,
        LFO_2_WAVEFORM,
        LFO_2_FREQUENCY,
        LFO_2_AMOUNT,
        LFO_COUNT
    };

    enum Filter {
        FILTER_TYPE = FILTER,
        FILTER_CUTOFF,
        FILTER_RESONANCE,
        FILTER_KEYTRACK,
        FILTER_ENV_DEPTH,
        FILTER_ATTACK,
        FILTER_DECAY,
        FILTER_SUSTAIN,
        FILTER_RELEASE,
        FILTER_COUNT
    };

    enum AmpEnv {
        AMP_ATTACK = AMP_ENV,
        AMP_DECAY,
        AMP_SUSTAIN,
        AMP_RELEASE,
        VELOCITY_TRACK,
        AMP_COUNT
    };

    enum FilterEnv {
        FILTER_ENV_ATTACK = FILTER_ENV,
        FILTER_ENV_DECAY,
        FILTER_ENV_SUSTAIN,
        FILTER_ENV_RELEASE,
        FILTER_ENV_COUNT
    };

    enum Note {
        NOTE_ON = NOTE,
        NOTE_HZ,
        NOTE_TRACK,
        NOTE_COUNT
    };

    enum Effects {
        DELAY_TIME = EFFECTS,
        DELAY_FEEDBACK,
        DELAY_DRY_WET,
        REVERB_AMOUNT,
        REVERB_DRY_WET,
        CHORUS_RATE,
        CHORUS_DEPTH,
        CHORUS_DRY_WET,
        DISTORTION_DRIVE,
        DISTORTION_DRY_WET,
        EFFECTS_COUNT
    };

    enum Performance {
        POLYPHONY = PERFORMANCE,
        LEGATO,
        PORTAMENTO_TIME,
        PORTAMENTO_TYPE,
        PITCH_BEND_RANGE,
        UNISON_VOICES,
        UNISON_SPREAD,
        PERFORMANCE_COUNT
    };

    enum ModMatrix {
        MOD_SOURCE_1 = MOD_MATRIX,
        MOD_SCALE_1,
        MOD_DESTINATION_1,
        MOD_SOURCE_2,
        MOD_SCALE_2,
        MOD_DESTINATION_2,
        MOD_SOURCE_3,
        MOD_SCALE_3,
        MOD_DESTINATION_3,
        MOD_COUNT
    };

    const int RESONANCE = FILTER_RESONANCE;
    const int FILTER_AMP_ATTACK = AMP_ATTACK;
    const int FILTER_AMP_DECAY = AMP_DECAY;
    const int FILTER_AMP_SUSTAIN = AMP_SUSTAIN;
    const int FILTER_AMP_RELEASE = AMP_RELEASE;
}

#endif