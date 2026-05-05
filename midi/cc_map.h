/**
 * MIDI Learn Tool - cc_map.h
 * Internal parameter database and common CC-to-parameter mapping.
 *
 * NOT DeepMind 12 specific - works with any controller and any synth engine.
 */

#ifndef CC_MAP_H
#define CC_MAP_H

#include <string>
#include <vector>
#include <map>
#include <cstdint>

// === MappingEntry - for reading/writing mapping files ===
struct MappingEntry {
    int cc;
    std::string paramName;
    int minVal;
    int maxVal;
    int defaultValue;
    
    MappingEntry() : cc(0), minVal(0), maxVal(127), defaultValue(64) {}
    MappingEntry(int _cc, const std::string& _name, int _min, int _max, int _def)
        : cc(_cc), paramName(_name), minVal(_min), maxVal(_max), defaultValue(_def) {}
};

// === ParamDef - for internal parameter database ===
struct ParamDef {
    int id;
    std::string name;
    std::string description;
    int category;       // 1=OSC 2=Filter 3=AmpEnv 4=FilterEnv 5=LFO 6=Volume 7=Effects 8=Unison 9=ModMatrix 10=Perf 11=Wave
    int engineMask;     // bitmask: 1=PBSynth 2=Cursynth 4=Twytch 8=Ncursesynth
    
    ParamDef() : id(0), category(0), engineMask(0) {}
    ParamDef(int _id, const char* _name, const char* _desc, int _cat, int _eng)
        : id(_id), name(_name), description(_desc), category(_cat), engineMask(_eng) {}
};

