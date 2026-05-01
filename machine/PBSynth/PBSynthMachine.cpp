#include "PBSynthMachine.h"
#include <iostream>
#define SAM 64

PBSynthMachine::PBSynthMachine(int polyphony)
    : polyphony_(polyphony), currentVoice(0), cutoff(125), resonance(10),
      lfo_depth(0), lfo_depth_shift(20), lfo_speed(0),
      trig_time_mode(0), trig_time_duration(0), trig_time_duration_sample(0)
{
    DPRINTF("PBSynthMachine::PBSynthMachine()");
    voices.resize(polyphony_);
    buffer_f = nullptr;
    buffer_i = nullptr;
    buffer_fix = nullptr;
}


PBSynthMachine::~PBSynthMachine()
{
    DPRINTF("PBSynthMachine::~PBSynthMachine()");
    for (auto& v : voices) {
        if (v.buffer_f) free(v.buffer_f);
        if (v.buffer_i) free(v.buffer_i);
        delete v.se;
    }
    if (buffer_f) free(buffer_f);
    if (buffer_i) free(buffer_i);
}


void PBSynthMachine::init()
{
    DPRINTF("PBSynthMachine::init()");

    if (buffer_f == nullptr)
        buffer_f = (mfloat*)malloc(sizeof(mfloat) * SAM);
    if (buffer_i == nullptr)
        buffer_i = (Sint16*)malloc(sizeof(Sint16) * SAM);
    if (buffer_fix == nullptr)
        buffer_fix = (Fixed*)malloc(sizeof(Fixed) * SAM);

    for (int i = 0; i < SAM; i++) {
        buffer_f[i] = 0;
        buffer_i[i] = 0;
        buffer_fix[i] = 0;
    }

    sample_num = 0;
    index = 0;

    for (auto& voice : voices) {
        if (voice.se == nullptr) {
            voice.se = new SynthEngine(SAM, 100);
        }
        if (voice.buffer_f == nullptr)
            voice.buffer_f = (mfloat*)malloc(sizeof(mfloat) * SAM);
        if (voice.buffer_i == nullptr)
            voice.buffer_i = (Sint16*)malloc(sizeof(Sint16) * SAM);

        for (int i = 0; i < SAM; i++) {
            voice.buffer_f[i] = 0;
            voice.buffer_i[i] = 0;
        }

        voice.se->init();
        voice.se->reset();
        voice.se->setPBSynthFilter24dB(1);

        voice.se->getEnvelope(0)->setADSR(0.2f, 0.2f, 1.0f, 0.2f);
        voice.se->getEnvelope(1)->setADSR(0.2f, 0.2f, 1.0f, 0.2f);

        voice.note = -1;
        voice.keyon = 0;
        voice.index = 0;
        voice.noteOnTime = sample_num;
        voice.sample_num = 0;
    }

    // Don't auto-trigger note at startup
    note = 60;
    freq = 440.0;
    keyon = 0;
}

const char * PBSynthMachine::getMachineParamCharStar(int machineParam,int paramValue)
{
  static const char * str_pbsynth_null       = " NULL";
  static const char * str_pbsynth_sqr        = "  SQR";
  static const char * str_pbsynth_trgl       = " TRGL";
  static const char * str_pbsynth_saw        = "  SAW";


  const char * str_osc[PICO_PBSYNTH_SIZE];

  static const char * str_fltr_algo_pblp = "PBLP";
  
  const        char * str_fltr_algo[PBSYNTH_FILTER_ALGO_SIZE];


  static const char * str_fltr_type_lp12   = "LP12";
  static const char * str_fltr_type_lp24   = "LP24";

  const        char * str_fltr_type[FILTER_TYPE_SIZE];

  static const char * str_fm_type_am      = "2OP_AM ";
  static const char * str_fm_type_fm      = "2OP_FM ";

  const        char * str_fm_type[FM_TYPE_SIZE];


  str_osc[PICO_PBSYNTH_SQUARE]        = str_pbsynth_sqr;
  str_osc[PICO_PBSYNTH_SAW]           = str_pbsynth_saw;
  str_osc[PICO_PBSYNTH_TRIANGLE]      = str_pbsynth_trgl;

  str_fltr_algo[PBSYNTH_FILTER_ALGO_PBLP]       = str_fltr_algo_pblp;

  str_fltr_type[PBSYNTH_FILTER_TYPE_LP12]       = str_fltr_type_lp12;

  str_fltr_type[PBSYNTH_FILTER_TYPE_LP12]       = str_fltr_type_lp12;
  str_fltr_type[PBSYNTH_FILTER_TYPE_LP24]       = str_fltr_type_lp24;

  str_fm_type[FM_TYPE_AM]             = str_fm_type_am;
  str_fm_type[FM_TYPE_FM]             = str_fm_type_fm;



  switch (machineParam)
    {
    case OSC1_TYPE:
      return str_osc[this->checkI(OSC1_TYPE,paramValue)];
    case OSC2_TYPE:
      return str_osc[this->checkI(OSC2_TYPE,paramValue)];

    case FILTER1_ALGO:
      return str_fltr_algo[this->checkI(FILTER1_ALGO,paramValue)];

    case FILTER1_TYPE:
      return str_fltr_type[this->checkI(FILTER1_TYPE,paramValue)];

    case FM_TYPE:
      return str_fm_type[paramValue];

    }
  return str_pbsynth_null;
}



