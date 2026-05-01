#include "../Machine.h"
#include "pbsynth_types.h"
#include "synthengine.h"
#include "mytypes.h"
#include "fixed.h"
#include <vector>



#ifndef __PBSYNTHMACHINE____
#define __PBSYNTHMACHINE____

static const int MAX_VOICES = 8;

struct PBSynthVoice {
    SynthEngine* se;
    int note;
    int keyon;
    mfloat* buffer_f;
    Sint16* buffer_i;
    int index;
    int sample_num;
    int noteOnTime;

    PBSynthVoice() : se(nullptr), note(-1), keyon(0), buffer_f(nullptr), buffer_i(nullptr), index(0), sample_num(0), noteOnTime(0) {}
};

class PBSynthMachine : public Machine
{
 public:
   PBSynthMachine(int polyphony = MAX_VOICES);
   ~PBSynthMachine();

   void init();
   void reset();
   Sint32  tick();
   Fixed  tick_fixed();
   Fixed  * tick_fixed_buffer();

   void setI(int what,int   val);
   void setF(int what,float val);

   int getLastNote() const { return note; }
   int getKeyOn() const { return keyon; }
   int  getI(int what);

    int checkI(int what,int   val);
    const char * getMachineParamCharStar(int machineParam,int paramValue);

    void applyCC(int cc, float normalized, const std::string& paramName) override;

    void setPolyphony(int voices) { polyphony_ = voices; }

  protected:
   std::vector<PBSynthVoice> voices;
   int polyphony_;
   int currentVoice;

   int                   cutoff;
   int                   resonance;


  Sint32                sample_num;
  Sint32                last_sample;

  Sint16              * tanh_table;
  mfloat              * buffer_f;
  Sint16              * buffer_i;
  Fixed               * buffer_fix;
  int                   index;

  //int                   freq;
  float                 freq;
  int                   keyon;

  //SinePBSynthOscillator          sineLfoOsc1;
  //PBSynthOscillator          sineLfoOsc1;

  int lfo_depth;
  int lfo_depth_shift;

  float lfo_speed;

  int note;

  int                   trig_time_mode;
  int                   trig_time_duration;
  Sint32                trig_time_duration_sample;

  int                   osc1_type;
  int                   osc2_type;


  int                   osc1_scale;
  int                   osc2_scale;

  int                   osc1_detune;
  int                   osc2_detune;

};

#endif
  
