#ifndef CURSYNTH_UI_H
#define CURSYNTH_UI_H

#include "machine_ui.h"
#include "../machine/Cursynth/CursynthMachine.h"

class CursynthUI : public MachineUI {
private:
    void initControls() override;

public:
    CursynthUI(Machine* mach, MachineManager* mgr = nullptr);
    ~CursynthUI();

    using MachineUI::setMidiNote;
    using MachineUI::setControlValue;
};

#endif
