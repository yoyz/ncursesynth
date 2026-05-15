#ifndef PBSYNTH_UI_H
#define PBSYNTH_UI_H

#include "machine_ui.h"

class PBSynthUI : public MachineUI {
private:
    void initControls() override;

public:
    PBSynthUI(Machine* mach, MachineManager* mgr = nullptr);
    ~PBSynthUI();

    using MachineUI::setMidiNote;
    using MachineUI::setControlValue;
};

#endif
