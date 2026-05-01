#ifndef MIDI_MAPPING_H
#define MIDI_MAPPING_H

#include <string>
#include <vector>
#include <map>
#include <memory>

class SynthArchitecture;

struct MappingEntry {
    int cc;
    std::string parameterName;
    float minValue;
    float maxValue;
    float defaultValue;
};

class MidiMapping {
private:
    std::string name;
    std::string filename;
    std::map<int, MappingEntry> mappings;

public:
    MidiMapping() = default;
    MidiMapping(const std::string& mappingName, const std::string& mappingFile);
    ~MidiMapping() = default;

    void setName(const std::string& n) { name = n; }
    void setFilename(const std::string& f) { filename = f; }
    const std::string& getName() const { return name; }
    const std::string& getFilename() const { return filename; }
    const std::map<int, MappingEntry>& getMappings() const { return mappings; }
    std::map<int, MappingEntry>& getMappings() { return mappings; }

    bool hasCC(int cc) const { return mappings.find(cc) != mappings.end(); }
    MappingEntry getEntry(int cc) const;

    bool loadFromFile(const std::string& path);
};

class MappingManager {
private:
    std::vector<std::unique_ptr<MidiMapping>> availableMappings;
    int currentMappingIndex;
    std::string mappingDir;

public:
    MappingManager(const std::string& dir = "mapping");
    ~MappingManager();

    void setMappingDirectory(const std::string& dir);
    bool loadMappings();

    int getMappingCount() const { return availableMappings.size(); }
    int getCurrentMappingIndex() const { return currentMappingIndex; }
    void setCurrentMapping(int index);

    MidiMapping* getCurrentMapping();
    MidiMapping* getMapping(int index);
    std::string getMappingName(int index) const;

    MappingEntry getMappingEntry(int cc);

    bool applyMapping(SynthArchitecture* synth, int cc, float value);
};

#endif