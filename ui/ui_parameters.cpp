#include "ui_parameters.h"
#include "ui_constants.h"
#include "../midi/midi_input.h"
#include "../midi/midi_mapping.h"
#include "../preset/preset_manager.h"

const char* UIParameters::getParameterName(Parameter param) {
    switch (param) {
        case Parameter::POLYPHONY: return "POLYPHONY";
        case Parameter::FILTER_TYPE: return "FILTER TYPE";
        case Parameter::CUTOFF: return "CUTOFF";
        case Parameter::RESONANCE: return "RESONANCE";
        case Parameter::FILTER_ENV_AMOUNT: return "FILTER ENV AMOUNT";
        case Parameter::HPF_FREQ: return "HPF FREQ";
        case Parameter::AMP_ATTACK: return "AMP ATTACK";
        case Parameter::AMP_DECAY: return "AMP DECAY";
        case Parameter::AMP_SUSTAIN: return "AMP SUSTAIN";
        case Parameter::AMP_RELEASE: return "AMP RELEASE";
        case Parameter::AMP_ENV_CURVE: return "AMP ENV CURVE";
        case Parameter::FILTER_ATTACK: return "FILTER ATTACK";
        case Parameter::FILTER_DECAY: return "FILTER DECAY";
        case Parameter::FILTER_SUSTAIN: return "FILTER SUSTAIN";
        case Parameter::FILTER_RELEASE: return "FILTER RELEASE";
        case Parameter::FILTER_ENV_CURVE: return "FILTER ENV CURVE";
        case Parameter::WAVEFORM: return "WAVEFORM";
        case Parameter::OSC_MIX: return "OSC MIX";
        case Parameter::OSC2_DETUNE: return "OSC2 DETUNE";
        case Parameter::PRESET_LOAD: return "LOAD PRESET";
        case Parameter::PRESET_SAVE: return "SAVE PRESET";
        case Parameter::DELAY_ENABLE: return "DELAY ENABLE";
        case Parameter::DELAY_TIME: return "DELAY TIME";
        case Parameter::DELAY_FEEDBACK: return "DELAY FEEDBACK";
        case Parameter::DELAY_MIX: return "DELAY MIX";
        case Parameter::REVERB_ENABLE: return "REVERB ENABLE";
        case Parameter::REVERB_DECAY: return "REVERB DECAY";
        case Parameter::REVERB_MIX: return "REVERB MIX";
        case Parameter::CHORUS_ENABLE: return "CHORUS ENABLE";
        case Parameter::CHORUS_DEPTH: return "CHORUS DEPTH";
        case Parameter::CHORUS_RATE: return "CHORUS RATE";
        case Parameter::CHORUS_MIX: return "CHORUS MIX";
        case Parameter::DISTORTION_ENABLE: return "DISTORTION ENABLE";
        case Parameter::DISTORTION_DRIVE: return "DISTORTION DRIVE";
        case Parameter::DISTORTION_MIX: return "DISTORTION MIX";
        case Parameter::VOLUME: return "VOLUME";
        case Parameter::MIDI_ENABLE: return "MIDI ENABLE";
        case Parameter::MIDI_DEVICE: return "MIDI DEVICE";
        case Parameter::MIDI_MAPPING: return "MIDI MAPPING";
        default: return "UNKNOWN";
    }
}

const char* UIParameters::getCurveName(EnvelopeCurve curve) {
    switch (curve) {
        case EnvelopeCurve::LINEAR: return "LINEAR";
        case EnvelopeCurve::EXPONENTIAL: return "EXPONENTIAL";
        case EnvelopeCurve::LOGARITHMIC: return "LOGARITHMIC";
        case EnvelopeCurve::SINE: return "SINE";
        case EnvelopeCurve::TENT: return "TENT";
        case EnvelopeCurve::REVERSE_TENT: return "REVERSE TENT";
        default: return "UNKNOWN";
    }
}

const char* UIParameters::getFilterTypeName(SynthArchitecture* synth) {
    switch (synth->getCurrentFilterType()) {
        case FilterType::MOOG: return "MOOG";
        case FilterType::KORG_MS20: return "KORG MS-20";
        case FilterType::OBERHEIM_SEM: return "OBERHEIM SEM";
        default: return "UNKNOWN";
    }
}

