#ifndef VOICE_H
#define VOICE_H

#include "../oscillators/saw_oscillator.h"
#include "../filters/filter_base.h"
#include "../filters/moog_filter.h"
#include "../filters/high_pass_filter.h"
#include "../filters/korgms20_filter.h"
#include "../filters/oberheim_sem_filter.h"
#include "../envelopes/flexible_envelope.h"

enum class FilterType {
    MOOG,
    KORG_MS20,
    OBERHEIM_SEM,
    MOOG_HPF
};

class Voice {
private:
    SawtoothOscillator oscillator1;
    SawtoothOscillator oscillator2;
    MoogLadderFilter moogFilter;
    HighPassFilter moogHPFFilter;
    KorgMS20Filter korgFilter;
    OberheimSEMFilter oberheimFilter;
    FlexibleEnvelope amplitudeEnvelope;
    FlexibleEnvelope filterEnvelope;
    FilterBase* currentFilter;
    
    float frequency;
    bool active;
    int noteId;
    float filterEnvelopeAmount;
    float baseCutoff;
    float resonance;
    float oscMix;
    float osc2Detune;
    
public:
    Voice(float sampleRate = 48000.0f);
    ~Voice();
    
    void setSampleRate(float rate);
    void noteOn(float freq, int id, FilterType filterType,
                float cutoff, float res, float filterEnvAmount,
                float ampAttack, float ampDecay, float ampSustain, float ampRelease,
                float filtAttack, float filtDecay, float filtSustain, float filtRelease,
                EnvelopeCurve ampCurve = EnvelopeCurve::EXPONENTIAL,
                EnvelopeCurve filtCurve = EnvelopeCurve::EXPONENTIAL);
    void noteOff();
    float process();
    bool isActive() const { return active; }
    int getNoteId() const { return noteId; }
    float getFrequency() const { return frequency; }
    float getEnvelopeLevel() const { return amplitudeEnvelope.getCurrentLevel(); }
    void reset();
    
    // Update global parameters
    void updateFilterType(FilterType type);
    void updateCutoff(float cutoff);
    void updateResonance(float res);
    void updateFilterEnvelopeAmount(float amount);
    void updateWaveform(Waveform wav);
    void updateOscMix(float mix);
    void updateOsc2Detune(float detune);
    void updateEnvelopeCurves(EnvelopeCurve ampCurve, EnvelopeCurve filtCurve);
};

#endif
