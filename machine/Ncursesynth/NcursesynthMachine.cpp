#include "NcursesynthMachine.h"
#include "../ParamID.h"
#include <cmath>

static constexpr int OUTPUT_AMPLITUDE = 8192;
static constexpr float CUTOFF_MIN = 20.0f;
static constexpr float CUTOFF_MAX = 18000.0f;
static constexpr float CUTOFF_OCTAVES = 9.813781f; // log2(18000/20)

static int mapParam(int id) {
    switch (id) {
        case 0:   return 80;  // unified amp_attack → MachineParam::AMP_ATTACK
        case 1:   return 81;
        case 2:   return 82;
        case 3:   return 83;
        case 4:   return 90;  // unified flt_attack → MachineParam::FILTER_ENV_ATTACK
        case 5:   return 91;
        case 6:   return 92;
        case 7:   return 93;
        case 20:  return 0;   // unified osc1_wave → MachineParam::OSC_1_WAVEFORM
        case 21:  return 4;   // unified osc2_wave → MachineParam::OSC_2_WAVEFORM
        case 28:  return 9;   // unified mix → MachineParam::OSC_MIX
        case 41:  return 22;  // unified lfo1_depth → MachineParam::LFO_1_AMOUNT
        case 42:  return 21;  // unified lfo1_freq → MachineParam::LFO_1_FREQUENCY
        case 52:  return 51;  // unified cutoff → MachineParam::FILTER_CUTOFF
        case 53:  return 52;  // unified resonance → MachineParam::RESONANCE
        case 75:  return 11;  // unified osc2_detune → MachineParam::OSC_2_TUNE
        case 94:  return 54;  // unified flt_env_depth → MachineParam::FILTER_ENV_DEPTH
        default:  return id;  // pass through (35, 50, 65, 66, 70, 75, etc.)
    }
}

NcursesynthMachine::NcursesynthMachine() : synth_(nullptr), noteOn_(0), noteFrequency_(440.0f), midiNote_(69) {
    setName("Ncursesynth");
    synth_ = new SynthArchitecture(8, 48000.0f);
    for (int i = 0; i < MachineParam::PERFORMANCE_COUNT; i++) {
        params_[i] = 0;
    }
}

NcursesynthMachine::~NcursesynthMachine() {
    delete synth_;
}