const char* UIParameters::getWaveformName(SynthArchitecture* synth) {
    switch (synth->getWaveform()) {
        case Waveform::SAWTOOTH: return "SAWTOOTH";
        case Waveform::SQUARE: return "SQUARE";
        case Waveform::TRIANGLE: return "TRIANGLE";
        default: return "UNKNOWN";
    }
}

const char* UIParameters::getBoolText(bool value) {
    return value ? "ON " : "OFF";
}

void UIParameters::increaseParameter(SynthArchitecture* synth, Parameter param) {
    switch (param) {
        case Parameter::POLYPHONY: {
            int newPoly = synth->getPolyphony() + 1;
            if (newPoly <= 16) synth->setPolyphony(newPoly);
            break;
        }
        case Parameter::FILTER_TYPE: {
            int type = static_cast<int>(synth->getCurrentFilterType());
            type = (type + 1) % 3;
            synth->setFilterType(static_cast<FilterType>(type));
            break;
        }
        case Parameter::CUTOFF: {
            float newVal = synth->getCutoff() + 50.0f;
            if (newVal > 8000.0f) newVal = 8000.0f;
            synth->setCutoff(newVal);
            break;
        }
        case Parameter::RESONANCE: {
            float newVal = synth->getResonance() + 0.02f;
            if (newVal > 1.0f) newVal = 1.0f;
            synth->setResonance(newVal);
            break;
        }
        case Parameter::FILTER_ENV_AMOUNT: {
            float newVal = synth->getFilterEnvelopeAmount() + 0.02f;
            if (newVal > 1.0f) newVal = 1.0f;
            synth->setFilterEnvelopeAmount(newVal);
            break;
        }
        case Parameter::HPF_FREQ: {
            float newVal = synth->getHPFCutoff() + 50.0f;
            if (newVal > 5000.0f) newVal = 5000.0f;
            synth->setHPFCutoff(newVal);
            break;
        }
        case Parameter::AMP_ATTACK: {
            float newVal = synth->getAmpAttack() + 0.02f;
            if (newVal > 1.0f) newVal = 1.0f;
            synth->setAmpAttack(newVal);
            break;
        }
        case Parameter::AMP_DECAY: {
            float newVal = synth->getAmpDecay() + 0.02f;
            if (newVal > 1.0f) newVal = 1.0f;
            synth->setAmpDecay(newVal);
            break;
        }
        case Parameter::AMP_SUSTAIN: {
            float newVal = synth->getAmpSustain() + 0.02f;
            if (newVal > 1.0f) newVal = 1.0f;
            synth->setAmpSustain(newVal);
            break;
        }
        case Parameter::AMP_RELEASE: {
            float newVal = synth->getAmpRelease() + 0.02f;
            if (newVal > 1.0f) newVal = 1.0f;
            synth->setAmpRelease(newVal);
            break;
        }
        case Parameter::AMP_ENV_CURVE: {
            int curve = static_cast<int>(synth->getAmpEnvelopeCurve());
            curve = (curve + 1) % 6;
            synth->setAmpEnvelopeCurve(static_cast<EnvelopeCurve>(curve));
            break;
        }
        case Parameter::FILTER_ATTACK: {
            float newVal = synth->getFilterAttack() + 0.02f;
            if (newVal > 1.0f) newVal = 1.0f;
            synth->setFilterAttack(newVal);
            break;
        }
        case Parameter::FILTER_DECAY: {
            float newVal = synth->getFilterDecay() + 0.02f;
            if (newVal > 1.0f) newVal = 1.0f;
            synth->setFilterDecay(newVal);
            break;
        }
        case Parameter::FILTER_SUSTAIN: {
            float newVal = synth->getFilterSustain() + 0.02f;
            if (newVal > 1.0f) newVal = 1.0f;
            synth->setFilterSustain(newVal);
            break;
        }
        case Parameter::FILTER_RELEASE: {
            float newVal = synth->getFilterRelease() + 0.02f;
            if (newVal > 1.0f) newVal = 1.0f;
            synth->setFilterRelease(newVal);
            break;
        }
        case Parameter::FILTER_ENV_CURVE: {
            int curve = static_cast<int>(synth->getFilterEnvelopeCurve());
            curve = (curve + 1) % 6;
            synth->setFilterEnvelopeCurve(static_cast<EnvelopeCurve>(curve));
            break;
        }
        case Parameter::WAVEFORM: {
            int wav = static_cast<int>(synth->getWaveform());
            wav = (wav + 1) % 3;
            synth->setWaveform(static_cast<Waveform>(wav));
            break;
        }
        case Parameter::OSC_MIX: {
            float newVal = synth->getOscMix() + 0.05f;
            if (newVal > 1.0f) newVal = 1.0f;
            synth->setOscMix(newVal);
            break;
        }
        case Parameter::OSC2_DETUNE: {
            float newVal = synth->getOsc2Detune() + 0.1f;
            if (newVal > 1.0f) newVal = 1.0f;
            synth->setOsc2Detune(newVal);
            break;
        }
        case Parameter::PRESET_LOAD: {
            PresetManager* pm = synth->getPresetManager();
            if (pm && pm->exists()) {
                int currentIdx = pm->getCurrentPresetIndex();
                int newIdx = (currentIdx + 1) % pm->getPresetCount();
                pm->loadPreset(newIdx, synth);
            }
            break;
        }
        case Parameter::PRESET_SAVE: {
            PresetManager* pm = synth->getPresetManager();
            if (pm && pm->exists()) {
                pm->savePreset(pm->getCurrentPresetIndex(), synth);
            }
            break;
        }
        case Parameter::MIDI_DEVICE: {
            MidiInput* midi = synth->getMidiInput();
            if (midi) {
                int currentDevice = midi->getSelectedPort();
                int newDevice = currentDevice + 1;
                if (newDevice >= midi->getDeviceCount()) {
                    newDevice = -1;
                }
                if (newDevice >= 0) {
                    midi->selectDevice(newDevice);
                    midi->start();
                } else {
                    midi->stop();
                }
            }
            break;
        }
        case Parameter::MIDI_MAPPING: {
            MidiInput* midi = synth->getMidiInput();
            if (midi) {
                MappingManager* mm = midi->getMappingManager();
                if (mm && mm->getMappingCount() > 0) {
                    int currentIdx = mm->getCurrentMappingIndex();
                    int newIdx = (currentIdx + 1) % mm->getMappingCount();
                    mm->setCurrentMapping(newIdx);
                }
            }
            break;
        }
        case Parameter::MIDI_ENABLE: {
            MidiInput* midi = synth->getMidiInput();
            if (midi) {
                if (midi->isRunning()) {
                    midi->stop();
                } else if (midi->getSelectedPort() >= 0) {
                    midi->start();
                }
            }
            break;
        }
        case Parameter::DELAY_ENABLE: {
            synth->getDelay()->setEnabled(!synth->getDelay()->isEnabled());
            break;
        }
        case Parameter::DELAY_TIME: {
            float newVal = synth->getDelay()->getDelayTime() + 10.0f;
            if (newVal > 1000.0f) newVal = 1000.0f;
            synth->getDelay()->setDelayTime(newVal);
            break;
        }
        case Parameter::DELAY_FEEDBACK: {
            float newVal = synth->getDelay()->getFeedback() + 0.02f;
            if (newVal > 0.95f) newVal = 0.95f;
            synth->getDelay()->setFeedback(newVal);
            break;
        }
        case Parameter::DELAY_MIX: {
            float newVal = synth->getDelay()->getMix() + 0.02f;
            if (newVal > 1.0f) newVal = 1.0f;
            synth->getDelay()->setMix(newVal);
            break;
        }
        case Parameter::REVERB_ENABLE: {
            synth->getReverb()->setEnabled(!synth->getReverb()->isEnabled());
            break;
        }
        case Parameter::REVERB_DECAY: {
            float newVal = synth->getReverb()->getDecay() + 0.02f;
            if (newVal > 0.98f) newVal = 0.98f;
            synth->getReverb()->setDecay(newVal);
            break;
        }
        case Parameter::REVERB_MIX: {
            float newVal = synth->getReverb()->getMix() + 0.02f;
            if (newVal > 1.0f) newVal = 1.0f;
            synth->getReverb()->setMix(newVal);
            break;
        }
        case Parameter::CHORUS_ENABLE: {
            synth->getChorus()->setEnabled(!synth->getChorus()->isEnabled());
            break;
        }
        case Parameter::CHORUS_DEPTH: {
            float newVal = synth->getChorus()->getDepth() + 0.02f;
            if (newVal > 1.0f) newVal = 1.0f;
            synth->getChorus()->setDepth(newVal);
            break;
        }
        case Parameter::CHORUS_RATE: {
            float newVal = synth->getChorus()->getRate() + 0.1f;
            if (newVal > 5.0f) newVal = 5.0f;
            synth->getChorus()->setRate(newVal);
            break;
        }
        case Parameter::CHORUS_MIX: {
            float newVal = synth->getChorus()->getMix() + 0.02f;
            if (newVal > 1.0f) newVal = 1.0f;
            synth->getChorus()->setMix(newVal);
            break;
        }
        case Parameter::DISTORTION_ENABLE: {
            synth->getDistortion()->setEnabled(!synth->getDistortion()->isEnabled());
            break;
        }
        case Parameter::DISTORTION_DRIVE: {
            float newVal = synth->getDistortion()->getDrive() + 0.02f;
            if (newVal > 1.0f) newVal = 1.0f;
            synth->getDistortion()->setDrive(newVal);
            break;
        }
        case Parameter::DISTORTION_MIX: {
            float newVal = synth->getDistortion()->getMix() + 0.02f;
            if (newVal > 1.0f) newVal = 1.0f;
            synth->getDistortion()->setMix(newVal);
            break;
        }
        case Parameter::VOLUME: {
            float newVal = synth->getVolume() + 0.02f;
            if (newVal > 1.0f) newVal = 1.0f;
            synth->setVolume(newVal);
            break;
        }
        default: break;
    }
}

