#include "midi_mapping.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <dirent.h>
#include <algorithm>
#include "machine/Ncursesynth/synth/synth_architecture.h"

MidiMapping::MidiMapping(const std::string& mappingName, const std::string& mappingFile)
    : name(mappingName), filename(mappingFile) {}

MappingManager::MappingManager(const std::string& dir)
    : currentMappingIndex(-1), mappingDir(dir) {}

MappingManager::~MappingManager() {}

MappingEntry MidiMapping::getEntry(int cc) const {
    auto it = mappings.find(cc);
    if (it != mappings.end()) {
        return it->second;
    }
    return MappingEntry{-1, "", 0, 127, 0};
}

bool MidiMapping::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open mapping file: " << path << std::endl;
        return false;
    }

    mappings.clear();
    bool valid = true;
    int lineNum = 0;

    std::string line;
    while (std::getline(file, line)) {
        lineNum++;
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        std::string ccStr, paramName, minStr, maxStr, defaultStr;

        if (std::getline(iss, ccStr, ',') &&
            std::getline(iss, paramName, ',') &&
            std::getline(iss, minStr, ',') &&
            std::getline(iss, maxStr, ',') &&
            std::getline(iss, defaultStr, ',')) {

            try {
                int cc = std::stoi(ccStr);
                float minVal = std::stof(minStr);
                float maxVal = std::stof(maxStr);
                float defaultVal = std::stof(defaultStr);

                if (cc < 0 || cc > 127) {
                    std::cerr << "  " << path << ":" << lineNum << " - CC " << cc << " out of range (0-127)" << std::endl;
                    valid = false;
                    continue;
                }

                mappings[cc] = {cc, paramName, minVal, maxVal, defaultVal};
            } catch (...) {
                std::cerr << "  " << path << ":" << lineNum << " - invalid number: '" << line << "'" << std::endl;
                valid = false;
            }
        } else {
            std::cerr << "  " << path << ":" << lineNum << " - bad format (expected: cc,param,min,max,default): '" << line << "'" << std::endl;
            valid = false;
        }
    }

    return valid;
}

void MappingManager::setMappingDirectory(const std::string& dir) {
    mappingDir = dir;
}

bool MappingManager::loadMappings() {
    availableMappings.clear();
    currentMappingIndex = -1;

    std::string dirPath = mappingDir;
    if (mappingDir.empty()) {
        dirPath = "mapping";
    }

    std::vector<std::string> files;

    // Scan directory for .txt files
    DIR* dir = opendir(dirPath.c_str());
    if (dir == nullptr) {
        std::cerr << "Cannot open mapping directory: " << dirPath << std::endl;
        return false;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name.length() > 4 && name.substr(name.length() - 4) == ".txt") {
            if (name != "index.txt" && name != "parameters.txt") {
                files.push_back(name);
            }
        }
    }
    closedir(dir);

    // Sort alphabetically for consistent ordering
    std::sort(files.begin(), files.end());

    // Load each mapping file
    for (const auto& filename : files) {
        std::string fullPath = dirPath + "/" + filename;
        auto mapping = std::make_unique<MidiMapping>();
        std::string name = filename.substr(0, filename.length() - 4);
        mapping->setName(name);
        mapping->setFilename(filename);

        if (mapping->loadFromFile(fullPath)) {
            if (mapping->hasAnyMappings()) {
                availableMappings.push_back(std::move(mapping));
            }
        } else {
            std::cout << "  " << name << " - syntax errors, skipped" << std::endl;
        }
    }

    if (!availableMappings.empty()) {
        currentMappingIndex = 0;
    }

    return !availableMappings.empty();
}

// Debug override - prints loaded mappings
void MappingManager::printMappings() {
    for (size_t i = 0; i < availableMappings.size(); i++) {
        MidiMapping* m = availableMappings[i].get();
        std::cout << "Mapping " << i << ": " << m->getName() << " (" << m->getFilename() << ")" << std::endl;
        const auto& mappings = m->getMappings();
        for (const auto& entry : mappings) {
            std::cout << "  CC " << entry.first << " -> " << entry.second.parameterName << std::endl;
        }
    }
}

void MappingManager::setCurrentMapping(int index) {
    if (index >= 0 && index < static_cast<int>(availableMappings.size())) {
        currentMappingIndex = index;
    }
}

MidiMapping* MappingManager::getCurrentMapping() {
    return getMapping(currentMappingIndex);
}

MidiMapping* MappingManager::getMapping(int index) {
    if (index >= 0 && index < static_cast<int>(availableMappings.size())) {
        return availableMappings[index].get();
    }
    return nullptr;
}

std::string MappingManager::getMappingName(int index) const {
    if (index >= 0 && index < static_cast<int>(availableMappings.size())) {
        return availableMappings[index]->getName();
    }
    return "None";
}

MappingEntry MappingManager::getMappingEntry(int cc) {
    MidiMapping* mapping = getCurrentMapping();
    if (!mapping) return MappingEntry{-1, "", 0, 127, 0};
    if (!mapping->hasCC(cc)) return MappingEntry{-1, "", 0, 127, 0};
    return mapping->getEntry(cc);
}

bool MappingManager::applyMapping(SynthArchitecture* synth, int cc, float value) {
    if (!synth || currentMappingIndex < 0) return false;

    MidiMapping* mapping = getCurrentMapping();
    if (!mapping) return false;

    if (!mapping->hasCC(cc)) return false;

    MappingEntry entry = mapping->getEntry(cc);
    float range = entry.maxValue - entry.minValue;
    if (range == 0.0f) return false;
    float normalized = (value - entry.minValue) / range;
    normalized = std::max(0.0f, std::min(1.0f, normalized));

    std::string param = entry.parameterName;

    if (param == "CUTOFF") {
        synth->setCutoff(20.0f + normalized * 7980.0f);
    } else if (param == "RESONANCE") {
        synth->setResonance(normalized);
    } else if (param == "FILTER_ENV_AMOUNT") {
        synth->setFilterEnvelopeAmount(normalized);
    } else if (param == "AMP_ATTACK") {
        synth->setAmpAttack(normalized);
    } else if (param == "AMP_DECAY") {
        synth->setAmpDecay(normalized);
    } else if (param == "AMP_SUSTAIN") {
        synth->setAmpSustain(normalized);
    } else if (param == "AMP_RELEASE") {
        synth->setAmpRelease(normalized);
    } else if (param == "FILTER_ATTACK") {
        synth->setFilterAttack(normalized);
    } else if (param == "FILTER_DECAY") {
        synth->setFilterDecay(normalized);
    } else if (param == "FILTER_SUSTAIN") {
        synth->setFilterSustain(normalized);
    } else if (param == "FILTER_RELEASE") {
        synth->setFilterRelease(normalized);
    } else if (param == "VOLUME") {
        synth->setVolume(normalized);
    } else if (param == "HPF_FREQ") {
        synth->setHPFCutoff(20.0f + normalized * 4980.0f);
    } else {
        return false;
    }

    return true;
}