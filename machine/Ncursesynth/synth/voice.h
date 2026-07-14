#ifndef VOICE_H
#define VOICE_H

#include "../oscillators/saw_oscillator.h"
#include "../filters/filter_base.h"
#include "../filters/moog_filter.h"
#include "../filters/high_pass_filter.h"
#include "../filters/korgms20_filter.h"
#include "../filters/oberheim_sem_filter.h"
#include "../filters/svf_filter.h"
#include "../filters/diode_filter.h"
#include "../filters/formant_filter.h"
#include "../filters/comb_filter.h"
#include "../envelopes/flexible_envelope.h"

enum class FilterType {
    MOOG,
    KORG_MS20,
    OBERHEIM_SEM,
    MOOG_HPF,
    SVF_LP12,
    SVF_HP12,
    SVF_BP12,
    SVF_AP12,
    DIODE,
    FORMANT,
    COMB
};

class Voice {
private:
    SawtoothOscillator oscillator1;
    SawtoothOscillator oscillator2;
    MoogLadderFilter moogFilter;
    HighPassFilter moogHPFFilter;
    KorgMS20Filter korgFilter;
    OberheimSEMFilter oberheimFilter;
    StateVariableFilter svfFilter;
    DiodeFilter diodeLadder;
    FormantFilter formantFilter;
    CombFilter combFilterInstance;
    FlexibleEnvelope amplitudeEnvelope;
    FlexibleEnvelope filterEnvelope;
    FilterBase* currentFilter;
    
    float frequency;
    bool active;
    bool released;
    int noteId;
    float filterEnvelopeAmount;
    float baseCutoff;
    float resonance;
    float oscMix;
    float osc1Detune;
    float osc2Detune;
    float osc1Scale;
    float osc2Scale;
    float osc1Amp;
    float osc2Amp;
    Waveform osc1Waveform;
    Waveform osc2Waveform;
    
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
    bool isReleased() const { return released; }
    void reset();
    
    // Update global parameters
    void updateFilterType(FilterType type);
    void updateCutoff(float cutoff);
    void updateResonance(float res);
    void updateFilterEnvelopeAmount(float amount);
    void updateOsc1Waveform(Waveform wav);
    void updateOsc2Waveform(Waveform wav);
    void updateOscMix(float mix);
    void updateOsc1Detune(float detune);
    void updateOsc2Detune(float detune);
    void updateOsc1Scale(float semitones);
    void updateOsc2Scale(float semitones);
    void updateOsc1Amp(float amp);
    void updateOsc2Amp(float amp);
    void updateEnvelopeCurves(EnvelopeCurve ampCurve, EnvelopeCurve filtCurve);
};

#endif
