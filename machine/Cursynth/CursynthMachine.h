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

  int getLastNote() const { return note; }
  int getKeyOn() const { return note_on; }

  void applyCC(int cc, float normalized, const std::string& paramName) override;

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

};

#endif
  
