#include "DigitsMachine.h"
#include <iostream>
#include "digits_types.h"

DigitsMachine::DigitsMachine()
    : m_bufferIndex(0)
{
    setName("Digits");
    m_engine.init();
}

DigitsMachine::~DigitsMachine()
{
}

void DigitsMachine::init()
{
    m_engine.init();
    m_engine.reset();
    m_bufferIndex = 0;
}

void DigitsMachine::reset()
{
    m_engine.reset();
    m_bufferIndex = 0;
}

int32_t DigitsMachine::tick()
{
    if (m_bufferIndex >= DIGITS_SAM || m_bufferIndex < 0)
        m_bufferIndex = 0;

    if (m_bufferIndex == 0)
        m_engine.process();

    float s = m_engine.getOutputBuffer()[m_bufferIndex];
    m_bufferIndex++;

    return (int32_t)(s * OUTPUT_AMPLITUDE);
}

void DigitsMachine::noteOn()
{
    int note = m_engine.getLastNote();
    m_engine.noteOn(note, 100);
}

void DigitsMachine::noteOff()
{
    int note = m_engine.getLastNote();
    m_engine.noteOff(note);
}

void DigitsMachine::setI(int index, int value)
{
    m_engine.setI(index, value);
}

void DigitsMachine::setF(int index, float value)
{
    int intVal = (int)(value * 127.0f);
    if (intVal < 0) intVal = 0;
    if (intVal > 127) intVal = 127;
    setI(index, intVal);
}

int DigitsMachine::getI(int index)
{
    return m_engine.getI(index);
}

float DigitsMachine::getF(int index)
{
    return getI(index) / 127.0f;
}

void DigitsMachine::applyCC(int cc, float normalized, const std::string& paramName)
{
    int val = (int)(normalized * 127.0f);

    if (paramName == "CUTOFF") {
        setI(FILTER1_CUTOFF, val);
    } else if (paramName == "RESONANCE") {
        setI(FILTER1_RESONANCE, val);
    } else if (paramName == "FILTER_ENV_AMOUNT") {
        setI(ENV1_DEPTH, val);
    } else if (paramName == "FILTER_ATTACK") {
        setI(ADSR_ENV1_ATTACK, val);
    } else if (paramName == "FILTER_DECAY") {
        setI(ADSR_ENV1_DECAY, val);
    } else if (paramName == "FILTER_SUSTAIN") {
        setI(ADSR_ENV1_SUSTAIN, val);
    } else if (paramName == "FILTER_RELEASE") {
        setI(ADSR_ENV1_RELEASE, val);
    } else if (paramName == "AMP_ATTACK") {
        setI(ADSR_ENV0_ATTACK, val);
    } else if (paramName == "AMP_DECAY") {
        setI(ADSR_ENV0_DECAY, val);
    } else if (paramName == "AMP_SUSTAIN") {
        setI(ADSR_ENV0_SUSTAIN, val);
    } else if (paramName == "AMP_RELEASE") {
        setI(ADSR_ENV0_RELEASE, val);
    } else if (paramName == "OSC1_DETUNE") {
        setI(OSC1_DETUNE, val);
    } else if (paramName == "OSC2_DETUNE") {
        setI(OSC2_DETUNE, val);
    } else if (paramName == "OSC1_SCALE") {
        setI(OSC1_SCALE, val);
    } else if (paramName == "OSC2_SCALE") {
        setI(OSC2_SCALE, val);
    } else if (paramName == "OSC1_AMP") {
        setI(OSC1_AMP, val);
    } else if (paramName == "OSC2_AMP") {
        setI(OSC2_AMP, val);
    } else if (paramName == "VOLUME") {
        setI(AMP, val);
    } else if (paramName == "HPF_FREQ") {
    } else {
        if (midiDebug_)
            std::cerr << "Digits applyCC: " << paramName << " unmapped" << std::endl;
    }
}

const char* DigitsMachine::getDisplayString(int index)
{
    static const char* basisNames[] = {"COS", "SIN"};
    static const char* lfoShapeNames[] = {"SIN", "SAW", "SQR", "NOISE"};

    if (index == OSC1_BASIS || index == OSC2_BASIS) {
        int val = (index == OSC1_BASIS) ? m_engine.getI(OSC1_BASIS) : m_engine.getI(OSC2_BASIS);
        int idx = (val > 0) ? 1 : 0;
        return basisNames[idx];
    }
    if (index == LFO1_SHAPE) {
        int idx = m_engine.getI(LFO1_SHAPE);
        if (idx < 0) idx = 0;
        if (idx > 3) idx = 3;
        return lfoShapeNames[idx];
    }
    return "";
}

std::vector<std::pair<std::string, int>> DigitsMachine::getPresetParams() const
{
    return {
        {"OSC1_TYPE", OSC1_TYPE},
        {"OSC2_TYPE", OSC2_TYPE},
        {"OSC1_SHAPER", OSC1_SHAPER},
        {"OSC1_SKEW", OSC1_SKEW},
        {"OSC1_BASIS", OSC1_BASIS},
        {"OSC2_SHAPER", OSC2_SHAPER},
        {"OSC2_SKEW", OSC2_SKEW},
        {"OSC2_BASIS", OSC2_BASIS},
        {"OSC_MIX", OSC12_MIX},
        {"OSC1_DETUNE", OSC1_DETUNE},
        {"OSC2_DETUNE", OSC2_DETUNE},
        {"FILTER_TYPE", FILTER1_TYPE},
        {"CUTOFF", FILTER1_CUTOFF},
        {"RESONANCE", FILTER1_RESONANCE},
        {"FILTER_ENV_DEPTH", ENV1_DEPTH},
        {"AMP_ATTACK", ADSR_ENV0_ATTACK},
        {"AMP_DECAY", ADSR_ENV0_DECAY},
        {"AMP_SUSTAIN", ADSR_ENV0_SUSTAIN},
        {"AMP_RELEASE", ADSR_ENV0_RELEASE},
        {"FILTER_ENV_ATTACK", ADSR_ENV1_ATTACK},
        {"FILTER_ENV_DECAY", ADSR_ENV1_DECAY},
        {"FILTER_ENV_SUSTAIN", ADSR_ENV1_SUSTAIN},
        {"FILTER_ENV_RELEASE", ADSR_ENV1_RELEASE},
        {"VOLUME", AMP},
        {"LFO1_FREQ", LFO1_FREQ},
        {"LFO1_DEPTH", LFO1_DEPTH},
        {"LFO1_SHAPE", LFO1_SHAPE},
        {"RESO_VOL", RESO_VOL}
    };
}
