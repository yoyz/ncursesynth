#include "DigitsEngine.h"
#include <cmath>
#include <cstring>
#include <algorithm>

DigitsEngine::DigitsEngine()
    : polyphony(DIGITS_MAX_VOICES),
      osc1_type(0), osc2_type(0),
      osc1_shaper(64), osc1_skew(64), osc1_basis(0),
      osc2_shaper(64), osc2_skew(64), osc2_basis(0),
      osc12_mix(45),
      osc1_detune(64), osc2_detune(64),
      osc1_scale(64), osc2_scale(64),
      osc1_amp(127), osc2_amp(127),
      filter1_type(0), filter1_cutoff(64), filter1_resonance(0),
      adsr_env0_attack(0), adsr_env0_decay(64), adsr_env0_sustain(127), adsr_env0_release(64),
      adsr_env1_attack(0), adsr_env1_decay(64), adsr_env1_sustain(127), adsr_env1_release(64),
      env1_depth(64),
      lfo1_freq(32), lfo1_depth(0),
      lfo1_shape(0), lfo1_wave(0),
      amp(101),
      reso_vol(0),
      lfo_amt_shaper(0),
      m_lfoPhase(0), m_lfoValue(0),
      m_keyon(0), m_last_note(-1)
{
    for (int i = 0; i < DIGITS_SAM; i++)
        m_buffer[i] = 0;
}

DigitsEngine::~DigitsEngine()
{
}

void DigitsEngine::init()
{
    for (int i = 0; i < DIGITS_MAX_VOICES; i++)
        voices[i].init(DEFAULTFREQ);
}

void DigitsEngine::reset()
{
    for (int i = 0; i < DIGITS_MAX_VOICES; i++)
        voices[i].reset();
    m_lfoPhase = 0;
    m_lfoValue = 0;
    for (int i = 0; i < DIGITS_SAM; i++)
        m_buffer[i] = 0;
    m_keyon = 0;
}

void DigitsEngine::noteOn(int midiNote, int velocity)
{
    // Priority 0: retrigger same note if already playing
    int same = findVoiceForNote(midiNote);
    if (same >= 0 && voices[same].keyon) {
        voices[same].reset();
        voices[same].noteOn(midiNote);
        setVoiceParams(same);
        m_keyon = 1;
        m_last_note = midiNote;
        return;
    }

    int v = findFreeVoice(midiNote);
    DigitsVoice& voice = voices[v];
    voice.reset();
    voice.noteOn(midiNote);
    setVoiceParams(v);

    m_keyon = 1;
    m_last_note = midiNote;
}

void DigitsEngine::noteOff(int midiNote)
{
    int v = findVoiceForNote(midiNote);
    if (v >= 0)
        voices[v].noteOff();
    if (m_last_note == midiNote)
        m_keyon = 0;
}