void PBSynthMachine::reset()
{
 sample_num=0;
 keyon=0;

 trig_time_mode=0;
 trig_time_duration=0;
 trig_time_duration_sample=0;
}

int PBSynthMachine::checkI(int what,int val)
{
  switch (what)
    {
    case OSC1_TYPE:
      if (val<0)                  return 0;
      if (val>=PICO_PBSYNTH_SIZE) return PICO_PBSYNTH_SIZE-1;
      return val;
      break;

    case OSC2_TYPE:
      if (val<0)                  return 0;
      if (val>=PICO_PBSYNTH_SIZE) return PICO_PBSYNTH_SIZE-1;
      return val;
      break;


    case FILTER1_TYPE:
      if (val<0)                  return 0;
      if (val>=PBSYNTH_FILTER_TYPE_SIZE) return PBSYNTH_FILTER_TYPE_SIZE-1;
      return val;
      break;


    case FILTER1_ALGO:
      if (val<0)                  return 0;
      if (val>=PBSYNTH_FILTER_ALGO_SIZE) return PBSYNTH_FILTER_ALGO_SIZE-1;
      return val;
      break;


    default:
      if (val<0)   return 0;
      if (val>127) return 127;
      DPRINTF("WARNING: PBSynthMachine::checkI(%d,%d)\n",what,val);
      return val;
      break;      
    }
}


