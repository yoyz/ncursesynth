#ifndef TWYTCH_UI_H
#define TWYTCH_UI_H

#include "machine_ui.h"
#include "../machine/Twytch/twytch_types.h"

// Twytch UI uses raw integer param IDs matching TwytchMachine::setI

class TwytchUI : public MachineUI {
private:
    void initControls() override;

public:
    TwytchUI(Machine* mach, MachineManager* mgr = nullptr);
    ~TwytchUI();

    using MachineUI::setMidiNote;
    using MachineUI::setControlValue;
};

#endif