void DigitsEngine::process()
{
    float lfoFreq = 440.0f * powf(2.0f, (lfo1_freq / 127.0f * 60.0f - 60.0f) / 12.0f);
    float lfoDelta = lfoFreq / DEFAULTFREQ;
    float lfo_depth = lfo1_depth / 127.0f;

    for (int s = 0; s < DIGITS_SAM; s++) {
        m_lfoPhase += lfoDelta;
        if (m_lfoPhase >= 1.0f) m_lfoPhase -= 1.0f;

        int lfoShape = lfo1_shape;
        if (lfoShape < 0) lfoShape = 0;
        if (lfoShape > 3) lfoShape = 3;
        switch (lfoShape) {
        case 0: m_lfoValue = sinf(m_lfoPhase * 2.0f * (float)M_PI); break;
        case 1: m_lfoValue = m_lfoPhase * 2.0f - 1.0f; break;
        case 2: m_lfoValue = (m_lfoPhase < 0.5f) ? 1.0f : -1.0f; break;
        default: m_lfoValue = ((float)rand() / RAND_MAX) * 2.0f - 1.0f; break;
        }
        m_lfoValue *= lfo_depth;

        float total = 0;
        for (int v = 0; v < polyphony; v++) {
            if (voices[v].isFinished()) continue;
            voices[v].osc1_shaper = (float)osc1_shaper;
            voices[v].osc1_skew = (float)osc1_skew;
            voices[v].osc1_basis = (float)osc1_basis;
            voices[v].osc2_shaper = (float)osc2_shaper;
            voices[v].osc2_skew = (float)osc2_skew;
            voices[v].osc2_basis = (float)osc2_basis;
            voices[v].mix = (float)osc12_mix;
            voices[v].detune = (float)osc1_detune;
            voices[v].volume = (float)amp;
            voices[v].resoVol = (float)reso_vol;
            voices[v].amp_attack = (float)adsr_env0_attack;
            voices[v].amp_decay = (float)adsr_env0_decay;
            voices[v].amp_sustain = (float)adsr_env0_sustain;
            voices[v].amp_release = (float)adsr_env0_release;
            voices[v].flt_attack = (float)adsr_env1_attack;
            voices[v].flt_decay = (float)adsr_env1_decay;
            voices[v].flt_sustain = (float)adsr_env1_sustain;
            voices[v].flt_release = (float)adsr_env1_release;
            voices[v].flt_env_depth = (float)env1_depth;
            voices[v].cutoff = (float)filter1_cutoff;
            voices[v].resonance = (float)filter1_resonance;

            total += voices[v].process(m_lfoValue);
        }

        m_buffer[s] = total;
    }

    for (int v = 0; v < polyphony; v++) {
        if (voices[v].isFinished() && voices[v].keyon == 0) {
            voices[v].note = -1;
        }
    }
}

void DigitsEngine::setI(int index, int value)
{
    switch (index) {
    case OSC1_TYPE: osc1_type = value; break;
    case OSC2_TYPE: osc2_type = value; break;
    case OSC1_SHAPER: osc1_shaper = value; break;
    case OSC1_SKEW: osc1_skew = value; break;
    case OSC1_BASIS: osc1_basis = value; break;
    case OSC2_SHAPER: osc2_shaper = value; break;
    case OSC2_SKEW: osc2_skew = value; break;
    case OSC2_BASIS: osc2_basis = value; break;
    case OSC12_MIX: osc12_mix = value; break;
    case OSC1_DETUNE: osc1_detune = value; break;
    case OSC2_DETUNE: osc2_detune = value; break;
    case OSC1_SCALE: osc1_scale = value; break;
    case OSC2_SCALE: osc2_scale = value; break;
    case OSC1_AMP: osc1_amp = value; break;
    case OSC2_AMP: osc2_amp = value; break;
    case FILTER1_TYPE: filter1_type = value; break;
    case FILTER1_CUTOFF: filter1_cutoff = value; break;
    case FILTER1_RESONANCE: filter1_resonance = value; break;
    case ADSR_ENV0_ATTACK: adsr_env0_attack = value; break;
    case ADSR_ENV0_DECAY: adsr_env0_decay = value; break;
    case ADSR_ENV0_SUSTAIN: adsr_env0_sustain = value; break;
    case ADSR_ENV0_RELEASE: adsr_env0_release = value; break;
    case ADSR_ENV1_ATTACK: adsr_env1_attack = value; break;
    case ADSR_ENV1_DECAY: adsr_env1_decay = value; break;
    case ADSR_ENV1_SUSTAIN: adsr_env1_sustain = value; break;
    case ADSR_ENV1_RELEASE: adsr_env1_release = value; break;
    case ENV1_DEPTH: env1_depth = value; break;
    case LFO1_FREQ: lfo1_freq = value; break;
    case LFO1_DEPTH: lfo1_depth = value; break;
    case LFO1_SHAPE: lfo1_shape = value; break;
    case AMP: amp = value; break;
    case RESO_VOL: reso_vol = value; break;
    case LFO_AMT_SHAPER: lfo_amt_shaper = value; break;
    case NOTE1: m_last_note = value; break;
    case NOTE_ON:
        if (value) noteOn(m_last_note, 100);
        else noteOff(m_last_note);
        break;
    }
}

