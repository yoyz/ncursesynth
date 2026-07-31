#ifndef AMBIKA_TYPES_H
#define AMBIKA_TYPES_H

#include <cstdint>

#define Sint32 int32_t
#define Sint16 int16_t
#define DPRINTF(...)

static constexpr int AMBIKA_MAX_VOICES = 6;
static constexpr int AMBIKA_BLOCK_SIZE = 40;
static constexpr int OUTPUT_AMPLITUDE = 8192;

// Ambika parameter IDs (mirroring AmbikaParam enum from plugin_shared.h)
#define AMB_OSC1_SHAPE      0
#define AMB_OSC1_PARAM      1
#define AMB_OSC1_RANGE      2
#define AMB_OSC1_DETUNE     3
#define AMB_OSC2_SHAPE      4
#define AMB_OSC2_PARAM      5
#define AMB_OSC2_RANGE      6
#define AMB_OSC2_DETUNE     7

#define AMB_FILTER_CUTOFF   8
#define AMB_FILTER_RES      9
#define AMB_FILTER_MODE     10
#define AMB_FILTER_TYPE     11

#define AMB_MIX_BALANCE     12
#define AMB_MIX_OP          13
#define AMB_MIX_SUB         14
#define AMB_MIX_NOISE       15
#define AMB_MIX_FUZZ        16

#define AMB_ENV1_ATTACK     17
#define AMB_ENV1_DECAY      18
#define AMB_ENV1_SUSTAIN    19
#define AMB_ENV1_RELEASE    20
#define AMB_ENV2_ATTACK     21
#define AMB_ENV2_DECAY      22
#define AMB_ENV2_SUSTAIN    23
#define AMB_ENV2_RELEASE    24
#define AMB_ENV3_ATTACK     25
#define AMB_ENV3_DECAY      26
#define AMB_ENV3_SUSTAIN    27
#define AMB_ENV3_RELEASE    28

#define AMB_LFO_SHAPE       29
#define AMB_LFO_RATE        30
#define AMB_VOLUME          31
#define AMB_PORTAMENTO      32

#define AMB_FENV_AMOUNT     80
#define AMB_FLFO_AMOUNT     81

// Ambika extended param IDs (unified scheme — in a range that avoids
// collisions with the shared ParamID namespace in machine/ParamID.h).
// These identify Ambika-specific controls that have no unified equivalent.
#define AMB_OSC1_PARAM      100
#define AMB_OSC2_PARAM      101
#define AMB_FILTER_MODE     102
#define AMB_MIX_OP          103
#define AMB_MIX_SUB         104
#define AMB_MIX_NOISE       105
#define AMB_MIX_FUZZ        106
#define AMB_FLFO_AMT        107
#define AMB_LFO_SHAPE       108
#define AMB_PORTAMENTO      109
#define AMB_MOD_ATTACK      110
#define AMB_MOD_DECAY       111
#define AMB_MOD_SUSTAIN     112
#define AMB_MOD_RELEASE     113

#define NOTE_ON             150
#define NOTE1               70
#define VELOCITY            140

#define AMP                 35

#endif
