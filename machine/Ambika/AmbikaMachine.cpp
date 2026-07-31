#include "AmbikaMachine.h"
#include "ambika_types.h"
#include "plugin_shared.h"
#include "../ParamID.h"
#include <iostream>
#include <cmath>
#include <cstring>

static constexpr int kNumOscShapes = 37;   // NONE..WAVETABLE_16 (WAVEFORM_LAST - 2)
static constexpr int kNumFilterModes = 4;
static constexpr int kNumFilterTypes = 2;
static constexpr int kNumMixOps = 6;
static constexpr int kNumLfoShapes = 4;

// Translate the shared/unified parameter IDs (machine/ParamID.h plus the
// Ambika extended IDs from ambika_types.h) to the native Ambika param enum
// used by PluginVoiceManager (AmbikaParam in plugin_shared.h).
int AmbikaMachine::mapParam(int id) {    switch (id) {
        case ParamID::amp_attack:    return PARAM_ENV2_ATTACK;        // 21
        case ParamID::amp_decay:     return PARAM_ENV2_DECAY;         // 22
        case ParamID::amp_sustain:   return PARAM_ENV2_SUSTAIN;       // 23
        case ParamID::amp_release:   return PARAM_ENV2_RELEASE;       // 24
        case ParamID::flt_attack:    return PARAM_ENV1_ATTACK;        // 17
        case ParamID::flt_decay:     return PARAM_ENV1_DECAY;         // 18
        case ParamID::flt_sustain:   return PARAM_ENV1_SUSTAIN;       // 19
        case ParamID::flt_release:   return PARAM_ENV1_RELEASE;       // 20
        case ParamID::osc1_wave:     return PARAM_OSC1_SHAPE;         // 0
        case ParamID::osc2_wave:     return PARAM_OSC2_SHAPE;         // 4
        case ParamID::osc1_amp:      return PARAM_MIX_BALANCE;        // 12
        case ParamID::osc2_amp:      return PARAM_MIX_BALANCE;        // 12
        case ParamID::mix:           return PARAM_MIX_BALANCE;        // 12
        case ParamID::osc1_scale:    return PARAM_OSC1_RANGE;         // 2
        case ParamID::osc2_scale:    return PARAM_OSC2_RANGE;         // 6
        case ParamID::osc1_detune:   return PARAM_OSC1_DETUNE;        // 3
        case ParamID::osc2_detune:   return PARAM_OSC2_DETUNE;        // 7
        case ParamID::cutoff:        return PARAM_FILTER_CUTOFF;      // 8
        case ParamID::resonance:     return PARAM_FILTER_RESONANCE;   // 9
        case ParamID::filter_type:   return PARAM_FILTER_TYPE;        // 11
        case ParamID::lfo1_freq:     return PARAM_LFO_RATE;           // 30
        case ParamID::lfo1_depth:    return PARAM_FILTER_LFO_AMOUNT;  // 81
        case ParamID::flt_env_depth: return PARAM_FILTER_ENV_AMOUNT;  // 80
        case ParamID::volume:        return PARAM_VOLUME;             // 31
        case AMB_OSC1_PARAM:         return PARAM_OSC1_PARAMETER;     // 1
        case AMB_OSC2_PARAM:         return PARAM_OSC2_PARAMETER;     // 5
        case AMB_FILTER_MODE:        return PARAM_FILTER_MODE;        // 10
        case AMB_MIX_OP:             return PARAM_MIX_OPERATOR;       // 13
        case AMB_MIX_SUB:            return PARAM_MIX_SUB_OSC;        // 14
        case AMB_MIX_NOISE:          return PARAM_MIX_NOISE;          // 15
        case AMB_MIX_FUZZ:           return PARAM_MIX_FUZZ;           // 16
        case AMB_FLFO_AMT:           return PARAM_FILTER_LFO_AMOUNT;  // 81
        case AMB_LFO_SHAPE:          return PARAM_LFO_SHAPE;          // 29
        case AMB_PORTAMENTO:         return PARAM_PORTAMENTO;         // 32
        case AMB_MOD_ATTACK:         return PARAM_ENV3_ATTACK;        // 25
        case AMB_MOD_DECAY:          return PARAM_ENV3_DECAY;         // 26
        case AMB_MOD_SUSTAIN:        return PARAM_ENV3_SUSTAIN;       // 27
        case AMB_MOD_RELEASE:        return PARAM_ENV3_RELEASE;       // 28
        default:                     return id;
    }
}