int PBSynthMachine::getI(int what)
{
  if (voices.empty() || voices[0].se == nullptr) return 0;

  SynthEngine* se = voices[0].se;

  if (what == NOTE_ON) return keyon;
  if (what == NOTE1) return note;
  if (what == OSC1_TYPE) return osc1_type;
  if (what == OSC2_TYPE) return osc2_type;
  if (what == OSC1_SCALE) return osc1_scale;
  if (what == OSC2_SCALE) return osc2_scale;
  if (what == OSC1_DETUNE) return osc1_detune;
  if (what == OSC2_DETUNE) return osc2_detune;
  if (what == ADSR_ENV0_ATTACK) {
      float a = se->getEnvelope(0)->getA();
      return (int)((a + 1.0f) / 2.0f * 127);
  }
  if (what == ADSR_ENV0_DECAY) {
      float d = se->getEnvelope(0)->getD();
      return (int)((d + 1.0f) / 2.0f * 127);
  }
  if (what == ADSR_ENV0_SUSTAIN) {
      float s = se->getEnvelope(0)->getS();
      return (int)((s + 1.0f) / 2.0f * 127);
  }
  if (what == ADSR_ENV0_RELEASE) {
      float r = se->getEnvelope(0)->getR();
      return (int)((r + 1.0f) / 2.0f * 127);
  }
  if (what == ADSR_ENV1_ATTACK) {
      float a = se->getEnvelope(1)->getA();
      return (int)((a + 1.0f) / 2.0f * 127);
  }
  if (what == ADSR_ENV1_DECAY) {
      float d = se->getEnvelope(1)->getD();
      return (int)((d + 1.0f) / 2.0f * 127);
  }
  if (what == ADSR_ENV1_SUSTAIN) {
      float s = se->getEnvelope(1)->getS();
      return (int)((s - 1.0f) / 2.0f * 127);
  }
  if (what == ADSR_ENV1_RELEASE) {
      float r = se->getEnvelope(1)->getR();
      return (int)((r + 1.0f) / 2.0f * 127);
  }
  if (what == FILTER1_CUTOFF) {
      float f = se->getParameter(SENGINE_FILTFREQ);
      return (int)((f + 1.0f) / 2.0f * 127);
  }
  if (what == FILTER1_RESONANCE) {
      float r = se->getParameter(SENGINE_FILTRESO);
      return (int)((r + 1.0f) / 2.0f * 127);
  }
  if (what == VCO_MIX) {
      float m = se->getParameter(SENGINE_OSCMIX);
      return (int)(m * 127);
  }
  if (what == LFO1_FREQ) {
      float f = se->getLFO(0)->getRate();
      return (int)(f * 127);
  }
  if (what == LFO1_DEPTH) {
      float d = se->getParameter(SENGINE_LFO1_TO_AMP);
      return (int)(d * 127);
  }
  if (what == LFO2_FREQ) {
      float f = se->getLFO(1)->getRate();
      return (int)(f * 127);
  }
  if (what == LFO2_DEPTH) {
      float d = se->getParameter(SENGINE_LFO2_TO_CUTOFF);
      return (int)((d + 0.5f) * 127);
  }
  if (what == ENV1_DEPTH) {
      float d = se->getParameter(SENGINE_ENV2_TO_CUTOFF);
      return (int)((d + 1.0f) / 2.0f * 127);
  }
  if (what == AMP) {
      float v = se->getParameter(SENGINE_AMPLEVEL);
      return (int)(v * 127);
  }
  return 0;
}

void PBSynthMachine::setF(int what,float val)
{
  float f_val=val;
  f_val=f_val/128;

  if (voices.empty() || voices[0].se == nullptr) return;

  // Apply to all voices
  for (auto& v : voices) {
      if (v.se == nullptr) continue;
      if (what==LFO1_FREQ)           v.se->getLFO(0)->setRate(f_val);
      if (what==LFO2_FREQ)           v.se->getLFO(1)->setRate(f_val);
  }

}



