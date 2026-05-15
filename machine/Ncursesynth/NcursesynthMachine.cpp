#include "NcursesynthMachine.h"
#include <cmath>

static constexpr int OUTPUT_AMPLITUDE = 8192;

NcursesynthMachine::NcursesynthMachine() : synth_(nullptr), noteOn_(0), noteFrequency_(440.0f), midiNote_(69) {
    setName("Ncursesynth");
    synth_ = new SynthArchitecture(8, 48000.0f);
    for (int i = 0; i < MachineParam::PERFORMANCE_COUNT; i++) {
        params_[i] = 0;
    }
}

NcursesynthMachine::~NcursesynthMachine() {}

void NcursesynthMachine::init() {
    if (synth_) {
        synth_->allNotesOff();
        synth_->reset();
    }
    noteOn_ = 0;
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
        return static_cast<int32_t>(sample * OUTPUT_AMPLITUDE);
    }
    return 0;
}

void NcursesynthMachine::noteOn() {
    noteOn_ = 1;
    if (synth_) {
        synth_->noteOn(noteFrequency_);
    }
}

void NcursesynthMachine::noteOff() {
    noteOn_ = 0;
    if (synth_) {
        synth_->noteOff(noteFrequency_);
    }
}

void NcursesynthMachine::setI(int index, int value) {
    params_[index] = value;
    
    if (index == MachineParam::NOTE_HZ || index == 70) {
        midiNote_ = value;
        noteFrequency_ = 440.0f * std::pow(2.0f, (value - 69.0f) / 12.0f);
        return;
    }
    
    if (index == 150) {
        if (value == 1) noteOn();
        else noteOff();
        return;
    }
    
    if (!synth_) return;

    switch (index) {
        case MachineParam::POLYPHONY:
            synth_->setPolyphony(value);
            break;
        case MachineParam::FILTER_TYPE:
            synth_->setFilterType(static_cast<FilterType>(value));
            break;
        case MachineParam::FILTER_CUTOFF:
            synth_->setCutoff(20.0f + (value / 127.0f) * 7980.0f);
            break;
        case MachineParam::RESONANCE:
            synth_->setResonance(value / 127.0f);
            break;
        case MachineParam::OSC_1_WAVEFORM:
            if (value < 0) value = 0;
            if (value > 2) value = 2;
            synth_->setOsc1Waveform(static_cast<Waveform>(value));
            break;
        case MachineParam::OSC_2_WAVEFORM:
            if (value < 0) value = 0;
            if (value > 2) value = 2;
            synth_->setOsc2Waveform(static_cast<Waveform>(value));
            break;
        case MachineParam::OSC_MIX:
            synth_->setOscMix(value / 127.0f);
            break;
        case MachineParam::OSC_2_TUNE:
            synth_->setOsc2Detune(value / 127.0f);
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
        case MachineParam::LFO_1_FREQUENCY:
            break;
        case MachineParam::LFO_1_AMOUNT:
            break;
        case 35: // AMP / VOLUME
            synth_->setVolume(value / 127.0f);
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

void NcursesynthMachine::applyCC(int cc, float normalized, const std::string& paramName) {
    int val = (int)(normalized * 127.0f);

    if (paramName == "CUTOFF") {
        setF(MachineParam::FILTER_CUTOFF, val);
    } else if (paramName == "RESONANCE") {
        setI(MachineParam::RESONANCE, val);
    } else if (paramName == "FILTER_ENV_AMOUNT") {
        setF(MachineParam::FILTER_ENV_DEPTH, val);
    } else if (paramName == "FILTER_ATTACK") {
        setI(MachineParam::FILTER_ENV_ATTACK, val);
    } else if (paramName == "FILTER_DECAY") {
        setI(MachineParam::FILTER_ENV_DECAY, val);
    } else if (paramName == "FILTER_SUSTAIN") {
        setI(MachineParam::FILTER_ENV_SUSTAIN, val);
    } else if (paramName == "FILTER_RELEASE") {
        setI(MachineParam::FILTER_ENV_RELEASE, val);
    } else if (paramName == "AMP_ATTACK") {
        setI(MachineParam::AMP_ATTACK, val);
    } else if (paramName == "AMP_DECAY") {
        setI(MachineParam::AMP_DECAY, val);
    } else if (paramName == "AMP_SUSTAIN") {
        setI(MachineParam::AMP_SUSTAIN, val);
    } else if (paramName == "AMP_RELEASE") {
        setI(MachineParam::AMP_RELEASE, val);
    } else if (paramName == "VOLUME") {
        setI(35, val);
    }
}

int NcursesynthMachine::getI(int index) {
    if (!synth_) return params_[index];

    switch (index) {
        case MachineParam::POLYPHONY:
            return synth_->getPolyphony();
        case MachineParam::FILTER_TYPE:
            return static_cast<int>(synth_->getCurrentFilterType());
        case MachineParam::FILTER_CUTOFF:
            return static_cast<int>(((synth_->getCutoff() - 20.0f) / 7980.0f) * 127.0f);
        case MachineParam::RESONANCE:
            return static_cast<int>(synth_->getResonance() * 127.0f);
        case MachineParam::OSC_1_WAVEFORM:
            return static_cast<int>(synth_->getOsc1Waveform());
        case MachineParam::OSC_2_WAVEFORM:
            return static_cast<int>(synth_->getOsc2Waveform());
        case MachineParam::OSC_MIX:
            return static_cast<int>(synth_->getOscMix() * 127.0f);
        case MachineParam::OSC_2_TUNE:
            return static_cast<int>(synth_->getOsc2Detune() * 127.0f);
        case MachineParam::AMP_ATTACK:
            return static_cast<int>(synth_->getAmpAttack() * 127.0f);
        case MachineParam::AMP_DECAY:
            return static_cast<int>(synth_->getAmpDecay() * 127.0f);
        case MachineParam::AMP_SUSTAIN:
            return static_cast<int>(synth_->getAmpSustain() * 127.0f);
        case MachineParam::AMP_RELEASE:
            return static_cast<int>(synth_->getAmpRelease() * 127.0f);
        case MachineParam::FILTER_ENV_ATTACK:
            return static_cast<int>(synth_->getFilterAttack() * 127.0f);
        case MachineParam::FILTER_ENV_DECAY:
            return static_cast<int>(synth_->getFilterDecay() * 127.0f);
        case MachineParam::FILTER_ENV_SUSTAIN:
            return static_cast<int>(synth_->getFilterSustain() * 127.0f);
        case MachineParam::FILTER_ENV_RELEASE:
            return static_cast<int>(synth_->getFilterRelease() * 127.0f);
        case 35: // VOLUME
            return static_cast<int>(synth_->getVolume() * 127.0f);
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
        case MachineParam::OSC_1_WAVEFORM: {
            static const char* names[] = {"SAW", "SQUARE", "TRIANGLE"};
            int w = static_cast<int>(synth_->getOsc1Waveform());
            return names[w];
        }
        case MachineParam::OSC_2_WAVEFORM: {
            static const char* names[] = {"SAW", "SQUARE", "TRIANGLE"};
            int w = static_cast<int>(synth_->getOsc2Waveform());
            return names[w];
        }
        default:
            return "";
    }
}