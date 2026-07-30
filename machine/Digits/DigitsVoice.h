#ifndef DIGITS_VOICE_H
#define DIGITS_VOICE_H

#include "DigitsPhaseDist.h"

class DigitsVoice {
public:
    DigitsVoice();
    void init(float sampleRate);
    void reset();
    void noteOn(int midiNote);
    void noteOff();
    bool isFinished() const;
    float process(float lfoOut);

    int note;
    int keyon;

    DigitsPhaseDist osc1;
    DigitsPhaseDist osc2;
    DigitsResoGen resoGen;

    float osc1_shaper;
    float osc1_skew;
    float osc1_basis;
    float osc2_shaper;
    float osc2_skew;
    float osc2_basis;
    float mix;
    float detune;
    float volume;
    float resoVol;

    float amp_attack;
    float amp_decay;
    float amp_sustain;
    float amp_release;
    float flt_attack;
    float flt_decay;
    float flt_sustain;
    float flt_release;
    float flt_env_depth;

    float cutoff;
    float resonance;
    int filterType;

private:
    float m_sampleRate;
    float m_noteFreq;

    enum EnvPhase { kAttack, kDecay, kSustain, kRelease, kFinished };
    EnvPhase m_ampPhase;
    EnvPhase m_fltPhase;
    float m_ampLevel;
    float m_ampLastOut;
    float m_fltLevel;
    float m_fltLastOut;

    float coeffFromMs(float ms) const;

    // Chamberlin SVF state
    float m_svf_lp, m_svf_bp;
};

#endif