void PBSynthMachine::setI(int what,int val)
{
  if (voices.empty() || voices[0].se == nullptr) return;

  float        f_val_cutoff;
  float        f_val_resonance;
  float        f_val;
  int          noteShift=2;
  f_val=val;
  f_val=f_val/128.0;
  if (what==TRIG_TIME_MODE)       trig_time_mode=val;
  if (what==TRIG_TIME_DURATION) { trig_time_duration=val; trig_time_duration_sample=val*512; }

  // Use first voice as master for parameter setting
  SynthEngine* masterSE = voices[0].se;

    if (what==NOTE_ON && val==1)
    {
      if (midiDebug_) std::cerr << ">>> NoteOn MIDI_NOTE=" << note << std::endl;
      int targetVoice = -1;
      for (int i = 0; i < polyphony_; i++) {
          if (voices[i].keyon == 0) {
              targetVoice = i;
              break;
          }
      }
      
      if (targetVoice == -1) {
          int lowestNote = -1;
          for (int i = 0; i < polyphony_; i++) {
              if (lowestNote == -1 || voices[i].note < lowestNote) {
                  lowestNote = voices[i].note;
                  targetVoice = i;
              }
          }
          if (midiDebug_) std::cerr << "  STEAL voice " << targetVoice << std::endl;
          voices[targetVoice].se->reset();
      }

      PBSynthVoice& v = voices[targetVoice];
      v.index = 0;
      v.se->triggerNoteOsc(0, note + osc1_scale - noteShift);
      v.se->triggerNoteOsc(1, note + osc2_scale - noteShift);
      v.note = note;
      v.keyon = 1;

      if (midiDebug_) std::cerr << "  ASSIGN voice " << targetVoice << " -> note " << note << std::endl;

      keyon = 1;
    }

    if (what==NOTE_ON && val==0)
    {
      if (midiDebug_) std::cerr << ">>> NoteOff MIDI_NOTE=" << note << std::endl;
      int released = -1;
      for (int i = 0; i < polyphony_; i++) {
          if (voices[i].keyon && voices[i].note == this->note) {
              voices[i].se->releaseNote();
              released = i;
              if (midiDebug_) std::cerr << "  RELEASE voice " << i << std::endl;
              break;
          }
      }
      if (midiDebug_ && released == -1) {
          std::cerr << "  NO MATCHING VOICE FOUND for note " << this->note << "!" << std::endl;
      }

      bool anyActive = false;
      for (auto& v : voices) {
          if (v.keyon) {
              anyActive = true;
              break;
          }
      }
      if (!anyActive) keyon = 0;
    }

    if (what==OSC1_TYPE)           { osc1_type = val; for (auto& v : voices) if(v.se) v.se->getPBSynthOscillator(0)->setWave(this->checkI(OSC1_TYPE,val)); }
    if (what==OSC2_TYPE)           { osc2_type = val; for (auto& v : voices) if(v.se) v.se->getPBSynthOscillator(1)->setWave(this->checkI(OSC2_TYPE,val)); }
    if (what==OSC1_DETUNE)         { osc1_detune = val; for (auto& v : voices) if(v.se) v.se->getPBSynthOscillator(0)->setDetune(this->checkI(OSC1_DETUNE,val)+64); }
    if (what==OSC2_DETUNE)         { osc2_detune = val; for (auto& v : voices) if(v.se) v.se->getPBSynthOscillator(1)->setDetune(this->checkI(OSC2_DETUNE,val)+64); }
    if (what==OSC1_SCALE)          osc1_scale=val;
    if (what==OSC2_SCALE)          osc2_scale=val;


    if (what==ADSR_ENV0_ATTACK)    { for (auto& v : voices) if(v.se) v.se->getEnvelope(0)->setA(-1+f_val); }
    if (what==ADSR_ENV0_DECAY)     { for (auto& v : voices) if(v.se) v.se->getEnvelope(0)->setD(-1+f_val); }
    if (what==ADSR_ENV0_SUSTAIN)   { for (auto& v : voices) if(v.se) v.se->getEnvelope(0)->setS( f_val); }
    if (what==ADSR_ENV0_RELEASE)   { for (auto& v : voices) if(v.se) v.se->getEnvelope(0)->setR(-1+f_val); }


    if (what==ADSR_ENV1_ATTACK)    { for (auto& v : voices) if(v.se) v.se->getEnvelope(1)->setA(-1+f_val); }
    if (what==ADSR_ENV1_DECAY)     { for (auto& v : voices) if(v.se) v.se->getEnvelope(1)->setD(-1+f_val); }
    if (what==ADSR_ENV1_SUSTAIN)   { for (auto& v : voices) if(v.se) v.se->getEnvelope(1)->setS( 1+f_val); }
    if (what==ADSR_ENV1_RELEASE)   { for (auto& v : voices) if(v.se) v.se->getEnvelope(1)->setR(-1+f_val); }

    //if (what==VCO_MIX)             SE.setParameter(SENGINE_OSCMIX,1.0f/val);
    //if (what==VCO_MIX)             SE.setParameter(SENGINE_ENV2_TO_CUTOFF,1.0f/(val+1));
    //if (what==VCO_MIX)             SE.setParameter(SENGINE_ENV2_TO_CUTOFF,(f_val*2)-1);
    //if (what==OSC1_PHASE)          masterSE->setParameter(SENGINE_ENV2_TO_CUTOFF,(f_val*2)-1);


    if (what==VCO_MIX)             { for (auto& v : voices) if(v.se) v.se->setParameter(SENGINE_OSCMIX,(f_val)); }

    if (what==LFO1_DEPTH)          { for (auto& v : voices) if(v.se) v.se->setParameter(SENGINE_LFO1_TO_AMP,(f_val)); }

    if (what==LFO2_DEPTH)          { for (auto& v : voices) if(v.se) v.se->setParameter(SENGINE_LFO2_TO_CUTOFF,(f_val/2)); }

    if (what==NOTE1) {
        note=val;
        if (midiDebug_) std::cerr << "PBSynth: NOTE1 set to " << val << std::endl;
    }

    if (what==FILTER1_CUTOFF) {
      for (auto& v : voices) if(v.se) v.se->setParameter(SENGINE_FILTFREQ,(f_val*2)-1);
    }
    if (what==FILTER1_RESONANCE) {
      for (auto& v : voices) if(v.se) v.se->setParameter(SENGINE_FILTRESO,(f_val*2)-1);
    }

    if (what==ENV1_DEPTH)          { for (auto& v : voices) if(v.se) v.se->setParameter(SENGINE_ENV2_TO_CUTOFF,(f_val*2)-1); }

    if (what==LFO1_FREQ)           { for (auto& v : voices) if(v.se) v.se->getLFO(0)->setRate(f_val); }
    if (what==LFO2_FREQ)           { for (auto& v : voices) if(v.se) v.se->getLFO(1)->setRate(f_val); }

    if (what==OSC2_AMP)            { for (auto& v : voices) if(v.se) v.se->setParameter(SENGINE_ENV1_TO_OSC2PW,(f_val*2)-1); }

}