// === COMMON_CC_MAP - suggested parameter for each CC (DeepMind 12 + Cursynth defaults) ===
static const MappingEntry COMMON_CC_MAP[] = {
    {6,  "CUTOFF",                0, 127, 64},
    {7,  "VOLUME",                0, 127, 100},
    {9,  "CUTOFF",                0, 127, 64},
    {11, "RESONANCE",             0, 127, 0},
    {12, "FILTER_ENV_AMOUNT",     0, 127, 0},
    {17, "OSC1_TYPE",             0, 127, 64},
    {18, "OSC1_AMP",             0, 127, 64},
    {19, "OSC1_DETUNE",           -64, 63, 0},
    {20, "OSC1_SCALE",            0, 127, 29},
    {21, "OSC1_MOD",              -64, 64, 0},
    {22, "OSC2_TYPE",             0, 127, 64},
    {23, "OSC2_AMP",             0, 127, 64},
    {24, "OSC2_DETUNE",           -64, 63, 0},
    {25, "OSC2_SCALE",            0, 127, 60},
    {26, "OSC2_MOD",              -64, 64, 0},
    {27, "OSC12_MIX",             0, 127, 64},
    {28, "OSC12_MIX",             0, 127, 64},
    {29, "CUTOFF",                18, 120, 64},
    {30, "RESONANCE",             0, 127, 0},
    {31, "FILTER_ENV_AMOUNT",    -64, 64, 0},
    {32, "FILTER_ATTACK",         0, 127, 0},
    {33, "FILTER_DECAY",          0, 127, 64},
    {34, "FILTER_SUSTAIN",        0, 127, 96},
    {35, "HPF_FREQ",              0, 127, 0},
    {36, "VOLUME",                0, 127, 100},
    {37, "AMP_ATTACK",            0, 127, 0},
    {38, "AMP_DECAY",             0, 127, 64},
    {39, "AMP_DECAY",             0, 127, 64},
    {40, "AMP_SUSTAIN",           0, 127, 127},
    {41, "AMP_RELEASE",           0, 127, 64},
    {42, "FILTER_ATTACK",         0, 127, 0},
    {43, "FILTER_DECAY",          0, 127, 64},
    {44, "FILTER_SUSTAIN",        0, 127, 127},
    {45, "FILTER_RELEASE",        0, 127, 64},
    {46, "LFO1_FREQ",             0, 127, 0},
    {47, "LFO1_DEPTH",            -64, 64, 0},
    {48, "LFO2_FREQ",             0, 127, 0},
    {49, "LFO2_DEPTH",            -64, 64, 0},
    {50, "FILTER1_TYPE",          0, 127, 0},
    {51, "FILTER1_ALGO",          0, 127, 0},
    {52, "CUTOFF",                18, 120, 64},
    {53, "RESONANCE",             0, 127, 0},
    {54, "FILTER1_SATURATION",    0, 127, 0},
    {55, "KEYTRACK",              0, 127, 96},
    {56, "FILTER1_FEEDBACK",      -64, 64, 0},
    {57, "FILTER_ENV_AMOUNT",    -64, 64, 0},
    {58, "FILTER_ENV_CURVE",      0, 127, 0},
    {59, "DELAY_TIME",            0, 127, 0},
    {60, "FM_TYPE",               0, 127, 0},
    {61, "OSC1_DETUNE",           -64, 64, 0},
    {62, "OSC2_DETUNE",           -64, 64, 0},
    {63, "OSC2_TRANSPOSE",        -24, 24, 0},
    {64, "OSC1_DETUNE_FINE",     -64, 64, 0},
    {65, "OSC1_SCALE",            0, 127, 29},
    {66, "OSC2_SCALE",            0, 127, 60},
    {67, "OSC1_DETUNE",           -64, 64, 0},
    {68, "OSC1_SCALE",            0, 127, 29},
    {69, "OSC2_SCALE",            0, 127, 60},
    {70, "NOTE1",                 0, 127, 60},
    {71, "RESONANCE",             0, 127, 0},
    {72, "FILTER_ENV_AMOUNT",    -64, 64, 0},
    {73, "AMP_ATTACK",            0, 127, 0},
    {74, "CUTOFF",                18, 120, 64},
    {75, "AMP_DECAY",             0, 127, 64},
    {76, "AMP_SUSTAIN",           0, 127, 127},
    {77, "AMP_RELEASE",           0, 127, 64},
    {78, "OSC1_UNISON",           1, 10, 2},
    {79, "OSC2_UNISON",           1, 10, 1},
    {80, "OSC1_UNISON_DT",       -64, 64, 0},
    {81, "OSC2_UNISON_DT",       -64, 64, 0},
    {82, "UNISON_VOICES",         2, 8, 4},
    {83, "UNISON_SPREAD",        -64, 64, 0},
    {84, "FILTER_KEYTRACK",       0, 127, 96},
    {85, "FILTER_ENV_ATTACK",     0, 127, 0},
    {86, "FILTER_ENV_DECAY",      0, 127, 64},
    {87, "FILTER_ENV_SUSTAIN",    0, 127, 96},
    {88, "FILTER_ENV_RELEASE",    0, 127, 64},
    {89, "FILTER_ENV_CURVE",      0, 127, 0},
    {100, "DELAY_TIME",           0, 127, 0},
    {101, "DELAY_FEEDBACK",       0, 127, 0},
    {102, "DELAY_DRY_WET",        0, 127, 64},
    {103, "REVERB_AMOUNT",        0, 127, 0},
    {104, "REVERB_DECAY",         0, 127, 64},
    {105, "CHORUS_RATE",          0, 127, 0},
    {106, "CHORUS_DEPTH",         0, 127, 0},
    {107, "CHORUS_DRY_WET",       0, 127, 64},
    {108, "DISTORTION_DRIVE",     0, 127, 0},
    {109, "DISTORTION_DRY_WET",   0, 127, 64},
    {110, "POLYPHONY",            1, 16, 8},
    {111, "LEGATO",               0, 127, 0},
    {112, "PORTAMENTO_TIME",      0, 127, 0},
    {113, "PORTAMENTO_TYPE",      0, 127, 0},
    {114, "PITCH_BEND_RANGE",     1, 24, 12},
    {115, "UNISON_VOICES",        2, 8, 4},
    {116, "UNISON_SPREAD",       -64, 64, 0},
    {140, "VELOCITY",              0, 127, 64},
};
static const int COMMON_CC_MAP_SIZE = sizeof(COMMON_CC_MAP) / sizeof(COMMON_CC_MAP[0]);

