#include "DigitsVoice.h"
#include <cmath>
#include <algorithm>

DigitsVoice::DigitsVoice()
    : note(-1), keyon(0),
      osc1_shaper(1.0f), osc1_skew(0.5f), osc1_basis(0),
      osc2_shaper(1.0f), osc2_skew(0.5f), osc2_basis(0),
      mix(0.35f), detune(0), volume(0.7f), resoVol(0),
      amp_attack(0), amp_decay(0), amp_sustain(1.0f), amp_release(0),
      flt_attack(0), flt_decay(0), flt_sustain(1.0f), flt_release(0),
      flt_env_depth(0),
      cutoff(0.7f), resonance(0), filterType(0),
      m_sampleRate(48000), m_noteFreq(440),
      m_ampPhase(kFinished), m_fltPhase(kFinished),
      m_ampLevel(0), m_ampLastOut(0),
      m_fltLevel(0), m_fltLastOut(0),
      m_svf_lp(0), m_svf_bp(0)
{
}

void DigitsVoice::init(float sampleRate)
{
    m_sampleRate = sampleRate;
    reset();
}

void DigitsVoice::reset()
{
    note = -1;
    keyon = 0;
    m_ampPhase = kFinished;
    m_fltPhase = kFinished;
    m_ampLevel = 0;
    m_ampLastOut = 0;
    m_fltLevel = 0;
    m_fltLastOut = 0;
    osc1.resetPhase();
    osc2.resetPhase();
    resoGen.resetPhase();
    m_svf_lp = m_svf_bp = 0;
}

void DigitsVoice::noteOn(int midiNote)
{
    note = midiNote;
    keyon = 1;
    m_noteFreq = 440.0f * powf(2.0f, (midiNote - 69.0f) / 12.0f);

    osc1.setPitch(m_noteFreq, m_sampleRate);
    osc1.resetPhase();
    osc2.setPitch(m_noteFreq, m_sampleRate);
    osc2.resetPhase();
    resoGen.setPitch(m_noteFreq, m_sampleRate);
    resoGen.resetPhase();

    m_ampPhase = kAttack;
    m_ampLevel = 0;
    m_ampLastOut = 0;

    m_fltPhase = kAttack;
    m_fltLevel = 0;
    m_fltLastOut = 0;
}

void DigitsVoice::noteOff()
{
    keyon = 0;
    m_ampPhase = kRelease;
    m_fltPhase = kRelease;
}

bool DigitsVoice::isFinished() const
{
    return m_ampPhase == kFinished && keyon == 0;
}

float DigitsVoice::coeffFromMs(float ms) const
{
    if (ms <= 0) return 0;
    float sams = (ms / 1000.0f) * m_sampleRate;
    return expf(-1.0f / sams);
}

static float envValueToMs(float value)
{
    return 0.003f * powf(5000.0f, value) * 1000.0f;
}

static float detuneSemitones(int val)
{
    float t = (val - 64) / 64.0f;
    return t * fabsf(t) * 1.0f;
}