AmbikaMachine::AmbikaMachine()
    : m_lastNote(-1)
    , m_hasActiveNotes(false)
    , m_resamplePos(0.0f)
    , m_resampleStep(31250.0f / 48000.0f)
    , m_osc1Shape(1), m_osc2Shape(0)
    , m_filterMode(0), m_filterType(0)
    , m_mixOp(0), m_lfoShape(0)
    , m_fltEnvDepth(64)
{
    setName("Ambika");
    m_vm = new PluginVoiceManager();
    std::memset(m_resampleBuf, 0, sizeof(m_resampleBuf));
}

AmbikaMachine::~AmbikaMachine()
{
    delete m_vm;
}

void AmbikaMachine::init()
{
    m_vm->ResetToInit();
    m_lastNote = -1;
    m_hasActiveNotes = false;
    m_resamplePos = 0.0f;
    std::memset(m_resampleBuf, 0, sizeof(m_resampleBuf));
    m_osc1Shape = 1;
    m_osc2Shape = 0;
    m_filterMode = 0;
    m_filterType = 0;
    m_mixOp = 0;
    m_lfoShape = 0;
    m_fltEnvDepth = 64;
}

void AmbikaMachine::reset()
{
    m_vm->Panic();
    m_lastNote = -1;
    m_hasActiveNotes = false;
    m_resamplePos = 0.0f;
    std::memset(m_resampleBuf, 0, sizeof(m_resampleBuf));
}

void AmbikaMachine::processBlock()
{
    float tmp[40];
    std::memset(tmp, 0, sizeof(tmp));

    for (int v = 0; v < 6; ++v)
        m_vm->voices_[v].ProcessBlock();

    for (int i = 0; i < 40; ++i) {
        float sum = 0.0f;
        for (int v = 0; v < 6; ++v) {
            float s = (m_vm->voices_[v].output()[i] - 128.0f) / 128.0f;
            sum += s;
        }
        tmp[i] = sum * m_vm->master_gain_;
        if (tmp[i] > 1.0f) tmp[i] = 1.0f;
        if (tmp[i] < -1.0f) tmp[i] = -1.0f;
    }

    std::memcpy(m_resampleBuf, tmp, sizeof(m_resampleBuf));
}

int32_t AmbikaMachine::tick()
{
    if (m_resamplePos >= 40.0f - 1.001f) {
        m_resamplePos -= 40.0f;
        processBlock();
    }

    int idx = (int)m_resamplePos;
    float frac = m_resamplePos - idx;
    int next = idx + 1;
    if (next >= 40) next = 39;

    float sample = m_resampleBuf[idx] + frac * (m_resampleBuf[next] - m_resampleBuf[idx]);
    m_resamplePos += m_resampleStep;

    return (int32_t)(sample * OUTPUT_AMPLITUDE);
}

void AmbikaMachine::noteOn()
{
    int note = m_lastNote;
    if (note < 0) note = 60;
    m_vm->NoteOn(0, note, 100);
    m_hasActiveNotes = true;
}

void AmbikaMachine::noteOff()
{
    int note = m_lastNote;
    if (note < 0) return;
    m_vm->NoteOff(0, note, 0);
    m_hasActiveNotes = false;
}