// === PARAM_DB - complete parameter database across all 4 engines ===
static const ParamDef PARAM_DB[] = {
    // Category 1: Oscillator
    ParamDef(20, "OSC1_TYPE",        "OSC 1 Waveform",         1, 0x0F),
    ParamDef(22, "OSC3_TYPE",        "OSC 3 Sub Waveform",     1, 0x0E),
    ParamDef(23, "OSC4_TYPE",        "OSC 4 Noise Waveform",   1, 0x0E),
    ParamDef(28, "OSC12_MIX",        "Oscillator Mix",         1, 0x0E),
    ParamDef(29, "OSC1_MOD",         "OSC 1 Modulation",       1, 0x0E),
    ParamDef(31, "OSC1_AMP",         "OSC 1 Amplitude",        1, 0x0F),
    ParamDef(33, "OSC3_AMP",         "OSC 3 Sub Amplitude",    1, 0x0E),
    ParamDef(34, "OSC4_AMP",         "OSC 4 Noise Amplitude",  1, 0x0E),
    ParamDef(35, "AMP",              "Master Amplitude",       6, 0x0E),
    ParamDef(65, "OSC1_SCALE",       "OSC 1 Scale",            1, 0x0E),
    ParamDef(66, "OSC2_SCALE",       "OSC 2 Scale",            1, 0x0E),
    ParamDef(74, "OSC1_DETUNE",      "OSC 1 Detune",           1, 0x0F),
    ParamDef(75, "OSC2_DETUNE",      "OSC 2 Detune",           1, 0x0F),
    ParamDef(76, "OSC1_UNISON",      "OSC 1 Unison Voices",    8, 0x0E),
    ParamDef(77, "OSC2_UNISON",      "OSC 2 Unison Voices",    8, 0x0E),
    ParamDef(78, "OSC1_UNISON_DT",   "OSC 1 Unison Detune",    8, 0x0E),
    ParamDef(79, "OSC2_UNISON_DT",   "OSC 2 Unison Detune",    8, 0x0E),
    ParamDef(95, "ENV2_DEPTH",       "Env 2 Depth",            7, 0x0E),
    
    // Category 2: Filter
    ParamDef(50, "FILTER1_TYPE",     "Filter Type",            2, 0x0E),
    ParamDef(51, "FILTER1_ALGO",     "Filter Algorithm",       2, 0x0E),
    ParamDef(52, "FILTER1_CUTOFF",   "Filter Cutoff",         2, 0x0E),
    ParamDef(53, "FILTER1_RESONANCE","Filter Resonance",       2, 0x0E),
    ParamDef(55, "KEYTRACK",         "Key Track",             2, 0x0E),
    ParamDef(54, "FILTER1_SATURATION","Filter Saturation",     2, 0x0E),
    ParamDef(56, "FILTER1_FEEDBACK",  "Filter Feedback",       2, 0x0E),
    
    // Category 3: Amp Envelope
    ParamDef(0,  "AMP_ATTACK",       "Amp Attack",            3, 0x0E),
    ParamDef(1,  "AMP_DECAY",        "Amp Decay",             3, 0x0E),
    ParamDef(2,  "AMP_SUSTAIN",      "Amp Sustain",           3, 0x0E),
    ParamDef(3,  "AMP_RELEASE",      "Amp Release",           3, 0x0E),
    
    // Category 4: Filter Envelope
    ParamDef(4,  "FILTER_ATTACK",    "Filter Attack",         4, 0x0E),
    ParamDef(5,  "FILTER_DECAY",     "Filter Decay",          4, 0x0E),
    ParamDef(6,  "FILTER_SUSTAIN",   "Filter Sustain",        4, 0x0E),
    ParamDef(7,  "FILTER_RELEASE",   "Filter Release",        4, 0x0E),
    ParamDef(94, "FILTER_ENV_AMOUNT","Filter Env Depth",       4, 0x0E),
    
    // Category 5: LFO
    ParamDef(40, "LFO1_WAVEFORM",    "LFO 1 Waveform",        5, 0x0E),
    ParamDef(42, "LFO1_FREQ",        "LFO 1 Frequency",       5, 0x0E),
    ParamDef(41, "LFO1_DEPTH",       "LFO 1 Depth",           5, 0x0E),
    ParamDef(43, "LFO2_WAVEFORM",    "LFO 2 Waveform",        5, 0x0E),
    ParamDef(44, "LFO2_FREQ",        "LFO 2 Frequency",       5, 0x0E),
    ParamDef(43, "LFO2_DEPTH",       "LFO 2 Depth",           5, 0x0E),
    
    // Category 6: Volume
    ParamDef(7,  "VOLUME",           "Volume",                6, 0x0E),
    ParamDef(36, "VOLUME",           "Volume (alt)",          6, 0x0E),
    
    // Category 7: Effects
    ParamDef(100, "DELAY_TIME",      "Delay Time",            7, 0x0E),
    ParamDef(101, "DELAY_FEEDBACK",  "Delay Feedback",        7, 0x0E),
    ParamDef(102, "DELAY_DRY_WET",   "Delay Dry/Wet",         7, 0x0E),
    ParamDef(103, "REVERB_AMOUNT",   "Reverb Amount",         7, 0x0E),
    ParamDef(104, "REVERB_DECAY",    "Reverb Decay",          7, 0x0E),
    ParamDef(105, "CHORUS_RATE",     "Chorus Rate",           7, 0x0E),
    ParamDef(106, "CHORUS_DEPTH",    "Chorus Depth",          7, 0x0E),
    ParamDef(107, "CHORUS_DRY_WET",  "Chorus Dry/Wet",        7, 0x0E),
    ParamDef(108, "DISTORTION_DRIVE","Distortion Drive",       7, 0x0E),
    ParamDef(109, "DISTORTION_DRY_WET","Distortion Dry/Wet",  7, 0x0E),
    
    // Category 8: Unison
    ParamDef(82, "UNISON_VOICES",    "Unison Voices",         8, 0x0E),
    ParamDef(83, "UNISON_SPREAD",    "Unison Spread",         8, 0x0E),
    ParamDef(84, "UNISON_DETUNE",    "Unison Detune",         8, 0x0E),
    
    // Category 9: Mod Matrix
    ParamDef(130, "MOD_SOURCE_1",     "Mod Source 1",         9, 0x08),
    ParamDef(131, "MOD_SCALE_1",      "Mod Scale 1",          9, 0x08),
    ParamDef(132, "MOD_DESTINATION_1","Mod Destination 1",    9, 0x08),
    ParamDef(133, "MOD_SOURCE_2",     "Mod Source 2",         9, 0x08),
    ParamDef(134, "MOD_SCALE_2",      "Mod Scale 2",          9, 0x08),
    ParamDef(135, "MOD_DESTINATION_2","Mod Destination 2",    9, 0x08),
    ParamDef(136, "MOD_SOURCE_3",     "Mod Source 3",         9, 0x08),
    ParamDef(137, "MOD_SCALE_3",      "Mod Scale 3",          9, 0x08),
    ParamDef(138, "MOD_DESTINATION_3","Mod Destination 3",    9, 0x08),
    
    // Category 10: Performance
    ParamDef(110, "POLYPHONY",        "Polyphony",            10, 0x08),
    ParamDef(111, "LEGATO",           "Legato",               10, 0x08),
    ParamDef(112, "PORTAMENTO_TIME",  "Portamento Time",      10, 0x08),
    ParamDef(113, "PORTAMENTO_TYPE",  "Portamento Type",      10, 0x08),
    ParamDef(114, "PITCH_BEND_RANGE", "Pitch Bend Range",     10, 0x08),
    ParamDef(140, "VELOCITY",         "Velocity Scale",       10, 0x06),
};
static const int PARAM_DB_SIZE = sizeof(PARAM_DB) / sizeof(PARAM_DB[0]);

