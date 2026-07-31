#ifndef AMBIKA_UI_H
#define AMBIKA_UI_H

#include "machine_ui.h"

class AmbikaUI : public MachineUI {
private:
    void initControls() override;
public:
    AmbikaUI(Machine* mach, MachineManager* mgr = nullptr)
        : MachineUI(mach, mgr) {
        columnTitles[0] = "OSCILLATORS";
        columnTitles[1] = "FILTER / MIX";
        columnTitles[2] = "ENVELOPES";
        init();
    }
};

#endif