float DigitsVoice::process(float lfoOut)
{
    if (m_ampPhase == kFinished && m_fltPhase == kFinished)
        return 0;

    float shaper1 = osc1_shaper / 63.5f;
    if (shaper1 < 0.01f) shaper1 = 0.01f;
    float skew1 = osc1_skew / 127.0f;
    float bright1 = 0.3f;
    float pwm1 = 0.5f;
    int basis1 = (osc1_basis > 63) ? 1 : 0;

    float shaper2 = osc2_shaper / 63.5f;
    if (shaper2 < 0.01f) shaper2 = 0.01f;
    float skew2 = osc2_skew / 127.0f;
    float bright2 = 0.3f;
    float pwm2 = 0.5f;
    int basis2 = (osc2_basis > 63) ? 1 : 0;

    float osc1freq = m_noteFreq * powf(2.0f, detuneSemitones((int)detune) / 12.0f);

    osc1.setPitch(osc1freq, m_sampleRate);
    float osc1_out = osc1.process(0, shaper1, skew1, bright1, pwm1, basis1);

    float osc2freq = m_noteFreq * powf(2.0f, -detuneSemitones((int)detune) / 12.0f);
    osc2.setPitch(osc2freq, m_sampleRate);
    float osc2_out = osc2.process(osc1_out * 0.3f, shaper2, skew2, bright2, pwm2, basis2);

    float mixVal = mix / 127.0f;
    float mixed = osc1_out * (1.0f - mixVal) + osc2_out * mixVal;

    float resoOut = resoGen.process(shaper1, pwm1, bright1, 1.0f, resoVol / 127.0f,
                                    DigitsResoGen::kSawQuarter);
    mixed += resoOut;

    {
        float m_attackCoeff = coeffFromMs(envValueToMs(amp_attack / 127.0f));
        float m_decayCoeff = coeffFromMs(envValueToMs(amp_decay / 127.0f));
        float m_sustainLevel = amp_sustain / 127.0f;
        float m_releaseCoeff = coeffFromMs(envValueToMs(amp_release / 127.0f));

        switch (m_ampPhase) {
        case kAttack:
            m_ampLevel = m_attackCoeff * m_ampLastOut + (1.0f - m_attackCoeff);
            m_ampLastOut = m_ampLevel;
            if (m_ampLevel >= 0.98f) {
                m_ampLevel = 0.98f;
                m_ampPhase = kDecay;
            }
            break;
        case kDecay:
            m_ampLevel = m_decayCoeff * m_ampLastOut + (1.0f - m_decayCoeff) * m_sustainLevel;
            m_ampLastOut = m_ampLevel;
            if (m_ampLevel <= m_sustainLevel + 0.001f)
                m_ampPhase = kSustain;
            break;
        case kSustain:
            // Live sustain changes: snap to the current sustain level so
            // moving the control while a note is held changes the volume
            // immediately (release uses m_ampLastOut, keep it in sync).
            m_ampLevel = m_sustainLevel;
            m_ampLastOut = m_ampLevel;
            if (m_ampLevel <= 0.001f) {
                m_ampLevel = 0;
                m_ampPhase = kFinished;
            }
            break;
        case kRelease:
            m_ampLevel = m_releaseCoeff * m_ampLastOut;
            m_ampLastOut = m_ampLevel;
            if (m_ampLevel <= 0.001f) {
                m_ampLevel = 0;
                m_ampPhase = kFinished;
            }
            break;
        default:
            break;
        }
    }

    {
        float fc = coeffFromMs(envValueToMs(flt_attack / 127.0f));
        float fd = coeffFromMs(envValueToMs(flt_decay / 127.0f));
        float fs = flt_sustain / 127.0f;
        float fr = coeffFromMs(envValueToMs(flt_release / 127.0f));

        switch (m_fltPhase) {
        case kAttack:
            m_fltLevel = fc * m_fltLastOut + (1.0f - fc);
            m_fltLastOut = m_fltLevel;
            if (m_fltLevel >= 0.98f) {
                m_fltLevel = 0.98f;
                m_fltPhase = kDecay;
            }
            break;
        case kDecay:
            m_fltLevel = fd * m_fltLastOut + (1.0f - fd) * fs;
            m_fltLastOut = m_fltLevel;
            if (m_fltLevel <= fs + 0.001f)
                m_fltPhase = kSustain;
            break;
        case kSustain:
            // Live sustain changes: snap to the current sustain level so
            // moving the control while a note is held takes effect immediately.
            m_fltLevel = fs;
            m_fltLastOut = m_fltLevel;
            break;
        case kRelease:
            m_fltLevel = fr * m_fltLastOut;
            m_fltLastOut = m_fltLevel;
            if (m_fltLevel <= 0.001f) {
                m_fltLevel = 0;
                m_fltPhase = kFinished;
            }
            break;
        default:
            break;
        }
    }

    float fltEnvMod = (flt_env_depth - 64.0f) / 64.0f;
    float modCutoff = cutoff + (m_fltLevel * fltEnvMod * 30.0f);
    if (modCutoff < 0) modCutoff = 0;
    if (modCutoff > 127) modCutoff = 127;

    float ampOut = mixed * m_ampLevel;

    // Simple one-pole LPF (always stable)
    float fc = 50.0f * powf(2.0f, 8.643856f * modCutoff / 127.0f);
    if (fc > 20000.0f) fc = 20000.0f;
    float g = expf(-2.0f * (float)M_PI * fc / m_sampleRate);
    float res = resonance / 127.0f;

    float filtered = (1.0f - g) * ampOut + g * m_svf_lp;
    m_svf_lp = filtered;

    return filtered * (volume / 127.0f);
}