// === KNOWN_PARAM_NAMES - valid parameter names in mapping files ===
static const char* KNOWN_PARAM_NAMES[] = {
    "CUTOFF", "RESONANCE", "FILTER_ENV_AMOUNT", "HPF_FREQ",
    "FILTER_ATTACK", "FILTER_DECAY", "FILTER_SUSTAIN", "FILTER_RELEASE",
    "AMP_ATTACK", "AMP_DECAY", "AMP_SUSTAIN", "AMP_RELEASE",
    "VOLUME",
    "OSC1_TYPE", "OSC2_TYPE", "OSC3_TYPE", "OSC4_TYPE",
    "OSC1_AMP", "OSC2_AMP", "OSC3_AMP", "OSC4_AMP",
    "OSC1_DETUNE", "OSC2_DETUNE", "OSC1_UNISON", "OSC2_UNISON",
    "OSC1_UNISON_DT", "OSC2_UNISON_DT", "OSC1_SCALE", "OSC2_SCALE",
    "OSC12_MIX", "OSC1_MOD", "OSC1_WAVE", "OSC2_WAVE",
    "FILTER1_TYPE", "FILTER1_ALGO", "FILTER1_CUTOFF", "FILTER1_RESONANCE",
    "FILTER1_SATURATION", "FILTER1_FEEDBACK", "KEYTRACK",
    "LFO1_WAVEFORM", "LFO1_FREQ", "LFO1_DEPTH",
    "LFO2_WAVEFORM", "LFO2_FREQ", "LFO2_DEPTH",
    "DELAY_TIME", "DELAY_FEEDBACK", "DELAY_DRY_WET",
    "REVERB_AMOUNT", "REVERB_DECAY",
    "CHORUS_RATE", "CHORUS_DEPTH", "CHORUS_DRY_WET",
    "DISTORTION_DRIVE", "DISTORTION_DRY_WET",
    "POLYPHONY", "LEGATO", "PORTAMENTO_TIME", "PORTAMENTO_TYPE",
    "PITCH_BEND_RANGE", "UNISON_VOICES", "UNISON_SPREAD",
    "VELOCITY", "MOD_SOURCE_1", "MOD_SCALE_1", "MOD_DESTINATION_1",
    "MOD_SOURCE_2", "MOD_SCALE_2", "MOD_DESTINATION_2",
    "MOD_SOURCE_3", "MOD_SCALE_3", "MOD_DESTINATION_3",
    "AMP", "ENV2_DEPTH", "FM_TYPE", "NOTE1",
    "OSC1_DETUNE_FINE", "OSC2_DETUNE_FINE", "OSC2_TRANSPOSE",
    nullptr
};

