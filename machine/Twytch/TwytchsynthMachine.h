using namespace std;

#include "../Machine.h"
#include "twytchhelm_common.h"
#include "twytchhelm_helm_engine.h"
#include "twytchhelm_value_switch.h"
#include "twytch_types.h"

#ifndef __TWYTCHSYNTHMACHINE__
#define __TWYTCHSYNTHMACHINE__

#include <cstdint>
#define Sint32 int32_t
#define Sint16 int16_t
#define Uint32 uint32_t
#define Uint16 uint16_t

static const int MAX_TWYTCH_VOICES = 8;

class TwytchsynthMachine : public Machine
{
 public:
   TwytchsynthMachine();
   ~TwytchsynthMachine();

   void init();
   void reset();
   Sint32  tick();

   void setI(int what,int val);
   void setF(int what,float val);
   int  getI(int what);
   int  checkI(int what,int val);

   const char * getMachineParamCharStar(int machineParam,int paramValue);

   int getLastNote() const { return note; }
   int getKeyOn() const { return note_on; }

   void applyCC(int cc, float normalized, const std::string& paramName) override;

 protected:

   twytchhelmmopo::HelmEngine * engine;

   int                   cutoff;
   int                   resonance;

   Sint16              * buffer_i;
   twytchhelmmopo::mopo_float    * buffer_f;

   Sint32                sample_num;
   Sint32                index;
   Sint16                last_sample;

   Sint16              * tanh_table;

   int                   note;
   int                   old_note;
   int                   note_on;
   int                   detune;

   int                   trig_time_mode;
   int                   trig_time_duration;
   Sint32                trig_time_duration_sample;

   int                   osc1_type;
   int                   osc2_type;
   int                   osc1_detune;
   int                   osc2_detune;

   int                   osc1_scale;
   int                   osc2_scale;

   int                   velocity;

   float                 f_env1_amount;
   float                 f_env2_amount;

   int                   pole;
   int                   need_note_on;

   int                   bpm;
   int                   osc3_type;
   int                   lfo1_waveform;
   int                   lfo2_waveform;

   int                   adsr_env0_attack;
   int                   adsr_env0_decay;
   int                   adsr_env0_sustain;
   int                   adsr_env0_release;

   int                   adsr_env1_attack;
   int                   adsr_env1_decay;
   int                   adsr_env1_sustain;
   int                   adsr_env1_release;

   int                   lfo1_env_amount;
   int                   lfo2_env_amount;
   
   int                   osc12_mix;
   int                   osc1_mod;

   int                   osc1_amp;
   int                   osc2_amp;
   int                   osc3_amp;
   int                   osc4_amp;
   
   int                   osc1_unison;
   int                   osc2_unison;
   int                   osc1_unisondt;
   int                   osc2_unisondt;

   int                   filter1_type;
   int                   filter1_saturation;
   int                   filter1_feedback;
   int                   filter1_cutoff;
   int                   filter1_resonance;

   float                 lfo1_freq;
   float                 lfo2_freq;

};

#endif