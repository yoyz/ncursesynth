#include "synth_architecture.h"
#include <algorithm>
#include <cmath>

SynthArchitecture::SynthArchitecture(int polyphony, float rate) 
    : maxPolyphony(polyphony), nextNoteId(0), sampleRate(rate),
      currentFilterType(FilterType::MOOG),
      cutoff(1000.0f),
      hpfCutoff(0.0f),
      resonance(0.5f),
      filterEnvelopeAmount(0.5f),
      ampEnvelopeCurve(EnvelopeCurve::EXPONENTIAL),
      filterEnvelopeCurve(EnvelopeCurve::EXPONENTIAL),
      ampAttack(0.0f),
      ampDecay(1.0f),
      ampSustain(0.5f),
      ampRelease(0.5f),
      filtAttack(0.0f),
      filtDecay(1.0f),
      filtSustain(0.5f),
      filtRelease(0.5f),
      volume(0.7f),
      osc1Waveform(Waveform::SAWTOOTH),
      osc2Waveform(Waveform::SAWTOOTH),
      oscMix(0.5f),
      osc1Detune(0.0f),
      osc2Detune(0.0f),
      osc1Scale(0.0f),
      osc2Scale(0.0f),
      osc1Amp(1.0f),
      osc2Amp(1.0f),
      presetManager("bank/ncursesynth") {
    
    // Create voices
    for (int i = 0; i < maxPolyphony; i++) {
        voices.push_back(std::unique_ptr<Voice>(new Voice(sampleRate)));
    }
    
    effectChain.setSampleRate(sampleRate);
    
    // Load bank if exists
    presetManager.loadBank();
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
    // Priority 1: Completely inactive voice
    for (size_t i = 0; i < voices.size(); i++) {
        if (!voices[i]->isActive()) {
            return i;
        }
    }

    // Priority 2: Released voice (in release tail, envelope still playing)
    for (size_t i = 0; i < voices.size(); i++) {
        if (voices[i]->isReleased()) {
            return i;
        }
    }

    // Priority 3: Oldest held voice (last resort — steal voice 0)
    return 0;
}


void SynthArchitecture::updateAllVoices() {
    for (auto& voice : voices) {
        voice->updateFilterType(currentFilterType);
        voice->updateCutoff(cutoff);
        voice->updateResonance(resonance);
        voice->updateFilterEnvelopeAmount(filterEnvelopeAmount);
        voice->updateOsc1Waveform(osc1Waveform);
        voice->updateOsc2Waveform(osc2Waveform);
        voice->updateOscMix(oscMix);
        voice->updateOsc1Detune(osc1Detune);
        voice->updateOsc2Detune(osc2Detune);
        voice->updateOsc1Scale(osc1Scale);
        voice->updateOsc2Scale(osc2Scale);
        voice->updateOsc1Amp(osc1Amp);
        voice->updateOsc2Amp(osc2Amp);
        voice->updateEnvelopeCurves(ampEnvelopeCurve, filterEnvelopeCurve);
    }
}

void SynthArchitecture::noteOn(float frequency) {
    // If a voice is already playing this frequency, retrigger it
    for (auto& voice : voices) {
        if (voice->isActive() && std::abs(voice->getFrequency() - frequency) < 0.1f) {
            voice->noteOn(frequency, nextNoteId++,
                         currentFilterType, cutoff, resonance, filterEnvelopeAmount,
                         ampAttack, ampDecay, ampSustain, ampRelease,
                         filtAttack, filtDecay, filtSustain, filtRelease,
                         ampEnvelopeCurve, filterEnvelopeCurve);
            return;
        }
    }

    int idx = findFreeVoice();
    voices[idx]->reset();
    voices[idx]->noteOn(frequency, nextNoteId++,
                       currentFilterType, cutoff, resonance, filterEnvelopeAmount,
                       ampAttack, ampDecay, ampSustain, ampRelease,
                       filtAttack, filtDecay, filtSustain, filtRelease,
                       ampEnvelopeCurve, filterEnvelopeCurve);
}

void SynthArchitecture::noteOff(float frequency) {
    // Find the exact voice playing this frequency
    for (auto& voice : voices) {
        if (voice->isActive() && std::abs(voice->getFrequency() - frequency) < 0.1f) {
            voice->noteOff();
            return;
        }
    }
    // If no exact match, try to find any voice close to this note
    for (auto& voice : voices) {
        if (voice->isActive()) {
            voice->noteOff();
            break;
        }
    }
}

float SynthArchitecture::process() {
    float output = 0.0f;
    int activeCount = 0;
    
    // Mix all active voices
    for (auto& voice : voices) {
        if (voice->isActive()) {
            output += voice->process();
            activeCount++;
        }
    }
    

    
    // Apply effects chain
    output = effectChain.process(output);
    
    // Apply master volume and soft-clip
    output = output * volume;
    float absOut = fabsf(output);
    if (absOut > 1.0f) {
        float over = (absOut - 1.0f) * 0.5f;
        output = (output > 0 ? 1.0f : -1.0f) * (1.0f - 1.0f / (1.0f + over));
    }
    
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

void SynthArchitecture::setHPFCutoff(float freq) {
    hpfCutoff = freq;
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

void SynthArchitecture::setOsc1Waveform(Waveform wav) {
    osc1Waveform = wav;
    updateAllVoices();
}

void SynthArchitecture::setOsc2Waveform(Waveform wav) {
    osc2Waveform = wav;
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

void SynthArchitecture::setOscMix(float mix) {
    oscMix = mix;
    updateAllVoices();
}

void SynthArchitecture::setOsc1Detune(float detune) {
    osc1Detune = detune;
    updateAllVoices();
}

void SynthArchitecture::setOsc2Detune(float detune) {
    osc2Detune = detune;
    updateAllVoices();
}

void SynthArchitecture::setOsc1Scale(float semitones) {
    osc1Scale = semitones;
    updateAllVoices();
}

void SynthArchitecture::setOsc2Scale(float semitones) {
    osc2Scale = semitones;
    updateAllVoices();
}

void SynthArchitecture::setOsc1Amp(float amp) {
    osc1Amp = amp;
    updateAllVoices();
}

void SynthArchitecture::setOsc2Amp(float amp) {
    osc2Amp = amp;
    updateAllVoices();
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