// === NRPN equivalent CC table ===
static const int NRPN_MAP[][3] = {
    {0,  1,  74},  // NRPN(0,1) -> CC 74 (CUTOFF)
    {0,  2,  71},  // NRPN(0,2) -> CC 71 (RESONANCE)
    {0,  3,  72},  // NRPN(0,3) -> CC 72 (FILTER_ENV_AMOUNT)
    {0,  4,  73},  // NRPN(0,4) -> CC 73 (AMP_ATTACK)
    {0,  5,  75},  // NRPN(0,5) -> CC 75 (AMP_DECAY)
    {0,  6,  76},  // NRPN(0,6) -> CC 76 (AMP_SUSTAIN)
    {0,  7,  77},  // NRPN(0,7) -> CC 77 (AMP_RELEASE)
    {0, 19,  96},  // NRPN(0,19) -> CC 96
    {0, 20,  97},  // NRPN(0,20) -> CC 97
    {0, 21,  98},  // NRPN(0,21) -> CC 98
    {0, 22,  99},  // NRPN(0,22) -> CC 99
    {1,  0, 100},  // NRPN(1,0) -> CC 100 (DELAY_TIME)
    {-1, -1, -1},  // terminator
};

// === CC descriptions for --show-unknown ===
static const char* CC_DESCRIPTIONS[] = {
    "", "", "", "", "", "", "", "", "", "", // CC 0-9
    // OSC
    "Filter Env", "Resonance", "Filter Env Atk", // CC 10-12
    "OSC 1 Waveform", "OSC 1 Amp", "OSC 1 Detune", "OSC 1 Scale", "OSC 1 Mod", // CC 17-21
    "OSC 2 Waveform", "OSC 2 Amp", "OSC 2 Detune", "OSC 2 Scale", "OSC 2 Mod", // CC 22-26
    "Osc Mix", "Osc Mix", // CC 27-28
    // Filter
    "Cutoff", "Resonance", "Filter Env Amt", // CC 29-31
    "Filter Env Atk", "Filter Env Dec", "Filter Env Sus", // CC 32-34
    "HPF Freq / Amp", // CC 35
    "Volume", // CC 36
    // Amp envelope
    "Amp Attack", "Amp Decay/Data Entry", "Amp Decay", // CC 37-39
    "Amp Sustain", "Amp Release", // CC 40-41
    // Filter env / LFO
    "Filter Env Atk / LFO1 Freq", "Filter Env Dec", // CC 42-43
    "Filter Env Sus / LFO1 Depth", "Filter Env Rel", // CC 44-45
    "LFO 1 Freq", "LFO 1 Depth", // CC 46-47
    "LFO 2 Freq", "LFO 2 Depth", // CC 48-49
    // More
    "LFO 1 Waveform", "LFO 2 Waveform", "Filter Type", // CC 50-52
    "Filter Resonance", "Filter Saturation", "Key Track", "Filter Feedback", // CC 53-56
    "Filter Env Amt", "Filter Env Curve", "Delay Time", // CC 57-59
    "FM Type", "OSC 1 Detune", "OSC 2 Detune", "OSC 2 Transpose", "OSC 1 Detune Fine", // CC 60-64
    "OSC 1 Scale / Porta", "OSC 2 Scale", "OSC 1 Detune", "OSC 1 Scale", "OSC 2 Scale", // CC 65-69
    "Note Value", "Resonance (NRPN)", "Filter Env Amt (NRPN)", "Amp Attack (NRPN)", // CC 70-73
    "Cutoff (NRPN)", "Amp Decay (NRPN)", "Amp Sustain (NRPN)", "Amp Release (NRPN)", // CC 74-77
    "OSC 1 Unison", "OSC 2 Unison", "OSC 1 Unison DT", "OSC 2 Unison DT", // CC 78-81
    "Unison Voices", "Unison Spread", "Filter Keytrack", "Filter Env Atk", "Filter Env Dec", "Filter Env Sus", "Filter Env Rel", "Filter Env Curve",
    "Arp Mode", "Arp Tempo", "Arp Swing", "Arp Glide",
    "Filter Env Atk", "Filter Env Dec", "NRPN LSB", "NRPN MSB", // CC 96-99
    "Delay Time", "Delay Feedback", "Delay Dry/Wet", "Reverb Amt", "Reverb Decay", "Reverb Mix", "Delay Time", "Reverb Type",
    "Distortion Drive", "Distortion Drive",
    "Polyphony", "Legato", "Portamento Time", "Portamento Type",
    "Pitch Bend Range", "Unison Voices", "Unison Spread", "Unison Detune",
    "OSC 1 Detune", "OSC 2 Detune", "OSC 1 Detune Fine", "OSC 2 Detune Fine",
    "OSC 1 Unison", "OSC 2 Unison", "OSC 1 Detune", "OSC 2 Detune",
    "Mod Source", "Mod Scale",
};
static const int CC_DESC_SIZE = sizeof(CC_DESCRIPTIONS) / sizeof(CC_DESCRIPTIONS[0]);