void AmbikaMachine::setI(int index, int value)
{
    if (index == 150) {
        if (value) {
            int note = m_lastNote;
            if (note < 0) note = 60;
            m_vm->NoteOn(0, note, 100);
            m_hasActiveNotes = true;
        } else {
            int note = m_lastNote;
            if (note >= 0) m_vm->NoteOff(0, note, 0);
            m_hasActiveNotes = false;
        }
        return;
    }
    if (index == 70) {
        m_lastNote = value;
        return;
    }
    if (index == 71) {
        // NOTE_HZ is used by ncursesynth-style tests to set the note; it is
        // redundant with index 70 here. Without this guard, it would fall
        // through to SetParam(PARAM_MOD_AMOUNT_12) and activate the LFO1→OSC1
        // pitch modulation, sweeping the oscillator wildly off-pitch.
        return;
    }
    if (index == 140) {
        return;
    }

    // Discrete params: value is the discrete index, map to 0.0-1.0 for engine
    if (index == ParamID::osc1_wave || index == ParamID::osc2_wave) {
        int& cache = (index == ParamID::osc1_wave) ? m_osc1Shape : m_osc2Shape;
        if (value < 0) value = 0;
        if (value >= kNumOscShapes) value = kNumOscShapes - 1;
        cache = value;
        m_vm->SetParam(mapParam(index), (float)value / (float)(kNumOscShapes - 1));
        return;
    }
    if (index == AMB_FILTER_MODE) {
        if (value < 0) value = 0;
        if (value >= kNumFilterModes) value = kNumFilterModes - 1;
        m_filterMode = value;
        m_vm->SetParam(mapParam(index), (float)value / (float)(kNumFilterModes - 1));
        return;
    }
    if (index == ParamID::filter_type) {
        m_filterType = value > 0 ? 1 : 0;
        m_vm->SetParam(mapParam(index), m_filterType > 0 ? 1.0f : 0.0f);
        return;
    }
    if (index == AMB_MIX_OP) {
        if (value < 0) value = 0;
        if (value >= kNumMixOps) value = kNumMixOps - 1;
        m_mixOp = value;
        m_vm->SetParam(mapParam(index), (float)value / (float)(kNumMixOps - 1));
        return;
    }
    if (index == AMB_LFO_SHAPE) {
        if (value < 0) value = 0;
        if (value >= kNumLfoShapes) value = kNumLfoShapes - 1;
        m_lfoShape = value;
        m_vm->SetParam(mapParam(index), (float)value / (float)(kNumLfoShapes - 1));
        return;
    }

    if (index == ParamID::flt_env_depth) {
        // Logarithmic (power) curve: fine resolution near center, where the
        // useful filter-envelope amount lives (roughly -15%..+15% of the
        // control), expanding toward full +/- at the extremes.
        m_fltEnvDepth = value;
        float t;
        if (value <= 64) {
            t = (value - 64) / 64.0f;
        } else {
            t = (value - 64) / 63.0f;
        }
        if (t < -1.0f) t = -1.0f;
        if (t > 1.0f) t = 1.0f;
        float sign = t < 0.0f ? -1.0f : 1.0f;
        float mag = powf(sign * t, 1.5f);
        float normalized = 0.5f + sign * mag * 0.5f;
        m_vm->SetParam(PARAM_FILTER_ENV_AMOUNT, normalized);
        return;
    }

    // Continuous/bipolar params: value is 0-127
    float normalized = value / 127.0f;
    if (normalized < 0.0f) normalized = 0.0f;
    if (normalized > 1.0f) normalized = 1.0f;

    m_vm->SetParam(mapParam(index), normalized);
}

void AmbikaMachine::setF(int index, float value)
{
    int intVal = (int)(value * 127.0f);
    if (intVal < 0) intVal = 0;
    if (intVal > 127) intVal = 127;
    setI(index, intVal);
}

int AmbikaMachine::getI(int index)
{
    if (index == 70) return m_lastNote;
    if (index == 150) return m_hasActiveNotes ? 1 : 0;

    // Discrete params: return cached discrete index directly
    switch (index) {
        case ParamID::osc1_wave:   return m_osc1Shape;
        case ParamID::osc2_wave:   return m_osc2Shape;
        case AMB_FILTER_MODE:      return m_filterMode;
        case ParamID::filter_type: return m_filterType;
        case AMB_MIX_OP:           return m_mixOp;
        case AMB_LFO_SHAPE:        return m_lfoShape;
        case ParamID::flt_env_depth: return m_fltEnvDepth;
        default: break;
    }

    // Continuous/bipolar params: convert 0.0-1.0 to 0-127
    float v = m_vm->GetParam(mapParam(index));
    return (int)(v * 127.0f);
}

