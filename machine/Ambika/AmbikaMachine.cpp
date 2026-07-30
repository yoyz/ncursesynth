#include "AmbikaMachine.h"
#include "ambika_types.h"
#include "plugin_shared.h"
#include <iostream>
#include <cmath>
#include <cstring>

static constexpr int kNumOscShapes = 35;
static constexpr int kNumFilterModes = 4;
static constexpr int kNumFilterTypes = 2;
static constexpr int kNumMixOps = 6;
static constexpr int kNumLfoShapes = 4;

AmbikaMachine::AmbikaMachine()
    : m_lastNote(-1)
    , m_hasActiveNotes(false)
    , m_resamplePos(0.0f)
    , m_resampleStep(31250.0f / 48000.0f)
    , m_osc1Shape(0), m_osc2Shape(1)
    , m_filterMode(0), m_filterType(0)
    , m_mixOp(0), m_lfoShape(0)
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
    m_osc1Shape = 0;
    m_osc2Shape = 1;
    m_filterMode = 0;
    m_filterType = 0;
    m_mixOp = 0;
    m_lfoShape = 0;
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
            m_vm->NoteOn(0, m_lastNote, 100);
            m_hasActiveNotes = true;
        } else {
            m_vm->NoteOff(0, m_lastNote, 0);
            m_hasActiveNotes = false;
        }
        return;
    }
    if (index == 70) {
        m_lastNote = value;
        return;
    }
    if (index == 140) {
        return;
    }

    // Discrete params: value is the discrete index, map to 0.0-1.0 for engine
    if (index == 0 || index == 4) {
        int& cache = (index == 0) ? m_osc1Shape : m_osc2Shape;
        if (value < 0) value = 0;
        if (value >= kNumOscShapes) value = kNumOscShapes - 1;
        cache = value;
        m_vm->SetParam(index, (float)value / (float)(kNumOscShapes - 1));
        return;
    }
    if (index == 10) {
        if (value < 0) value = 0;
        if (value >= kNumFilterModes) value = kNumFilterModes - 1;
        m_filterMode = value;
        m_vm->SetParam(index, (float)value / (float)(kNumFilterModes - 1));
        return;
    }
    if (index == 11) {
        m_filterType = value > 0 ? 1 : 0;
        m_vm->SetParam(index, m_filterType > 0 ? 1.0f : 0.0f);
        return;
    }
    if (index == 13) {
        if (value < 0) value = 0;
        if (value >= kNumMixOps) value = kNumMixOps - 1;
        m_mixOp = value;
        m_vm->SetParam(index, (float)value / (float)(kNumMixOps - 1));
        return;
    }
    if (index == 29) {
        if (value < 0) value = 0;
        if (value >= kNumLfoShapes) value = kNumLfoShapes - 1;
        m_lfoShape = value;
        m_vm->SetParam(index, (float)value / (float)(kNumLfoShapes - 1));
        return;
    }

    // Continuous/bipolar params: value is 0-127
    float normalized = value / 127.0f;
    if (normalized < 0.0f) normalized = 0.0f;
    if (normalized > 1.0f) normalized = 1.0f;

    m_vm->SetParam(index, normalized);
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
        case 0: return m_osc1Shape;
        case 4: return m_osc2Shape;
        case 10: return m_filterMode;
        case 11: return m_filterType;
        case 13: return m_mixOp;
        case 29: return m_lfoShape;
        default: break;
    }

    // Continuous/bipolar params: convert 0.0-1.0 to 0-127
    float v = m_vm->GetParam(index);
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
        setI(8, val);
    } else if (paramName == "RESONANCE") {
        setI(9, val);
    } else if (paramName == "FILTER_ENV_AMOUNT") {
        setI(80, val);
    } else if (paramName == "FILTER_ATTACK") {
        setI(17, val);  // Env1 = Filter envelope
    } else if (paramName == "FILTER_DECAY") {
        setI(18, val);
    } else if (paramName == "FILTER_SUSTAIN") {
        setI(19, val);
    } else if (paramName == "FILTER_RELEASE") {
        setI(20, val);
    } else if (paramName == "AMP_ATTACK") {
        setI(21, val);  // Env2 = Amp envelope
    } else if (paramName == "AMP_DECAY") {
        setI(22, val);
    } else if (paramName == "AMP_SUSTAIN") {
        setI(23, val);
    } else if (paramName == "AMP_RELEASE") {
        setI(24, val);
    } else if (paramName == "OSC1_DETUNE") {
        setI(3, val);
    } else if (paramName == "OSC2_DETUNE") {
        setI(7, val);
    } else if (paramName == "OSC1_SCALE") {
        setI(2, val);
    } else if (paramName == "OSC2_SCALE") {
        setI(6, val);
    } else if (paramName == "OSC1_AMP") {
        setI(12, val);
    } else if (paramName == "OSC2_AMP") {
    } else if (paramName == "VOLUME") {
        setI(31, val);
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
        "CZ SW", "C LP", "C PK", "C BP", "C HP",
        "C PLP", "C PP", "C PBP", "C PHP",
        "C TRI", "QUAD", "FM", "8BIT",
        "DPWM", "FNOIS", "VOWEL", "WTBL", "WSEQ",
        "WT2", "WT3", "WT4", "WT5", "WT6",
        "WT7", "WT8", "WT9", "WT10", "WT11",
        "WT12", "WT13", "WT14", "WT15", "WT16",
        "WSEQ"
    };
    static const char* filterModeNames[] = {"LP", "BP", "HP", "NCH"};
    static const char* filterTypeNames[] = {"2POLE", "4POLE"};
    static const char* opNames[] = {"SUM", "SYNC", "RING", "XOR", "FOLD", "BITS"};
    static const char* lfoShapeNames[] = {"TRI", "SQR", "S&H", "RAMP"};

    switch (index) {
        case 0:
        case 4: {
            int idx = (index == 0) ? m_osc1Shape : m_osc2Shape;
            if (idx < 0) idx = 0;
            if (idx > 35) idx = 35;
            return oscNames[idx];
        }
        case 10: {
            if (m_filterMode < 0) return filterModeNames[0];
            if (m_filterMode >= kNumFilterModes) return filterModeNames[kNumFilterModes - 1];
            return filterModeNames[m_filterMode];
        }
        case 11: {
            return filterTypeNames[m_filterType > 0 ? 1 : 0];
        }
        case 13: {
            if (m_mixOp < 0) return opNames[0];
            if (m_mixOp >= kNumMixOps) return opNames[kNumMixOps - 1];
            return opNames[m_mixOp];
        }
        case 29: {
            if (m_lfoShape < 0) return lfoShapeNames[0];
            if (m_lfoShape >= kNumLfoShapes) return lfoShapeNames[kNumLfoShapes - 1];
            return lfoShapeNames[m_lfoShape];
        }
        default:
            break;
    }
    return "";
}
