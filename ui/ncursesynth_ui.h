#ifndef NCURSESYNTH_UI_H
#define NCURSESYNTH_UI_H

#include "machine_ui.h"
#include "../machine/NcursesynthMachine.h"
#include "../machine/ParamID.h"

class NcursesynthUI : public MachineUI {
private:
    void initControls() override;

public:
    NcursesynthUI(Machine* mach, MachineManager* mgr = nullptr);
    ~NcursesynthUI();

    using MachineUI::setMidiNote;
    using MachineUI::setControlValue;
};

#endif