// === lookup helpers ===

/*
 * Search COMMON_CC_MAP by CC number.
 * Returns const MappingEntry* or nullptr if not found.
 */
static const MappingEntry* lookupCommonCC(int cc) {
    for (int i = 0; i < COMMON_CC_MAP_SIZE; i++) {
        if (COMMON_CC_MAP[i].cc == cc) {
            return &COMMON_CC_MAP[i];
        }
    }
    return nullptr;
}

/*
 * Search PARAM_DB by name.
 * Returns const ParamDef* or nullptr if not found.
 */
static const ParamDef* lookupParamByName(const std::string& name) {
    for (int i = 0; i < PARAM_DB_SIZE; i++) {
        if (PARAM_DB[i].name == name) {
            return &PARAM_DB[i];
        }
    }
    return nullptr;
}

/*
 * Check if a parameter name is in the known list.
 */
static bool isKnownParam(const char* name) {
    for (int i = 0; KNOWN_PARAM_NAMES[i] != nullptr; i++) {
        if (KNOWN_PARAM_NAMES[i] == name) {
            return true;
        }
    }
    return false;
}

/*
 * Convert NRPN msb/lsb to equivalent CC number.
 * Returns -1 if not in table.
 */
static int nrpnToEquivCC(int msb, int lsb) {
    for (int i = 0; NRPN_MAP[i][0] >= 0; i++) {
        if (NRPN_MAP[i][0] == msb && NRPN_MAP[i][1] == lsb) {
            return NRPN_MAP[i][2];
        }
    }
    return -1;
}

/*
 * Check if CC number is an NRPN address control change.
 */
static bool isNRPNAddress(int cc) {
    return cc == 98 || cc == 99;
}

/*
 * Check if CC number is an NRPN data entry control change.
 */
static bool isNRPNDataCC(int cc) {
    return cc == 6 || cc == 38;
}

#endif // CC_MAP_H
