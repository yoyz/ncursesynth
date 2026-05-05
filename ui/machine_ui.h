#ifndef MACHINE_UI_H
#define MACHINE_UI_H

#include <string>
#include <vector>
#include <ncurses.h>
#include "../machine/Machine.h"
#include "../machine/MachineManager.h"
#include "../midi/midi_mapping.h"

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
    int menuIndex;
    int midiDeviceIndex;
    int mappingIndex;

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
    virtual void stop();

    void setMidiNote(int note, int vel) { lastMidiNote = note; lastMidiVel = vel; midiActivity = true; }
    void setMidiInput(MidiInput* midi) { midiInput = midi; }
    void setMenuIndex(int idx) { menuIndex = idx; }
    void setMenuSelection(int sel) { menuSelection = sel; }
    void setMidiDeviceIndex(int idx) { midiDeviceIndex = idx; }
    void setMappingIndex(int idx) { mappingIndex = idx; }
    int getMenuIndex() const { return menuIndex; }
    int getMidiDeviceIndex() const { return midiDeviceIndex; }
    int getMappingIndex() const { return mappingIndex; }
    int getMenuSelection() const { return menuSelection; }
    class MappingManager* getMappingManager();

    virtual void setControlValue(int paramId, float value);
};

#endif