void UIParameters::decreaseParameter(SynthArchitecture* synth, Parameter param) {
    switch (param) {
        case Parameter::POLYPHONY: {
            int newPoly = synth->getPolyphony() - 1;
            if (newPoly >= 1) synth->setPolyphony(newPoly);
            break;
        }
        case Parameter::FILTER_TYPE: {
            int type = static_cast<int>(synth->getCurrentFilterType());
            type = (type - 1 + 3) % 3;
            synth->setFilterType(static_cast<FilterType>(type));
            break;
        }
        case Parameter::CUTOFF: {
            float newVal = synth->getCutoff() - 50.0f;
            if (newVal < 20.0f) newVal = 20.0f;
            synth->setCutoff(newVal);
            break;
        }
        case Parameter::RESONANCE: {
            float newVal = synth->getResonance() - 0.02f;
            if (newVal < 0.0f) newVal = 0.0f;
            synth->setResonance(newVal);
            break;
        }
        case Parameter::FILTER_ENV_AMOUNT: {
            float newVal = synth->getFilterEnvelopeAmount() - 0.02f;
            if (newVal < 0.0f) newVal = 0.0f;
            synth->setFilterEnvelopeAmount(newVal);
            break;
        }
        case Parameter::HPF_FREQ: {
            float newVal = synth->getHPFCutoff() - 50.0f;
            if (newVal < 20.0f) newVal = 20.0f;
            synth->setHPFCutoff(newVal);
            break;
        }
        case Parameter::AMP_ATTACK: {
            float newVal = synth->getAmpAttack() - 0.02f;
            if (newVal < 0.0f) newVal = 0.0f;
            synth->setAmpAttack(newVal);
            break;
        }
        case Parameter::AMP_DECAY: {
            float newVal = synth->getAmpDecay() - 0.02f;
            if (newVal < 0.0f) newVal = 0.0f;
            synth->setAmpDecay(newVal);
            break;
        }
        case Parameter::AMP_SUSTAIN: {
            float newVal = synth->getAmpSustain() - 0.02f;
            if (newVal < 0.0f) newVal = 0.0f;
            synth->setAmpSustain(newVal);
            break;
        }
        case Parameter::AMP_RELEASE: {
            float newVal = synth->getAmpRelease() - 0.02f;
            if (newVal < 0.0f) newVal = 0.0f;
            synth->setAmpRelease(newVal);
            break;
        }
        case Parameter::AMP_ENV_CURVE: {
            int curve = static_cast<int>(synth->getAmpEnvelopeCurve());
            curve = (curve - 1 + 6) % 6;
            synth->setAmpEnvelopeCurve(static_cast<EnvelopeCurve>(curve));
            break;
        }
        case Parameter::FILTER_ATTACK: {
            float newVal = synth->getFilterAttack() - 0.02f;
            if (newVal < 0.0f) newVal = 0.0f;
            synth->setFilterAttack(newVal);
            break;
        }
        case Parameter::FILTER_DECAY: {
            float newVal = synth->getFilterDecay() - 0.02f;
            if (newVal < 0.0f) newVal = 0.0f;
            synth->setFilterDecay(newVal);
            break;
        }
        case Parameter::FILTER_SUSTAIN: {
            float newVal = synth->getFilterSustain() - 0.02f;
            if (newVal < 0.0f) newVal = 0.0f;
            synth->setFilterSustain(newVal);
            break;
        }
        case Parameter::FILTER_RELEASE: {
            float newVal = synth->getFilterRelease() - 0.02f;
            if (newVal < 0.0f) newVal = 0.0f;
            synth->setFilterRelease(newVal);
            break;
        }
        case Parameter::FILTER_ENV_CURVE: {
            int curve = static_cast<int>(synth->getFilterEnvelopeCurve());
            curve = (curve - 1 + 6) % 6;
            synth->setFilterEnvelopeCurve(static_cast<EnvelopeCurve>(curve));
            break;
        }
        case Parameter::WAVEFORM: {
            int wav = static_cast<int>(synth->getWaveform());
            wav = (wav - 1 + 3) % 3;
            synth->setWaveform(static_cast<Waveform>(wav));
            break;
        }
        case Parameter::OSC_MIX: {
            float newVal = synth->getOscMix() - 0.05f;
            if (newVal < 0.0f) newVal = 0.0f;
            synth->setOscMix(newVal);
            break;
        }
        case Parameter::OSC2_DETUNE: {
            float newVal = synth->getOsc2Detune() - 0.1f;
            if (newVal < -1.0f) newVal = -1.0f;
            synth->setOsc2Detune(newVal);
            break;
        }
        case Parameter::PRESET_LOAD: {
            PresetManager* pm = synth->getPresetManager();
            if (pm && pm->exists()) {
                int currentIdx = pm->getCurrentPresetIndex();
                int newIdx = currentIdx - 1;
                if (newIdx < 0) newIdx = pm->getPresetCount() - 1;
                pm->loadPreset(newIdx, synth);
            }
            break;
        }
        case Parameter::PRESET_SAVE: {
            PresetManager* pm = synth->getPresetManager();
            if (pm && pm->exists()) {
                pm->savePreset(pm->getCurrentPresetIndex(), synth);
            }
            break;
        }
        case Parameter::MIDI_DEVICE: {
            MidiInput* midi = synth->getMidiInput();
            if (midi) {
                int currentDevice = midi->getSelectedPort();
                int newDevice = currentDevice - 1;
                if (newDevice < -1) newDevice = midi->getDeviceCount() - 1;
                if (newDevice >= 0) {
                    midi->selectDevice(newDevice);
                    midi->start();
                } else {
                    midi->stop();
                }
            }
            break;
        }
        case Parameter::MIDI_MAPPING: {
            MidiInput* midi = synth->getMidiInput();
            if (midi) {
                MappingManager* mm = midi->getMappingManager();
                if (mm && mm->getMappingCount() > 0) {
                    int currentIdx = mm->getCurrentMappingIndex();
                    int newIdx = currentIdx - 1;
                    if (newIdx < 0) newIdx = mm->getMappingCount() - 1;
                    mm->setCurrentMapping(newIdx);
                }
            }
            break;
        }
        case Parameter::MIDI_ENABLE: {
            MidiInput* midi = synth->getMidiInput();
            if (midi) {
                if (midi->isRunning()) {
                    midi->stop();
                } else if (midi->getSelectedPort() >= 0) {
                    midi->start();
                }
            }
            break;
        }
        case Parameter::DELAY_ENABLE: {
            synth->getDelay()->setEnabled(!synth->getDelay()->isEnabled());
            break;
        }
        case Parameter::DELAY_TIME: {
            float newVal = synth->getDelay()->getDelayTime() - 10.0f;
            if (newVal < 1.0f) newVal = 1.0f;
            synth->getDelay()->setDelayTime(newVal);
            break;
        }
        case Parameter::DELAY_FEEDBACK: {
            float newVal = synth->getDelay()->getFeedback() - 0.02f;
            if (newVal < 0.0f) newVal = 0.0f;
            synth->getDelay()->setFeedback(newVal);
            break;
        }
        case Parameter::DELAY_MIX: {
            float newVal = synth->getDelay()->getMix() - 0.02f;
            if (newVal < 0.0f) newVal = 0.0f;
            synth->getDelay()->setMix(newVal);
            break;
        }
        case Parameter::REVERB_ENABLE: {
            synth->getReverb()->setEnabled(!synth->getReverb()->isEnabled());
            break;
        }
        case Parameter::REVERB_DECAY: {
            float newVal = synth->getReverb()->getDecay() - 0.02f;
            if (newVal < 0.0f) newVal = 0.0f;
            synth->getReverb()->setDecay(newVal);
            break;
        }
        case Parameter::REVERB_MIX: {
            float newVal = synth->getReverb()->getMix() - 0.02f;
            if (newVal < 0.0f) newVal = 0.0f;
            synth->getReverb()->setMix(newVal);
            break;
        }
        case Parameter::CHORUS_ENABLE: {
            synth->getChorus()->setEnabled(!synth->getChorus()->isEnabled());
            break;
        }
        case Parameter::CHORUS_DEPTH: {
            float newVal = synth->getChorus()->getDepth() - 0.02f;
            if (newVal < 0.0f) newVal = 0.0f;
            synth->getChorus()->setDepth(newVal);
            break;
        }
        case Parameter::CHORUS_RATE: {
            float newVal = synth->getChorus()->getRate() - 0.1f;
            if (newVal < 0.1f) newVal = 0.1f;
            synth->getChorus()->setRate(newVal);
            break;
        }
        case Parameter::CHORUS_MIX: {
            float newVal = synth->getChorus()->getMix() - 0.02f;
            if (newVal < 0.0f) newVal = 0.0f;
            synth->getChorus()->setMix(newVal);
            break;
        }
        case Parameter::DISTORTION_ENABLE: {
            synth->getDistortion()->setEnabled(!synth->getDistortion()->isEnabled());
            break;
        }
        case Parameter::DISTORTION_DRIVE: {
            float newVal = synth->getDistortion()->getDrive() - 0.02f;
            if (newVal < 0.0f) newVal = 0.0f;
            synth->getDistortion()->setDrive(newVal);
            break;
        }
        case Parameter::DISTORTION_MIX: {
            float newVal = synth->getDistortion()->getMix() - 0.02f;
            if (newVal < 0.0f) newVal = 0.0f;
            synth->getDistortion()->setMix(newVal);
            break;
        }
        case Parameter::VOLUME: {
            float newVal = synth->getVolume() - 0.02f;
            if (newVal < 0.0f) newVal = 0.0f;
            synth->setVolume(newVal);
            break;
        }
        default: break;
    }
}

void UIParameters::loadNextPreset(SynthArchitecture* synth) {
    PresetManager* pm = synth->getPresetManager();
    if (pm && pm->exists()) {
        int currentIdx = pm->getCurrentPresetIndex();
        int newIdx = (currentIdx + 1) % pm->getPresetCount();
        pm->loadPreset(newIdx, synth);
    }
}

void UIParameters::loadPrevPreset(SynthArchitecture* synth) {
    PresetManager* pm = synth->getPresetManager();
    if (pm && pm->exists()) {
        int currentIdx = pm->getCurrentPresetIndex();
        int newIdx = currentIdx - 1;
        if (newIdx < 0) newIdx = pm->getPresetCount() - 1;
        pm->loadPreset(newIdx, synth);
    }
}
