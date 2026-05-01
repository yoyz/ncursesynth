#ifndef PRESET_MANAGER_H
#define PRESET_MANAGER_H

#include <string>
#include <vector>
#include <map>

class SynthArchitecture;

struct Preset {
    std::string name;
    std::string filename;
    std::map<std::string, std::string> params;
};

class PresetManager {
private:
    std::vector<Preset> presets;
    int currentPresetIndex;
    std::string bankPath;
    
public:
    PresetManager(const std::string& bankDir = "bank");
    ~PresetManager();
    
    void setBankPath(const std::string& path);
    bool loadBank();
    bool loadPreset(int index, SynthArchitecture* synth);
    bool savePreset(int index, SynthArchitecture* synth);
    bool saveCurrentAsNew(const std::string& name, SynthArchitecture* synth);
    
    int getPresetCount() const { return presets.size(); }
    int getCurrentPresetIndex() const { return currentPresetIndex; }
    std::string getPresetName(int index) const;
    std::string getPresetFile(int index) const;
    void setCurrentPreset(int index);
    
    bool exists() const { return !presets.empty(); }
};

#endif