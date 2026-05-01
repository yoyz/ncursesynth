#ifndef MACHINE_UI_H
#define MACHINE_UI_H

#include <string>
#include <vector>
#include <ncurses.h>
#include "../machine/Machine.h"
#include "../machine/MachineManager.h"

class MidiInput;

struct MachineControl {
    std::string name;
    int param;
    int row;
    int col;
    float value;
    float minVal;
    float maxVal;
};

class MachineUI {
protected:
    int selectedControl;
    std::vector<MachineControl> controls;
    Machine* machine;
    MachineManager* machineManager;
    MidiInput* midiInput;
    int screenRows;
    int screenCols;

    int lastMidiNote;
    int lastMidiVel;
    bool midiActivity;

    int menuSelection;
    int engineIndex;
    int midiDeviceIndex;

    virtual void initControls() = 0;
    virtual void drawColumnHeader(int col, const char* title) = 0;
    virtual void drawControl(int index, bool selected) = 0;
    virtual void drawSlider(int row, int col, const char* name, float value, bool selected) = 0;
    virtual void updateControlValues() = 0;

    void drawMenuBar();
    void drawMidiMonitor();
    void handleNavigation(int ch);
    void handleValueChange(int ch);

public:
    MachineUI(Machine* mach, MachineManager* mgr = nullptr);
    virtual ~MachineUI();

    virtual void init();
    virtual void draw();
    virtual void handleInput(int ch);
    virtual void updateValues();
    virtual bool isActive() const { return true; }

    void setMidiNote(int note, int vel) { lastMidiNote = note; lastMidiVel = vel; midiActivity = true; }
    void setMidiInput(MidiInput* midi) { midiInput = midi; }
    void setEngineIndex(int idx) { engineIndex = idx; }
    void setMidiDeviceIndex(int idx) { midiDeviceIndex = idx; }
    int getEngineIndex() const { return engineIndex; }
    int getMidiDeviceIndex() const { return midiDeviceIndex; }

    virtual void setControlValue(int paramId, float value);
};

#endif