int DigitsEngine::getI(int index) const
{
    switch (index) {
    case OSC1_TYPE: return osc1_type;
    case OSC2_TYPE: return osc2_type;
    case OSC1_SHAPER: return osc1_shaper;
    case OSC1_SKEW: return osc1_skew;
    case OSC1_BASIS: return osc1_basis;
    case OSC2_SHAPER: return osc2_shaper;
    case OSC2_SKEW: return osc2_skew;
    case OSC2_BASIS: return osc2_basis;
    case OSC12_MIX: return osc12_mix;
    case OSC1_DETUNE: return osc1_detune;
    case OSC2_DETUNE: return osc2_detune;
    case OSC1_SCALE: return osc1_scale;
    case OSC2_SCALE: return osc2_scale;
    case OSC1_AMP: return osc1_amp;
    case OSC2_AMP: return osc2_amp;
    case FILTER1_TYPE: return filter1_type;
    case FILTER1_CUTOFF: return filter1_cutoff;
    case FILTER1_RESONANCE: return filter1_resonance;
    case ADSR_ENV0_ATTACK: return adsr_env0_attack;
    case ADSR_ENV0_DECAY: return adsr_env0_decay;
    case ADSR_ENV0_SUSTAIN: return adsr_env0_sustain;
    case ADSR_ENV0_RELEASE: return adsr_env0_release;
    case ADSR_ENV1_ATTACK: return adsr_env1_attack;
    case ADSR_ENV1_DECAY: return adsr_env1_decay;
    case ADSR_ENV1_SUSTAIN: return adsr_env1_sustain;
    case ADSR_ENV1_RELEASE: return adsr_env1_release;
    case ENV1_DEPTH: return env1_depth;
    case LFO1_FREQ: return lfo1_freq;
    case LFO1_DEPTH: return lfo1_depth;
    case LFO1_SHAPE: return lfo1_shape;
    case AMP: return amp;
    case RESO_VOL: return reso_vol;
    case LFO_AMT_SHAPER: return lfo_amt_shaper;
    default: return 0;
    }
}

int DigitsEngine::getKeyOn() const
{
    return m_keyon;
}

int DigitsEngine::getLastNote() const
{
    return m_last_note;
}

void DigitsEngine::setVoiceParams(int v)
{
    DigitsVoice& voice = voices[v];
    voice.osc1_shaper = (float)osc1_shaper;
    voice.osc1_skew = (float)osc1_skew;
    voice.osc1_basis = (float)osc1_basis;
    voice.osc2_shaper = (float)osc2_shaper;
    voice.osc2_skew = (float)osc2_skew;
    voice.osc2_basis = (float)osc2_basis;
    voice.mix = (float)osc12_mix;
    voice.detune = (float)osc1_detune;
    voice.volume = (float)amp;
    voice.resoVol = (float)reso_vol;
    voice.amp_attack = (float)adsr_env0_attack;
    voice.amp_decay = (float)adsr_env0_decay;
    voice.amp_sustain = (float)adsr_env0_sustain;
    voice.amp_release = (float)adsr_env0_release;
    voice.flt_attack = (float)adsr_env1_attack;
    voice.flt_decay = (float)adsr_env1_decay;
    voice.flt_sustain = (float)adsr_env1_sustain;
    voice.flt_release = (float)adsr_env1_release;
    voice.flt_env_depth = (float)env1_depth;
    voice.cutoff = (float)filter1_cutoff;
    voice.resonance = (float)filter1_resonance;
}

int DigitsEngine::findFreeVoice(int midiNote)
{
    // Priority 1: inactive/finished voice (envelope done, silent)
    for (int i = 0; i < polyphony; i++)
        if (voices[i].isFinished())
            return i;

    // Priority 2: released voice (in release tail, fading out)
    for (int i = 0; i < polyphony; i++)
        if (!voices[i].isFinished() && voices[i].keyon == 0)
            return i;

    // Priority 3: steal the oldest voice (index 0)
    return 0;
}

int DigitsEngine::findVoiceForNote(int midiNote)
{
    for (int i = 0; i < polyphony; i++)
        if (voices[i].note == midiNote && voices[i].keyon)
            return i;
    return -1;
}

int DigitsEngine::stealVoice()
{
    // Not used anymore - findFreeVoice handles all cases
    return 0;
}
