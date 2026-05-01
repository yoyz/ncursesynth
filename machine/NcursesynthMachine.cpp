#include "NcursesynthMachine.h"
#include <cmath>

NcursesynthMachine::NcursesynthMachine() : synth_(nullptr), noteOn_(0) {
    setName("Ncursesynth");
    for (int i = 0; i < MachineParam::PERFORMANCE_COUNT; i++) {
        params_[i] = 0;
    }
}

NcursesynthMachine::~NcursesynthMachine() {}

void NcursesynthMachine::init() {
    // No auto-trigger at startup
}

void NcursesynthMachine::reset() {
    if (synth_) {
        synth_->reset();
    }
    noteOn_ = 0;
}

int32_t NcursesynthMachine::tick() {
    if (synth_) {
        float sample = synth_->process();
        return static_cast<int32_t>(sample * 640.0f);
    }
    return 0;
}

void NcursesynthMachine::noteOn() {
    noteOn_ = 1;
    if (synth_) {
        synth_->noteOn(440.0f);
    }
}

void NcursesynthMachine::noteOff() {
    noteOn_ = 0;
    if (synth_) {
        synth_->noteOff(440.0f);
    }
}

void NcursesynthMachine::setI(int index, int value) {
    params_[index] = value;
    if (!synth_) return;

    switch (index) {
        case MachineParam::POLYPHONY:
            synth_->setPolyphony(value);
            break;
        case MachineParam::FILTER_TYPE:
            synth_->setFilterType(static_cast<FilterType>(value));
            break;
        case MachineParam::RESONANCE:
            synth_->setResonance(value / 127.0f);
            break;
        case MachineParam::AMP_ATTACK:
            synth_->setAmpAttack(value / 127.0f);
            break;
        case MachineParam::AMP_DECAY:
            synth_->setAmpDecay(value / 127.0f);
            break;
        case MachineParam::AMP_SUSTAIN:
            synth_->setAmpSustain(value / 127.0f);
            break;
        case MachineParam::AMP_RELEASE:
            synth_->setAmpRelease(value / 127.0f);
            break;
        case MachineParam::FILTER_ENV_ATTACK:
            synth_->setFilterAttack(value / 127.0f);
            break;
        case MachineParam::FILTER_ENV_DECAY:
            synth_->setFilterDecay(value / 127.0f);
            break;
        case MachineParam::FILTER_ENV_SUSTAIN:
            synth_->setFilterSustain(value / 127.0f);
            break;
        case MachineParam::FILTER_ENV_RELEASE:
            synth_->setFilterRelease(value / 127.0f);
            break;
    }
}

void NcursesynthMachine::setF(int index, float value) {
    if (!synth_) return;

    switch (index) {
        case MachineParam::FILTER_CUTOFF:
            synth_->setCutoff(20.0f + value * 7980.0f);
            break;
        case MachineParam::FILTER_ENV_DEPTH:
            synth_->setFilterEnvelopeAmount(value);
            break;
        case MachineParam::OSC_MIX:
            synth_->setOscMix(value);
            break;
        case MachineParam::OSC_2_TUNE:
            synth_->setOsc2Detune(value);
            break;
    }
}

int NcursesynthMachine::getI(int index) {
    if (!synth_) return params_[index];

    switch (index) {
        case MachineParam::POLYPHONY:
            return synth_->getPolyphony();
        case MachineParam::FILTER_TYPE:
            return static_cast<int>(synth_->getCurrentFilterType());
        case MachineParam::RESONANCE:
            return static_cast<int>(synth_->getResonance() * 127.0f);
        case MachineParam::AMP_ATTACK:
            return static_cast<int>(synth_->getAmpAttack() * 127.0f);
        case MachineParam::AMP_DECAY:
            return static_cast<int>(synth_->getAmpDecay() * 127.0f);
        case MachineParam::AMP_SUSTAIN:
            return static_cast<int>(synth_->getAmpSustain() * 127.0f);
        case MachineParam::AMP_RELEASE:
            return static_cast<int>(synth_->getAmpRelease() * 127.0f);
        default:
            return params_[index];
    }
}

float NcursesynthMachine::getF(int index) {
    if (!synth_) return 0.0f;

    switch (index) {
        case MachineParam::FILTER_CUTOFF:
            return (synth_->getCutoff() - 20.0f) / 7980.0f;
        case MachineParam::FILTER_ENV_DEPTH:
            return synth_->getFilterEnvelopeAmount();
        case MachineParam::OSC_MIX:
            return synth_->getOscMix();
        case MachineParam::OSC_2_TUNE:
            return synth_->getOsc2Detune();
        default:
            return 0.0f;
    }
}

int NcursesynthMachine::checkI(int index, int value) {
    switch (index) {
        case MachineParam::POLYPHONY:
            return std::max(1, std::min(16, value));
        case MachineParam::FILTER_TYPE:
            return std::max(0, std::min(7, value));
        default:
            return std::max(0, std::min(127, value));
    }
}

const char* NcursesynthMachine::getDisplayString(int index) {
    if (!synth_) return "";

    switch (index) {
        case MachineParam::FILTER_TYPE: {
            static const char* names[] = {"MOOG", "KORG MS20", "OBERHEIM", "MOOG HPF",
                               "SVF LP12", "SVF HP12", "SVF BP12", "SVF AP12"};
            int t = static_cast<int>(synth_->getCurrentFilterType());
            return names[t];
        }
        case MachineParam::OSC_1_WAVEFORM:
        case MachineParam::OSC_2_WAVEFORM: {
            static const char* names[] = {"SAW", "SQUARE", "TRIANGLE"};
            int w = static_cast<int>(synth_->getWaveform());
            return names[w];
        }
        default:
            return "";
    }
}