float AmbikaMachine::getF(int index)
{
    return getI(index) / 127.0f;
}

void AmbikaMachine::applyCC(int cc, float normalized, const std::string& paramName)
{
    int val = (int)(normalized * 127.0f);

    if (paramName == "CUTOFF") {
        setI(ParamID::cutoff, val);
    } else if (paramName == "RESONANCE") {
        setI(ParamID::resonance, val);
    } else if (paramName == "FILTER_ENV_AMOUNT") {
        setI(ParamID::flt_env_depth, val);
    } else if (paramName == "FILTER_ATTACK") {
        setI(ParamID::flt_attack, val);   // Env1 = Filter envelope
    } else if (paramName == "FILTER_DECAY") {
        setI(ParamID::flt_decay, val);
    } else if (paramName == "FILTER_SUSTAIN") {
        setI(ParamID::flt_sustain, val);
    } else if (paramName == "FILTER_RELEASE") {
        setI(ParamID::flt_release, val);
    } else if (paramName == "AMP_ATTACK") {
        setI(ParamID::amp_attack, val);   // Env2 = Amp envelope
    } else if (paramName == "AMP_DECAY") {
        setI(ParamID::amp_decay, val);
    } else if (paramName == "AMP_SUSTAIN") {
        setI(ParamID::amp_sustain, val);
    } else if (paramName == "AMP_RELEASE") {
        setI(ParamID::amp_release, val);
    } else if (paramName == "OSC1_DETUNE") {
        setI(ParamID::osc1_detune, val);
    } else if (paramName == "OSC2_DETUNE") {
        setI(ParamID::osc2_detune, val);
    } else if (paramName == "OSC1_SCALE") {
        setI(ParamID::osc1_scale, val);
    } else if (paramName == "OSC2_SCALE") {
        setI(ParamID::osc2_scale, val);
    } else if (paramName == "OSC1_AMP") {
        setI(ParamID::osc1_amp, val);
    } else if (paramName == "OSC2_AMP") {
        setI(ParamID::osc2_amp, val);
    } else if (paramName == "OSC1_PARAM") {
        setI(AMB_OSC1_PARAM, val);
    } else if (paramName == "OSC2_PARAM") {
        setI(AMB_OSC2_PARAM, val);
    } else if (paramName == "MOD_ATTACK") {
        setI(AMB_MOD_ATTACK, val);   // Env3 = MOD envelope
    } else if (paramName == "MOD_DECAY") {
        setI(AMB_MOD_DECAY, val);
    } else if (paramName == "MOD_SUSTAIN") {
        setI(AMB_MOD_SUSTAIN, val);
    } else if (paramName == "MOD_RELEASE") {
        setI(AMB_MOD_RELEASE, val);
    } else if (paramName == "VOLUME") {
        setI(ParamID::volume, val);
    } else if (paramName == "NOISE") {
        setI(AMB_MIX_NOISE, val);
    } else if (paramName == "HPF_FREQ") {
    } else {
        if (midiDebug_)
            std::cerr << "Ambika applyCC: " << paramName << " unmapped" << std::endl;
    }
}

