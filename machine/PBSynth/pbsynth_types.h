#ifndef PBSYNTH_TYPES_H
#define PBSYNTH_TYPES_H

#include <cstdint>

// Basic types
#define Sint32 int32_t
#define Sint16 int16_t
#define Sint8 int8_t
#define Uint32 uint32_t
#define Uint16 uint16_t
#define Uint8 uint8_t

// Oscillator types
#define PICO_PBSYNTH_SIZE 4
#define PICO_PBSYNTH_SQUARE 0
#define PICO_PBSYNTH_TRIANGLE 1
#define PICO_PBSYNTH_SAW 2
#define PICO_PBSYNTH_NOISE 3

// Filter
#define PBSYNTH_FILTER_ALGO_SIZE 2
#define PBSYNTH_FILTER_ALGO_PBLP 0

#define FILTER_TYPE_SIZE 4
#define PBSYNTH_FILTER_TYPE_SIZE 4
#define PBSYNTH_FILTER_TYPE_LP12 0
#define PBSYNTH_FILTER_TYPE_LP24 1
#define PBSYNTH_FILTER_TYPE_HP12 2
#define PBSYNTH_FILTER_TYPE_HP24 3

// FM
#define FM_TYPE_SIZE 3
#define FM_TYPE_AM 0
#define FM_TYPE_PM 1
#define FM_TYPE_NONE 2
#define FM_TYPE_FM 2

// Macros that were missing
#define DPRINTF(...)

// From interfacepicoloop/Machine.h - essential parameter IDs
#define OSC1_TYPE           20
#define OSC2_TYPE           21

#define OSC1_SCALE          65
#define OSC2_SCALE          66

#define OSC1_DETUNE         74
#define OSC2_DETUNE         75

#define FILTER1_TYPE        50
#define FILTER1_ALGO        51

#define FM_TYPE             60

#define LFO1_FREQ           42
#define LFO2_FREQ           44

#define NOTE_ON             150

#define TRIG_TIME_MODE      101
#define TRIG_TIME_DURATION 102

// ADSR Envelopes
#define ADSR_ENV0_ATTACK    0
#define ADSR_ENV0_DECAY     1
#define ADSR_ENV0_SUSTAIN   2
#define ADSR_ENV0_RELEASE   3

#define ADSR_AMP_ATTACK     0
#define ADSR_AMP_DECAY     1
#define ADSR_AMP_SUSTAIN    2
#define ADSR_AMP_RELEASE    3

#define ADSR_ENV1_ATTACK    4
#define ADSR_ENV1_DECAY     5
#define ADSR_ENV1_SUSTAIN   6
#define ADSR_ENV1_RELEASE   7

#define ADSR_FLTR_ATTACK    4
#define ADSR_FLTR_DECAY     5
#define ADSR_FLTR_SUSTAIN   6
#define ADSR_FLTR_RELEASE   7

// VCO Mix and other
#define VCO_MIX             28
#define OSC12_MIX           28
#define OSC1_AMP            31
#define OSC2_AMP            32
#define AMP                 35

#define LFO1_DEPTH          41
#define LFO2_DEPTH          43
#define NOTE1               70

#define ENV1_DEPTH          94
#define ENV2_DEPTH          95

#define FILTER1_CUTOFF      52
#define FILTER1_RESONANCE   53

// Dummy Fixed type for compilation
typedef int Fixed;

#endif