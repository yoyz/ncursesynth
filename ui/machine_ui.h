#ifndef MACHINE_UI_H
#define MACHINE_UI_H

#include <string>
#include <vector>
#include <ncurses.h>
#include "../machine/Machine.h"
#include "../machine/MachineManager.h"
#include "../midi/midi_mapping.h"

struct PresetInfo {
    std::string name;
    std::string path;
};

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
    int presetIndex;
    std::vector<PresetInfo> presets;
    bool presetInputMode;
    std::string presetInputBuffer;

    std::string columnTitles[3];

    static constexpr int HEADER_ROW = 6;
    static constexpr int CONTROL_ROW_OFFSET = 7;
    static constexpr int CONTROL_BAR_LEN = 16;

    virtual void initControls() = 0;
    virtual void drawColumnHeader(int col, const char* title);
    virtual void drawControl(int index, bool selected);
    virtual void drawSlider(int row, int col, const char* name, float value, bool selected);
    virtual void updateControlValues();

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

    void scanPresets();
    bool loadPreset(int index);
    bool savePreset(const std::string& name);
    int getPresetIndex() const { return presetIndex; }
    void setPresetIndex(int idx) { presetIndex = idx; }
    const std::string& getCurrentPresetName() const;
    class MappingManager* getMappingManager();

    virtual void setControlValue(int paramId, float value);

    const std::vector<MachineControl>& getControls() const { return controls; }
};

#endif