#ifndef MACHINE_UI_H
#define MACHINE_UI_H

#include <string>
#include <vector>
#include <map>
#include "irenderer.h"
#include "widget.h"
#include "../machine/Machine.h"
#include "../machine/MachineManager.h"
#include "../midi/midi_mapping.h"

struct PresetInfo {
    std::string name;
    std::string path;
};

class MidiInput;

class MachineUI {
protected:
    int selectedControl;
    std::vector<Widget> widgets;
    Machine* machine;
    MachineManager* machineManager;
    MidiInput* midiInput;
    IRenderer* renderer;
    int screenRows;
    int screenCols;

    int lastMidiNote;
    int lastMidiVel;
    bool midiActivity;

    bool pcKeyboardMode;
    int pcOctave;
    std::map<int, bool> pcActiveKeys;

    int menuSelection;
    int menuIndex;
    int midiDeviceIndex;
    int mappingIndex;
    int presetIndex;
    std::vector<PresetInfo> presets;
    bool presetInputMode;
    std::string presetInputBuffer;

    std::string statusMessage;
    int statusTimer;

    std::string columnTitles[3];

    static constexpr int HEADER_ROW = 6;
    static constexpr int CONTROL_ROW_OFFSET = 7;
    static constexpr int CONTROL_BAR_LEN = 16;

    virtual void initControls() = 0;
    virtual void drawColumnHeader(int col, const char* title);
    virtual void updateControlValues();

    void drawMenuBar();
    void drawMidiMonitor();
    void drawLevelMeter();
    void handleNavigation(int ch);

public:
    MachineUI(Machine* mach, MachineManager* mgr = nullptr);
    virtual ~MachineUI();

    virtual void init();
    virtual void draw();
    virtual void handleInput(int ch);
    virtual void updateValues();
    virtual bool isActive() const { return true; }
    virtual void stop();

    void setRenderer(IRenderer* r) { renderer = r; }
    IRenderer* getRenderer() const { return renderer; }

    void setMidiNote(int note, int vel) { lastMidiNote = note; lastMidiVel = vel; midiActivity = true; }
    void setMidiInput(MidiInput* midi) { midiInput = midi; }
    void setMenuIndex(int idx) { menuIndex = idx; }
    void setMenuSelection(int sel) { menuSelection = sel; }
    void setMidiDeviceIndex(int idx) { midiDeviceIndex = idx; }
    void setMappingIndex(int idx) { mappingIndex = idx; }
    void setPcKeyboardMode(bool on) { pcKeyboardMode = on; pcOctave = 4; pcActiveKeys.clear(); }
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

    void setControlValue(int paramId, float value);

    const std::vector<Widget>& getWidgets() const { return widgets; }
    int getControlCount() const { return (int)widgets.size(); }
    const Widget& getControl(int index) const { return widgets[index]; }
};

#endif