const char* AmbikaMachine::getDisplayString(int index)
{
    static const char* oscNames[] = {
        "NONE", "SAW", "SQR", "TRI", "SINE",
        "C SAW", "C LP", "C PK", "C BP", "C HP",
        "C PLP", "C PP", "C PBP", "C PHP", "C TRI",
        "QUAD", "FM", "8BIT", "DPWM", "FNOIS",
        "VOWEL", "WT1", "WT2", "WT3", "WT4",
        "WT5", "WT6", "WT7", "WT8", "WT9",
        "WT10", "WT11", "WT12", "WT13", "WT14",
        "WT15", "WT16"
    };
    static const char* filterModeNames[] = {"LP", "BP", "HP", "NCH"};
    static const char* filterTypeNames[] = {"2POLE", "4POLE"};
    static const char* opNames[] = {"SUM", "SYNC", "RING", "XOR", "FOLD", "BITS"};
    static const char* lfoShapeNames[] = {"TRI", "SQR", "S&H", "RAMP"};

    switch (index) {
        case ParamID::osc1_wave:
        case ParamID::osc2_wave: {
            int idx = (index == ParamID::osc1_wave) ? m_osc1Shape : m_osc2Shape;
            if (idx < 0) idx = 0;
            if (idx >= kNumOscShapes) idx = kNumOscShapes - 1;
            return oscNames[idx];
        }
        case AMB_FILTER_MODE: {
            if (m_filterMode < 0) return filterModeNames[0];
            if (m_filterMode >= kNumFilterModes) return filterModeNames[kNumFilterModes - 1];
            return filterModeNames[m_filterMode];
        }
        case ParamID::filter_type: {
            return filterTypeNames[m_filterType > 0 ? 1 : 0];
        }
        case AMB_MIX_OP: {
            if (m_mixOp < 0) return opNames[0];
            if (m_mixOp >= kNumMixOps) return opNames[kNumMixOps - 1];
            return opNames[m_mixOp];
        }
        case AMB_LFO_SHAPE: {
            if (m_lfoShape < 0) return lfoShapeNames[0];
            if (m_lfoShape >= kNumLfoShapes) return lfoShapeNames[kNumLfoShapes - 1];
            return lfoShapeNames[m_lfoShape];
        }
        default:
            break;
    }
    return "";
}

std::vector<std::pair<std::string, int>> AmbikaMachine::getPresetParams() const
{
    return {
        {"OSC1_SHAPE",   ParamID::osc1_wave},
        {"OSC1_PARAM",   AMB_OSC1_PARAM},
        {"OSC1_RANGE",   ParamID::osc1_scale},
        {"OSC1_DETUNE",  ParamID::osc1_detune},
        {"OSC2_SHAPE",   ParamID::osc2_wave},
        {"OSC2_PARAM",   AMB_OSC2_PARAM},
        {"OSC2_RANGE",   ParamID::osc2_scale},
        {"OSC2_DETUNE",  ParamID::osc2_detune},
        {"CUTOFF",       ParamID::cutoff},
        {"RESONANCE",    ParamID::resonance},
        {"FILTER_MODE",  AMB_FILTER_MODE},
        {"FILTER_TYPE",  ParamID::filter_type},
        {"BALANCE",      ParamID::mix},
        {"MIX_OP",       AMB_MIX_OP},
        {"SUB_OSC",      AMB_MIX_SUB},
        {"NOISE",        AMB_MIX_NOISE},
        {"FUZZ",         AMB_MIX_FUZZ},
        {"FENV_AMT",     ParamID::flt_env_depth},
        {"FLFO_AMT",     AMB_FLFO_AMT},
        {"FLT_ATTACK",   ParamID::flt_attack},
        {"FLT_DECAY",    ParamID::flt_decay},
        {"FLT_SUSTAIN",  ParamID::flt_sustain},
        {"FLT_RELEASE",  ParamID::flt_release},
        {"AMP_ATTACK",   ParamID::amp_attack},
        {"AMP_DECAY",    ParamID::amp_decay},
        {"AMP_SUSTAIN",  ParamID::amp_sustain},
        {"AMP_RELEASE",  ParamID::amp_release},
        {"MOD_ATTACK",   AMB_MOD_ATTACK},
        {"MOD_DECAY",    AMB_MOD_DECAY},
        {"MOD_SUSTAIN",  AMB_MOD_SUSTAIN},
        {"MOD_RELEASE",  AMB_MOD_RELEASE},
        {"LFO_SHAPE",    AMB_LFO_SHAPE},
        {"LFO_RATE",     ParamID::lfo1_freq},
        {"PORTAMENTO",   AMB_PORTAMENTO},
        {"VOLUME",       ParamID::volume}
    };
}
