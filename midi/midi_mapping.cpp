#include "midi_mapping.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "../synth/synth_architecture.h"

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

    std::string line;
    while (std::getline(file, line)) {
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

                mappings[cc] = {cc, paramName, minVal, maxVal, defaultVal};
            } catch (...) {
                continue;
            }
        }
    }

    return true;
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

    std::ifstream indexFile(dirPath + "/index.txt");
    if (!indexFile.is_open()) {
        std::cerr << "No mapping index file found at: " << dirPath << "/index.txt" << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(indexFile, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        std::string name, filename;

        if (std::getline(iss, name, '=') && std::getline(iss, filename)) {
            auto mapping = std::make_unique<MidiMapping>();
            mapping->setName(name);
            mapping->setFilename(filename);
            if (mapping->loadFromFile(dirPath + "/" + filename)) {
                availableMappings.push_back(std::move(mapping));
            }
        }
    }

    if (!availableMappings.empty()) {
        currentMappingIndex = 0;
    }

    return !availableMappings.empty();
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
    float normalized = (value - entry.minValue) / (entry.maxValue - entry.minValue);
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