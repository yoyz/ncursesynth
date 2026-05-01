#ifndef NCURSESYNTH_MACHINE_H
#define NCURSESYNTH_MACHINE_H

#include "../synth/synth_architecture.h"
#include "Machine.h"
#include "Parameter.h"

class NcursesynthMachine : public Machine {
private:
    SynthArchitecture* synth_;
    int params_[MachineParam::PERFORMANCE_COUNT];
    int noteOn_;

public:
    NcursesynthMachine();
    virtual ~NcursesynthMachine();

    void setSynth(SynthArchitecture* synth) { synth_ = synth; }
    SynthArchitecture* getSynth() { return synth_; }

    virtual void init() override;
    virtual void reset() override;
    virtual int32_t tick() override;
    virtual void noteOn() override;
    virtual void noteOff() override;

    virtual void setI(int index, int value) override;
    virtual void setF(int index, float value) override;
    virtual int getI(int index) override;
    virtual float getF(int index) override;
    virtual int checkI(int index, int value) override;
    virtual const char* getDisplayString(int index) override;
};

#endif