#include "voice.h"
#include <algorithm>
#include <cmath>

Voice::Voice(float sampleRate) 
    : oscillator(sampleRate),
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
      resonance(0.5f) {
    
    currentFilter = &moogFilter;
    oscillator.setWaveform(Waveform::SAWTOOTH);
}

Voice::~Voice() {}

void Voice::setSampleRate(float rate) {
    oscillator.setSampleRate(rate);
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
    
    // Reset everything for new note
    frequency = freq;
    noteId = id;
    baseCutoff = cutoff;
    resonance = res;
    filterEnvelopeAmount = filterEnvAmount;
    
    // Reset oscillator
    oscillator.setFrequency(freq);
    oscillator.reset();
    
    // Set filter type
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
    
    // Reset and set filter
    currentFilter->reset();
    currentFilter->setCutoff(cutoff);
    currentFilter->setResonance(res);
    
    // Configure amplitude envelope
    amplitudeEnvelope.reset();
    amplitudeEnvelope.setAttack(ampAttack);
    amplitudeEnvelope.setDecay(ampDecay);
    amplitudeEnvelope.setSustain(ampSustain);
    amplitudeEnvelope.setRelease(ampRelease);
    amplitudeEnvelope.setAttackCurve(ampCurve);
    amplitudeEnvelope.setDecayCurve(ampCurve);
    amplitudeEnvelope.setReleaseCurve(ampCurve);
    
    // Configure filter envelope
    filterEnvelope.reset();
    filterEnvelope.setAttack(filtAttack);
    filterEnvelope.setDecay(filtDecay);
    filterEnvelope.setSustain(filtSustain);
    filterEnvelope.setRelease(filtRelease);
    filterEnvelope.setAttackCurve(filtCurve);
    filterEnvelope.setDecayCurve(filtCurve);
    filterEnvelope.setReleaseCurve(filtCurve);
    
    // Trigger envelopes
    amplitudeEnvelope.noteOn();
    filterEnvelope.noteOn();
    active = true;
}

void Voice::noteOff() {
    if (active) {
        amplitudeEnvelope.noteOff();
        filterEnvelope.noteOff();
        // Don't set active = false here - let the envelope finish naturally
        // The voice will be considered active until envelope completes
    }
}

float Voice::process() {
    // Check if envelope is finished
    if (active && !amplitudeEnvelope.isActive()) {
        active = false;
        return 0.0f;
    }
    
    if (!active && !amplitudeEnvelope.isActive()) {
        return 0.0f;
    }
    
    // Process envelopes
    float ampEnvValue = amplitudeEnvelope.process();
    float filterEnvValue = filterEnvelope.process();
    
    // Apply filter envelope to cutoff
    float modulatedCutoff = baseCutoff + (filterEnvValue * filterEnvelopeAmount * 4000.0f);
    modulatedCutoff = std::max(20.0f, std::min(8000.0f, modulatedCutoff));
    currentFilter->setCutoff(modulatedCutoff);
    
    // Generate sound
    float osc_out = oscillator.process();
    float filtered = currentFilter->process(osc_out);
    float output = filtered * ampEnvValue * 0.3f;
    
    return output;
}

void Voice::reset() {
    active = false;
    noteId = -1;
    oscillator.reset();
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
    oscillator.setWaveform(wav);
}

void Voice::updateEnvelopeCurves(EnvelopeCurve ampCurve, EnvelopeCurve filtCurve) {
    amplitudeEnvelope.setAttackCurve(ampCurve);
    amplitudeEnvelope.setDecayCurve(ampCurve);
    amplitudeEnvelope.setReleaseCurve(ampCurve);
    
    filterEnvelope.setAttackCurve(filtCurve);
    filterEnvelope.setDecayCurve(filtCurve);
    filterEnvelope.setReleaseCurve(filtCurve);
}
