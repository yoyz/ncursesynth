#ifndef DIGITS_ENGINE_H
#define DIGITS_ENGINE_H

#include "DigitsVoice.h"
#include "digits_types.h"

class DigitsEngine {
public:
    DigitsEngine();
    ~DigitsEngine();

    void init();
    void reset();
    void process();

    void noteOn(int midiNote, int velocity);
    void noteOff(int midiNote);

    float* getOutputBuffer() { return m_buffer; }
    int getBufferSize() const { return DIGITS_SAM; }

    void setI(int index, int value);
    int getI(int index) const;

    int getKeyOn() const;
    int getLastNote() const;

    DigitsVoice voices[DIGITS_MAX_VOICES];
    int polyphony;

    // Parameters
    int osc1_type, osc2_type;
    int osc1_shaper, osc1_skew, osc1_basis;
    int osc2_shaper, osc2_skew, osc2_basis;
    int osc12_mix;
    int osc1_detune, osc2_detune;
    int osc1_scale, osc2_scale;
    int osc1_amp, osc2_amp;
    int filter1_type, filter1_cutoff, filter1_resonance;
    int adsr_env0_attack, adsr_env0_decay, adsr_env0_sustain, adsr_env0_release;
    int adsr_env1_attack, adsr_env1_decay, adsr_env1_sustain, adsr_env1_release;
    int env1_depth;
    int lfo1_freq, lfo1_depth;
    int lfo1_shape, lfo1_wave;
    int amp;
    int reso_vol;
    int lfo_amt_shaper;

private:
    float m_buffer[DIGITS_SAM];
    float m_lfoPhase;
    float m_lfoValue;
    int m_keyon;
    int m_last_note;

    void setVoiceParams(int v);
    int findFreeVoice(int midiNote);
    int findVoiceForNote(int midiNote);
    int stealVoice();
};

#endif