void NcursesynthMachine::init() {
    if (synth_) {
        synth_->allNotesOff();
        synth_->reset();
        synth_->setVolume(0.8f);
        synth_->setOsc1Amp(1.0f);
        synth_->setOsc2Amp(0.0f);
        synth_->getDelay()->setEnabled(false);
        synth_->getDelay()->setMix(0.0f);
        synth_->getReverb()->setEnabled(false);
        synth_->getReverb()->setMix(0.0f);
        synth_->getChorus()->setEnabled(false);
        synth_->getChorus()->setMix(0.0f);
        synth_->getDistortion()->setEnabled(false);
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
    if (index == 74) {
        float t = (value - 64) / 64.0f;
        synth_->setOsc1Detune(t * fabsf(t) * 1.0f);
        return;
    }

    if (index == 65 || index == 66) {
        int idx = (value > 4) ? std::min(4, (value * 4 + 63) / 127) : value;
        idx = std::max(0, std::min(4, idx));
        int semitones = (idx - 2) * 12;
        if (index == 65) synth_->setOsc1Scale((float)semitones);
        else synth_->setOsc2Scale((float)semitones);
        return;
    }

    if (index == 31 || index == 32) {
        float amp = value / 127.0f;
        if (index == 31) synth_->setOsc1Amp(amp);
        else synth_->setOsc2Amp(amp);
        return;
    }

    index = mapParam(index);
    if (index >= 0 && index < MachineParam::PERFORMANCE_COUNT)
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
        case MachineParam::FILTER_TYPE: {
            int ft = std::max(0, std::min(14, value));
            synth_->setFilterType(static_cast<FilterType>(ft));
            break;
        }
        case MachineParam::FILTER_CUTOFF:
            synth_->setCutoff(CUTOFF_MIN * powf(2.0f, CUTOFF_OCTAVES * value / 127.0f));
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
        case MachineParam::OSC_2_TUNE: {
            float t = (value - 64) / 64.0f;
            synth_->setOsc2Detune(t * fabsf(t) * 1.0f);
            break;
        }
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
        case MachineParam::FILTER_ENV_DEPTH:
            synth_->setFilterEnvelopeAmount(value / 127.0f);
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
            synth_->setCutoff(CUTOFF_MIN * powf(2.0f, CUTOFF_OCTAVES * value));
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
        setI(52, val);  // Unified param ID for cutoff
    } else if (paramName == "RESONANCE") {
        setI(53, val);  // Unified param ID for resonance
    } else if (paramName == "FILTER_ENV_AMOUNT") {
        setI(94, val);  // Unified param ID for filter env depth
    } else if (paramName == "FILTER_ATTACK") {
        setI(4, val);   // Unified param ID for filter env attack
    } else if (paramName == "FILTER_DECAY") {
        setI(5, val);   // Unified param ID for filter env decay
    } else if (paramName == "FILTER_SUSTAIN") {
        setI(6, val);   // Unified param ID for filter env sustain
    } else if (paramName == "FILTER_RELEASE") {
        setI(7, val);   // Unified param ID for filter env release
    } else if (paramName == "AMP_ATTACK") {
        setI(0, val);   // Unified param ID for amp attack
    } else if (paramName == "AMP_DECAY") {
        setI(1, val);   // Unified param ID for amp decay
    } else if (paramName == "AMP_SUSTAIN") {
        setI(2, val);   // Unified param ID for amp sustain
    } else if (paramName == "AMP_RELEASE") {
        setI(3, val);   // Unified param ID for amp release
    } else if (paramName == "OSC1_DETUNE") {
        setI(74, val);
    } else if (paramName == "OSC2_DETUNE") {
        setI(75, val);
    } else if (paramName == "OSC1_SCALE") {
        setI(65, val);
    } else if (paramName == "OSC2_SCALE") {
        setI(66, val);
    } else if (paramName == "OSC1_AMP") {
        setI(31, val);
    } else if (paramName == "OSC2_AMP") {
        setI(32, val);
    } else if (paramName == "VOLUME") {
        setI(35, val);
    }
}

int NcursesynthMachine::getI(int index) {
    if (index == 74) {
        float detune = synth_->getOsc1Detune();
        float sign = (detune >= 0) ? 1.0f : -1.0f;
        float normalized = sqrtf(fabsf(detune) / 1.0f);
        return static_cast<int>(normalized * sign * 64.0f + 64.0f + 0.5f);
    }

    if (index == 65) {
        return static_cast<int>((synth_->getOsc1Scale() / 12.0f) + 2.0f);
    }
    if (index == 66) {
        return static_cast<int>((synth_->getOsc2Scale() / 12.0f) + 2.0f);
    }

    if (index == 31) {
        return static_cast<int>(synth_->getOsc1Amp() * 127.0f + 0.5f);
    }
    if (index == 32) {
        return static_cast<int>(synth_->getOsc2Amp() * 127.0f + 0.5f);
    }

    index = mapParam(index);
    if (!synth_) {
        if (index >= 0 && index < MachineParam::PERFORMANCE_COUNT)
            return params_[index];
        return 0;
    }

    switch (index) {
        case MachineParam::POLYPHONY:
            return synth_->getPolyphony();
        case MachineParam::FILTER_TYPE:
            return static_cast<int>(synth_->getCurrentFilterType());
        default:
            if (index >= 0 && index < MachineParam::PERFORMANCE_COUNT)
                return params_[index];
            return 0;
    }
}

