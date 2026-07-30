#ifndef DIGITS_UI_H
#define DIGITS_UI_H

#include "machine_ui.h"

class DigitsUI : public MachineUI {
private:
    void initControls() override;
public:
    DigitsUI(Machine* mach, MachineManager* mgr = nullptr)
        : MachineUI(mach, mgr) {
        columnTitles[0] = "OSCILLATORS";
        columnTitles[1] = "FILTER";
        columnTitles[2] = "ENVELOPE / LFO";
        init();
    }
};

#endif
