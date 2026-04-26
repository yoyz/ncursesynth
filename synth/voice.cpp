#include "voice.h"
#include <algorithm>
#include <cmath>

Voice::Voice(float sampleRate) 
    : oscillator1(sampleRate),
      oscillator2(sampleRate),
      moogFilter(sampleRate),
      korgFilter(sampleRate),
      oberheimFilter(sampleRate),
      amplitudeEnvelope(sampleRate),
      filterEnvelope(sampleRate),
      frequency(440.0f),
      active(false),
      noteId(-1),
      filterEnvelopeAmount(0.5f),
      baseCutoff(1000.0f),
      resonance(0.5f),
      oscMix(0.5f),
      osc2Detune(0.0f) {
    
    currentFilter = &moogFilter;
    oscillator1.setWaveform(Waveform::SAWTOOTH);
    oscillator2.setWaveform(Waveform::SAWTOOTH);
}

Voice::~Voice() {}

void Voice::setSampleRate(float rate) {
    oscillator1.setSampleRate(rate);
    oscillator2.setSampleRate(rate);
    moogFilter.setSampleRate(rate);
    korgFilter.setSampleRate(rate);
    oberheimFilter.setSampleRate(rate);
    amplitudeEnvelope.setSampleRate(rate);
    filterEnvelope.setSampleRate(rate);
}

void Voice::noteOn(float freq, int id, FilterType filterType,
                   float cutoff, float res, float filterEnvAmount,
                   float ampAttack, float ampDecay, float ampSustain, float ampRelease,
                   float filtAttack, float filtDecay, float filtSustain, float filtRelease,
                   EnvelopeCurve ampCurve, EnvelopeCurve filtCurve) {
    
    frequency = freq;
    noteId = id;
    baseCutoff = cutoff;
    resonance = res;
    filterEnvelopeAmount = filterEnvAmount;
    
    oscillator1.setFrequency(freq);
    oscillator1.reset();
    
    oscillator2.setFrequency(freq);
    oscillator2.reset();
    
    switch (filterType) {
        case FilterType::MOOG:
            currentFilter = &moogFilter;
            break;
        case FilterType::KORG_MS20:
            currentFilter = &korgFilter;
            break;
        case FilterType::OBERHEIM_SEM:
            currentFilter = &oberheimFilter;
            break;
    }
    
    currentFilter->reset();
    currentFilter->setCutoff(cutoff);
    currentFilter->setResonance(res);
    
    // Fully reset both envelopes to start fresh
    amplitudeEnvelope.reset();
    amplitudeEnvelope.setAttack(ampAttack);
    amplitudeEnvelope.setDecay(ampDecay);
    amplitudeEnvelope.setSustain(ampSustain);
    amplitudeEnvelope.setRelease(ampRelease);
    amplitudeEnvelope.setAttackCurve(ampCurve);
    amplitudeEnvelope.setDecayCurve(ampCurve);
    amplitudeEnvelope.setReleaseCurve(ampCurve);
    
    filterEnvelope.reset();
    filterEnvelope.setAttack(filtAttack);
    filterEnvelope.setDecay(filtDecay);
    filterEnvelope.setSustain(filtSustain);
    filterEnvelope.setRelease(filtRelease);
    filterEnvelope.setAttackCurve(filtCurve);
    filterEnvelope.setDecayCurve(filtCurve);
    filterEnvelope.setReleaseCurve(filtCurve);
    
    amplitudeEnvelope.noteOn();
    filterEnvelope.noteOn();
    active = true;
}

void Voice::noteOff() {
    if (active) {
        amplitudeEnvelope.noteOff();
        filterEnvelope.noteOff();
    }
}

float Voice::process() {
    // Process the amp envelope - it's the master envelope
    float ampEnvValue = amplitudeEnvelope.process();
    
    // Check if amp envelope finished and we're past sustain
    if (!amplitudeEnvelope.isActive() && amplitudeEnvelope.getCurrentLevel() <= 0.001f) {
        active = false;
        return 0.0f;
    }
    
    // Keep active if envelope is still running (attack, decay, or release phases)
    active = true;
    
    float filterEnvValue = filterEnvelope.process();
    
    float modulatedCutoff = baseCutoff + (filterEnvValue * filterEnvelopeAmount * 4000.0f);
    modulatedCutoff = std::max(20.0f, std::min(8000.0f, modulatedCutoff));
    currentFilter->setCutoff(modulatedCutoff);
    
    float osc1_out = oscillator1.process();
    
    float osc2_freq = frequency * std::powf(2.0f, osc2Detune / 12.0f);
    oscillator2.setFrequency(osc2_freq);
    float osc2_out = oscillator2.process();
    
    float mixed = osc1_out * (1.0f - oscMix) + osc2_out * oscMix;
    
    float filtered = currentFilter->process(mixed);
    float output = filtered * ampEnvValue * 0.3f;
    
    return output;
}

void Voice::reset() {
    active = false;
    noteId = -1;
    oscillator1.reset();
    oscillator2.reset();
    moogFilter.reset();
    korgFilter.reset();
    oberheimFilter.reset();
    amplitudeEnvelope.reset();
    filterEnvelope.reset();
}

void Voice::updateFilterType(FilterType type) {
    switch (type) {
        case FilterType::MOOG:
            currentFilter = &moogFilter;
            break;
        case FilterType::KORG_MS20:
            currentFilter = &korgFilter;
            break;
        case FilterType::OBERHEIM_SEM:
            currentFilter = &oberheimFilter;
            break;
    }
}

void Voice::updateCutoff(float cutoff) {
    baseCutoff = cutoff;
}

void Voice::updateResonance(float res) {
    resonance = res;
    if (currentFilter) {
        currentFilter->setResonance(resonance);
    }
}

void Voice::updateFilterEnvelopeAmount(float amount) {
    filterEnvelopeAmount = amount;
}

void Voice::updateWaveform(Waveform wav) {
    oscillator1.setWaveform(wav);
    oscillator2.setWaveform(wav);
}

void Voice::updateOscMix(float mix) {
    oscMix = mix;
}

void Voice::updateOsc2Detune(float detune) {
    osc2Detune = detune;
}

void Voice::updateEnvelopeCurves(EnvelopeCurve ampCurve, EnvelopeCurve filtCurve) {
    amplitudeEnvelope.setAttackCurve(ampCurve);
    amplitudeEnvelope.setDecayCurve(ampCurve);
    amplitudeEnvelope.setReleaseCurve(ampCurve);
    
    filterEnvelope.setAttackCurve(filtCurve);
    filterEnvelope.setDecayCurve(filtCurve);
    filterEnvelope.setReleaseCurve(filtCurve);
}