#ifndef DIGITS_TYPES_H
#define DIGITS_TYPES_H

#include <cstdint>
#define Sint32 int32_t
#define Sint16 int16_t
#define DPRINTF(...)
#define DEFAULTFREQ 48000

static constexpr int DIGITS_MAX_VOICES = 8;
static constexpr int DIGITS_SAM = 64;
static constexpr int OUTPUT_AMPLITUDE = 8192;

#define OSC1_SHAPER      160
#define OSC1_SKEW        161
#define OSC1_BASIS       162
#define OSC2_SHAPER      163
#define OSC2_SKEW        164
#define OSC2_BASIS       165
#define RESO_VOL         166
#define LFO1_SHAPE       167
#define LFO_AMT_SHAPER   168

#define NOTE_ON          150
#define NOTE1            70
#define VELOCITY         140

#define OSC1_TYPE        20
#define OSC2_TYPE        21
#define OSC12_MIX        28
#define OSC1_DETUNE      74
#define OSC2_DETUNE      75
#define OSC1_SCALE       65
#define OSC2_SCALE       66
#define OSC1_AMP         31
#define OSC2_AMP         32

#define FILTER1_TYPE     50
#define FILTER1_CUTOFF   52
#define FILTER1_RESONANCE 53

#define ADSR_ENV0_ATTACK   0
#define ADSR_ENV0_DECAY    1
#define ADSR_ENV0_SUSTAIN  2
#define ADSR_ENV0_RELEASE  3

#define ADSR_ENV1_ATTACK   4
#define ADSR_ENV1_DECAY    5
#define ADSR_ENV1_SUSTAIN  6
#define ADSR_ENV1_RELEASE  7

#define ENV1_DEPTH        94

#define LFO1_FREQ         42
#define LFO1_DEPTH        41
#define LFO2_FREQ         44
#define LFO2_DEPTH        43

#define AMP               35

#define POLYPHONY         110

#endif