void PBSynthMachine::applyCC(int cc, float normalized, const std::string& paramName)
{
    int val = (int)(normalized * 127.0f);

    if (paramName == "CUTOFF") {
        setI(FILTER1_CUTOFF, val);
    } else if (paramName == "RESONANCE") {
        setI(FILTER1_RESONANCE, val);
    } else if (paramName == "FILTER_ENV_AMOUNT") {
        setI(ENV1_DEPTH, val);
    } else if (paramName == "FILTER_ATTACK") {
        setI(ADSR_ENV1_ATTACK, val);
    } else if (paramName == "FILTER_DECAY") {
        setI(ADSR_ENV1_DECAY, val);
    } else if (paramName == "FILTER_SUSTAIN") {
        setI(ADSR_ENV1_SUSTAIN, val);
    } else if (paramName == "FILTER_RELEASE") {
        setI(ADSR_ENV1_RELEASE, val);
    } else if (paramName == "AMP_ATTACK") {
        setI(ADSR_ENV0_ATTACK, val);
    } else if (paramName == "AMP_DECAY") {
        setI(ADSR_ENV0_DECAY, val);
    } else if (paramName == "AMP_SUSTAIN") {
        setI(ADSR_ENV0_SUSTAIN, val);
    } else if (paramName == "AMP_RELEASE") {
        setI(ADSR_ENV0_RELEASE, val);
    } else if (paramName == "VOLUME") {
        setI(AMP, val);
    } else if (paramName == "HPF_FREQ") {
    } else {
        if (midiDebug_) std::cerr << "CC " << cc << " param=" << paramName << " unmapped in PBSynth" << std::endl;
    }
}

Sint32 PBSynthMachine::tick()
{
    Sint32 total_output = 0;
    int voiceCount = 0;

    for (auto& voice : voices) {
        if (voice.keyon && voice.se != nullptr) {
            if (voice.index >= SAM || voice.index < 0)
                voice.index = 0;

            if (voice.index == 0) {
                voice.se->process(voice.buffer_f, SAM);
                for (int i = 0; i < SAM; i++) {
                    voice.buffer_i[i] = (Sint16)(voice.buffer_f[i] * 1280);
                }
            }

            Sint32 s = voice.buffer_i[voice.index];
            if (s > 32000) s = 32000;
            if (s < -32000) s = -32000;

            total_output += s;
            voiceCount++;

            voice.index++;
            voice.sample_num++;

            if (voice.se->isVoiceFinished()) {
                if (midiDebug_) std::cerr << "  VOICE " << &voice - &voices[0] << " finished release" << std::endl;
                voice.keyon = 0;
                voice.note = -1;
            }
        }
    }

    if (voiceCount > 0) {
        return total_output / voiceCount;
    }

    return 0;
}

Fixed PBSynthMachine::tick_fixed()
{
    // Not used in polyphony mode - use tick() instead
    return 0;
}

Fixed * PBSynthMachine::tick_fixed_buffer()
{
    return nullptr;
}


