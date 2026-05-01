#ifndef TWYTCH_TYPES_H
#define TWYTCH_TYPES_H

#include <cstdint>

#define Sint32 int32_t
#define Sint16 int16_t
#define Uint32 uint32_t
#define Uint16 uint16_t

#define DPRINTF(...)

#define DEFAULTFREQ 48000

#define OSC1_TYPE 20
#define OSC2_TYPE 21
#define OSC1_SCALE 65
#define OSC2_SCALE 66
#define OSC1_DETUNE 74
#define OSC2_DETUNE 75

#define FILTER1_TYPE 50
#define FILTER1_CUTOFF 52
#define FILTER1_RESONANCE 53

#define LFO1_WAVEFORM 2
#define LFO2_WAVEFORM 3

#define LFO1_FREQ 42
#define LFO2_FREQ 44

#define LFO_TYPE 40
#define LFO1_ENV_AMOUNT 41
#define LFO2_ENV_AMOUNT 43
#define LFO1_DEPTH 41
#define LFO2_DEPTH 43

#define NOTE_ON 150
#define NOTE1 70

#define VELOCITY 140
#define OSC12_MIX 28
#define OSC1_MOD 29
#define KEYTRACK 55
#define ENV1_DEPTH 94
#define FX1_DEPTH 100
#define FX1_SPEED 101
#define AMP 35

#define ADSR_ENV0_ATTACK 0
#define ADSR_ENV0_DECAY 1
#define ADSR_ENV0_SUSTAIN 2
#define ADSR_ENV0_RELEASE 3

#define ADSR_ENV1_ATTACK 4
#define ADSR_ENV1_DECAY 5
#define ADSR_ENV1_SUSTAIN 6
#define ADSR_ENV1_RELEASE 7

#ifndef TRIG_TIME_MODE
#define TRIG_TIME_MODE 160
#endif

#ifndef TRIG_TIME_DURATION
#define TRIG_TIME_DURATION 161
#endif

#define OSC3_TYPE 22
#define OSC4_TYPE 23
#define OSC3_AMP 33
#define OSC4_AMP 34
#define OSC1_UNISON 76
#define OSC2_UNISON 77
#define OSC1_UNISONDT 78
#define OSC2_UNISONDT 79
#define FILTER1_SATURATION 54
#define FILTER1_FEEDBACK 56

#endif