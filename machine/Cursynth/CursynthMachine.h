#include "../Machine.h"
#include "cursynth_engine.h"

#ifndef __CURSYNTHMACHINE____
#define __CURSYNTHMACHINE____

#define DPRINTF(...)

#include <cstdint>
#include "cursynth_types.h"
#include <vector>

#define Sint32 int32_t
#define Sint16 int16_t
#define Uint32 uint32_t
#define Uint16 uint16_t

static const int MAX_CURSYNTH_VOICES = 8;

class CursynthMachine : public Machine
{
 public:
   CursynthMachine(int polyphony = MAX_CURSYNTH_VOICES);
   ~CursynthMachine();

   void init();
   void reset();
   Sint32 tick();

   void setI(int what,int val);
   void setF(int what,float val);
   int  getI(int what);
   int  checkI(int what,int val);

 const char * getMachineParamCharStar(int machineParam,int paramValue);
const char* getDisplayString(int index) override;

  std::vector<std::pair<std::string, int>> getPresetParams() const override;

  int getLastNote() const { return note; }
  int getKeyOn() const { return note_on; }

 void applyCC(int cc, float normalized, const std::string& paramName) override;
  void noteOn() override;
  void noteOff() override;

  protected:
    int polyphony_;

   mopocursynth::CursynthEngine * engine;

   int                   cutoff;
   int                   resonance;

   Sint16              * buffer_i;
   mopocursynth::mopo_float    * buffer_f;

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
    int                   env1_depth;
    int                   filter1_type;

    int                   adsr_env0_attack;
    int                   adsr_env0_decay;
    int                   adsr_env0_sustain;
    int                   adsr_env0_release;
    int                   adsr_env1_attack;
    int                   adsr_env1_decay;
    int                   adsr_env1_sustain;
    int                   adsr_env1_release;
    int                   osc12_mix;
    int                   amp_volume;
    int                   lfo1_freq;
    int                   lfo2_freq;
    int                   lfo1_depth;
    int                   lfo2_depth;

};

#endif
  
