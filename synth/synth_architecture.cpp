#include "synth_architecture.h"
#include <algorithm>

SynthArchitecture::SynthArchitecture(int polyphony, float rate) 
    : maxPolyphony(polyphony), nextNoteId(0), sampleRate(rate),
      currentFilterType(FilterType::MOOG),
      currentWaveform(Waveform::SAWTOOTH),
      cutoff(1000.0f),
      resonance(0.5f),
      filterEnvelopeAmount(0.5f),
      ampEnvelopeCurve(EnvelopeCurve::EXPONENTIAL),
      filterEnvelopeCurve(EnvelopeCurve::EXPONENTIAL),
      ampAttack(0.01f),
      ampDecay(0.2f),
      ampSustain(0.7f),
      ampRelease(0.3f),
      filtAttack(0.05f),
      filtDecay(0.3f),
      filtSustain(0.3f),
      filtRelease(0.5f),
      volume(0.3f) {
    
    // Create voices
    for (int i = 0; i < maxPolyphony; i++) {
        voices.push_back(std::unique_ptr<Voice>(new Voice(sampleRate)));
    }
    
    effectChain.setSampleRate(sampleRate);
}

SynthArchitecture::~SynthArchitecture() {}

void SynthArchitecture::setSampleRate(float rate) {
    sampleRate = rate;
    for (auto& voice : voices) {
        voice->setSampleRate(rate);
    }
    effectChain.setSampleRate(rate);
}

void SynthArchitecture::setPolyphony(int newPolyphony) {
    if (newPolyphony == maxPolyphony) return;
    
    // Stop all currently playing notes
    allNotesOff();
    
    // Clear existing voices
    voices.clear();
    
    // Create new voices with new polyphony
    maxPolyphony = std::max(1, std::min(16, newPolyphony));
    for (int i = 0; i < maxPolyphony; i++) {
        voices.push_back(std::unique_ptr<Voice>(new Voice(sampleRate)));
    }
}

int SynthArchitecture::findFreeVoice() {
    // First, try to find a completely inactive voice
    for (size_t i = 0; i < voices.size(); i++) {
        if (!voices[i]->isActive()) {
            return i;
        }
    }
    
    // If all voices are active, just use the first one for voice stealing
    // (Remove the unused minLevel variable)
    
    return 0; // Return first voice for stealing
}


void SynthArchitecture::updateAllVoices() {
    for (auto& voice : voices) {
        voice->updateFilterType(currentFilterType);
        voice->updateCutoff(cutoff);
        voice->updateResonance(resonance);
        voice->updateFilterEnvelopeAmount(filterEnvelopeAmount);
        voice->updateWaveform(currentWaveform);
        voice->updateEnvelopeCurves(ampEnvelopeCurve, filterEnvelopeCurve);
    }
}

void SynthArchitecture::noteOn(float frequency) {
    // First, clean up any voices that are finished (envelope complete)
    for (auto& voice : voices) {
        if (!voice->isActive()) {
            voice->reset();
        }
    }
    
    int voiceIndex = findFreeVoice();
    
    if (voiceIndex == -1) {
        // No free voice, find the oldest active voice
        voiceIndex = 0;
        // Force note off on that voice
        voices[voiceIndex]->noteOff();
        voices[voiceIndex]->reset();
    }
    
    voices[voiceIndex]->noteOn(frequency, nextNoteId++,
                               currentFilterType, cutoff, resonance, filterEnvelopeAmount,
                               ampAttack, ampDecay, ampSustain, ampRelease,
                               filtAttack, filtDecay, filtSustain, filtRelease,
                               ampEnvelopeCurve, filterEnvelopeCurve);
}

void SynthArchitecture::noteOff(float frequency) {
    // Find voice playing this frequency
    for (auto& voice : voices) {
        if (voice->isActive()) {
            voice->noteOff();
            break; // Found and released one voice
        }
    }
}