float NcursesynthMachine::getF(int index) {
    if (!synth_) return 0.0f;

    switch (index) {
        case MachineParam::FILTER_CUTOFF:
            return log2f(synth_->getCutoff() / CUTOFF_MIN) / CUTOFF_OCTAVES;
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
            return std::max(0, std::min(14, value));
        default:
            return std::max(0, std::min(127, value));
    }
}

std::vector<std::pair<std::string, int>> NcursesynthMachine::getPresetParams() const {
    return {
        {"OSC1_WAVEFORM", MachineParam::OSC_1_WAVEFORM},
        {"OSC2_WAVEFORM", MachineParam::OSC_2_WAVEFORM},
        {"OSC_MIX", MachineParam::OSC_MIX},
        {"OSC2_TUNE", MachineParam::OSC_2_TUNE},
        {"FILTER_TYPE", MachineParam::FILTER_TYPE},
        {"CUTOFF", MachineParam::FILTER_CUTOFF},
        {"RESONANCE", MachineParam::RESONANCE},
        {"FILTER_ENV_DEPTH", MachineParam::FILTER_ENV_DEPTH},
        {"AMP_ATTACK", MachineParam::AMP_ATTACK},
        {"AMP_DECAY", MachineParam::AMP_DECAY},
        {"AMP_SUSTAIN", MachineParam::AMP_SUSTAIN},
        {"AMP_RELEASE", MachineParam::AMP_RELEASE},
        {"FILTER_ENV_ATTACK", MachineParam::FILTER_ENV_ATTACK},
        {"FILTER_ENV_DECAY", MachineParam::FILTER_ENV_DECAY},
        {"FILTER_ENV_SUSTAIN", MachineParam::FILTER_ENV_SUSTAIN},
        {"FILTER_ENV_RELEASE", MachineParam::FILTER_ENV_RELEASE},
        {"VOLUME", 35},
        {"LFO1_FREQ", MachineParam::LFO_1_FREQUENCY},
        {"LFO1_AMOUNT", MachineParam::LFO_1_AMOUNT}
    };
}

const char* NcursesynthMachine::getDisplayString(int index) {
    if (index == 65) {
        static const char* names[] = {"-2", "-1", "0", "+1", "+2"};
        int idx = static_cast<int>((synth_->getOsc1Scale() / 12.0f) + 2.0f);
        return names[idx];
    }
    if (index == 66) {
        static const char* names[] = {"-2", "-1", "0", "+1", "+2"};
        int idx = static_cast<int>((synth_->getOsc2Scale() / 12.0f) + 2.0f);
        return names[idx];
    }

    index = mapParam(index);
    if (!synth_) return "";

    switch (index) {
        case MachineParam::FILTER_TYPE: {
            static const char* names[] = {"MOOG", "KORG MS20", "OBERHEIM", "MOOG HPF",
                               "SVF LP12", "SVF HP12", "SVF BP12", "SVF AP12",
                               "DIODE", "FORMANT", "COMB",
                               "V-SALEN KY", "V-DGT SVF", "V-LADDER", "V-DIRTY"};
            int t = static_cast<int>(synth_->getCurrentFilterType());
            if (t < 0) t = 0;
            if (t > 14) t = 14;
            return names[t];
        }
        case MachineParam::OSC_1_WAVEFORM: {
            static const char* names[] = {"SAW", "SQUARE", "TRIANGLE"};
            int w = static_cast<int>(synth_->getOsc1Waveform());
            if (w < 0) w = 0;
            if (w > 2) w = 2;
            return names[w];
        }
        case MachineParam::OSC_2_WAVEFORM: {
            static const char* names[] = {"SAW", "SQUARE", "TRIANGLE"};
            int w = static_cast<int>(synth_->getOsc2Waveform());
            if (w < 0) w = 0;
            if (w > 2) w = 2;
            return names[w];
        }
        default:
            return "";
    }
}