float SynthArchitecture::process() {
    float output = 0.0f;
    
    // Mix all active voices
    for (auto& voice : voices) {
        if (voice->isActive()) {
            output += voice->process();
        }
    }
    
    // Apply effects chain
    output = effectChain.process(output);
    
    // Apply master volume and clip
    output = output * volume;
    if (output > 1.0f) output = 1.0f;
    if (output < -1.0f) output = -1.0f;
    
    return output;
}

bool SynthArchitecture::isPlaying() const {
    for (auto& voice : voices) {
        if (voice->isActive()) return true;
    }
    return false;
}

void SynthArchitecture::reset() {
    for (auto& voice : voices) {
        voice->reset();
    }
    effectChain.reset();
}

void SynthArchitecture::allNotesOff() {
    for (auto& voice : voices) {
        voice->noteOff();
        voice->reset();
    }
}

void SynthArchitecture::setFilterType(FilterType type) {
    currentFilterType = type;
    updateAllVoices();
}

void SynthArchitecture::setCutoff(float freq) {
    cutoff = freq;
    updateAllVoices();
}

void SynthArchitecture::setResonance(float res) {
    resonance = res;
    updateAllVoices();
}

void SynthArchitecture::setFilterEnvelopeAmount(float amount) {
    filterEnvelopeAmount = amount;
    updateAllVoices();
}

void SynthArchitecture::setWaveform(Waveform wav) {
    currentWaveform = wav;
    updateAllVoices();
}

void SynthArchitecture::setAmpEnvelopeCurve(EnvelopeCurve curve) {
    ampEnvelopeCurve = curve;
    updateAllVoices();
}

void SynthArchitecture::setFilterEnvelopeCurve(EnvelopeCurve curve) {
    filterEnvelopeCurve = curve;
    updateAllVoices();
}

void SynthArchitecture::setAmpAttack(float value) {
    ampAttack = value;
}

void SynthArchitecture::setAmpDecay(float value) {
    ampDecay = value;
}

void SynthArchitecture::setAmpSustain(float value) {
    ampSustain = value;
}

void SynthArchitecture::setAmpRelease(float value) {
    ampRelease = value;
}

void SynthArchitecture::setFilterAttack(float value) {
    filtAttack = value;
}

void SynthArchitecture::setFilterDecay(float value) {
    filtDecay = value;
}

void SynthArchitecture::setFilterSustain(float value) {
    filtSustain = value;
}

void SynthArchitecture::setFilterRelease(float value) {
    filtRelease = value;
}

void SynthArchitecture::setVolume(float vol) {
    volume = vol;
}

void SynthArchitecture::setEffectEnabled(int effectIndex, bool enabled) {
    switch (effectIndex) {
        case 0: effectChain.getDelay()->setEnabled(enabled); break;
        case 1: effectChain.getReverb()->setEnabled(enabled); break;
        case 2: effectChain.getChorus()->setEnabled(enabled); break;
        case 3: effectChain.getDistortion()->setEnabled(enabled); break;
    }
}

void SynthArchitecture::setDelayParams(float timeMs, float feedback, float mix) {
    effectChain.getDelay()->setDelayTime(timeMs);
    effectChain.getDelay()->setFeedback(feedback);
    effectChain.getDelay()->setMix(mix);
}

void SynthArchitecture::setReverbParams(float decay, float mix) {
    effectChain.getReverb()->setDecay(decay);
    effectChain.getReverb()->setMix(mix);
}

void SynthArchitecture::setChorusParams(float depth, float rate, float mix) {
    effectChain.getChorus()->setDepth(depth);
    effectChain.getChorus()->setRate(rate);
    effectChain.getChorus()->setMix(mix);
}

void SynthArchitecture::setDistortionParams(float drive, float mix) {
    effectChain.getDistortion()->setDrive(drive);
    effectChain.getDistortion()->setMix(mix);
}
