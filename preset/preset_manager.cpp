#include "preset_manager.h"
#include "../synth/synth_architecture.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

PresetManager::PresetManager(const std::string& bankDir) 
    : currentPresetIndex(-1), bankPath(bankDir) {}

PresetManager::~PresetManager() {}

void PresetManager::setBankPath(const std::string& path) {
    bankPath = path;
    loadBank();
}

bool PresetManager::loadBank() {
    presets.clear();
    currentPresetIndex = -1;
    
    std::string indexFile = bankPath + "/index.txt";
    std::ifstream infile(indexFile.c_str());
    
    if (!infile.is_open()) {
        std::cerr << "Preset bank not found: " << indexFile << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(infile, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        std::istringstream iss(line);
        std::string filename, name;
        if (std::getline(iss, filename, ':') && std::getline(iss, name)) {
            Preset p;
            p.filename = bankPath + "/" + filename;
            p.name = name;
            
            std::ifstream pfile(p.filename.c_str());
            if (pfile.is_open()) {
                std::string paramLine;
                while (std::getline(pfile, paramLine)) {
                    if (paramLine.empty() || paramLine[0] == '#') continue;
                    size_t eq = paramLine.find('=');
                    if (eq != std::string::npos) {
                        std::string key = paramLine.substr(0, eq);
                        std::string value = paramLine.substr(eq + 1);
                        p.params[key] = value;
                    }
                }
                pfile.close();
            }
            
            presets.push_back(p);
        }
    }
    
    infile.close();
    
    if (!presets.empty()) {
        currentPresetIndex = 0;
    }
    
    return !presets.empty();
}

bool PresetManager::loadPreset(int index, SynthArchitecture* synth) {
    if (index < 0 || index >= (int)presets.size()) return false;
    
    const Preset& p = presets[index];
    
    auto getParam = [&p](const std::string& key, float defaultVal) -> float {
        if (p.params.find(key) != p.params.end()) {
            return std::stof(p.params.at(key));
        }
        return defaultVal;
    };
    
    auto getParamInt = [&p](const std::string& key, int defaultVal) -> int {
        if (p.params.find(key) != p.params.end()) {
            return std::stoi(p.params.at(key));
        }
        return defaultVal;
    };
    
    synth->setPolyphony(getParamInt("polyphony", 4));
    synth->setFilterType((FilterType)getParamInt("filter_type", 0));
    synth->setCutoff(getParam("cutoff", 1000.0f));
    synth->setResonance(getParam("resonance", 0.5f));
    synth->setFilterEnvelopeAmount(getParam("filter_env_amount", 0.5f));
    synth->setWaveform((Waveform)getParamInt("waveform", 0));
    synth->setOscMix(getParam("osc_mix", 0.5f));
    synth->setOsc2Detune(getParam("osc2_detune", 0.0f));
    synth->setAmpAttack(getParam("amp_attack", 0.01f));
    synth->setAmpDecay(getParam("amp_decay", 0.2f));
    synth->setAmpSustain(getParam("amp_sustain", 0.7f));
    synth->setAmpRelease(getParam("amp_release", 0.3f));
    synth->setAmpEnvelopeCurve((EnvelopeCurve)getParamInt("amp_env_curve", 1));
    synth->setFilterAttack(getParam("filter_attack", 0.05f));
    synth->setFilterDecay(getParam("filter_decay", 0.3f));
    synth->setFilterSustain(getParam("filter_sustain", 0.3f));
    synth->setFilterRelease(getParam("filter_release", 0.5f));
    synth->setFilterEnvelopeCurve((EnvelopeCurve)getParamInt("filter_env_curve", 1));
    synth->setVolume(getParam("volume", 0.3f));
    
    synth->getDelay()->setEnabled(getParamInt("delay_enable", 1));
    synth->getDelay()->setDelayTime(getParam("delay_time", 250.0f));
    synth->getDelay()->setFeedback(getParam("delay_feedback", 0.4f));
    synth->getDelay()->setMix(getParam("delay_mix", 0.3f));
    
    synth->getReverb()->setEnabled(getParamInt("reverb_enable", 1));
    synth->getReverb()->setDecay(getParam("reverb_decay", 0.5f));
    synth->getReverb()->setMix(getParam("reverb_mix", 0.3f));
    
    synth->getChorus()->setEnabled(getParamInt("chorus_enable", 1));
    synth->getChorus()->setDepth(getParam("chorus_depth", 0.5f));
    synth->getChorus()->setRate(getParam("chorus_rate", 0.5f));
    synth->getChorus()->setMix(getParam("chorus_mix", 0.3f));
    
    synth->getDistortion()->setEnabled(getParamInt("distortion_enable", 0));
    synth->getDistortion()->setDrive(getParam("distortion_drive", 0.0f));
    synth->getDistortion()->setMix(getParam("distortion_mix", 0.0f));
    
    currentPresetIndex = index;
    return true;
}

bool PresetManager::savePreset(int index, SynthArchitecture* synth) {
    if (index < 0 || index >= (int)presets.size()) return false;
    
    Preset& p = presets[index];
    p.params.clear();
    
    auto addParam = [&p](const std::string& key, float value) {
        std::ostringstream oss;
        oss << value;
        p.params[key] = oss.str();
    };
    
    addParam("polyphony", (float)synth->getPolyphony());
    addParam("filter_type", (int)synth->getCurrentFilterType());
    addParam("cutoff", synth->getCutoff());
    addParam("resonance", synth->getResonance());
    addParam("filter_env_amount", synth->getFilterEnvelopeAmount());
    addParam("waveform", (int)synth->getWaveform());
    addParam("osc_mix", synth->getOscMix());
    addParam("osc2_detune", synth->getOsc2Detune());
    addParam("amp_attack", synth->getAmpAttack());
    addParam("amp_decay", synth->getAmpDecay());
    addParam("amp_sustain", synth->getAmpSustain());
    addParam("amp_release", synth->getAmpRelease());
    addParam("amp_env_curve", (int)synth->getAmpEnvelopeCurve());
    addParam("filter_attack", synth->getFilterAttack());
    addParam("filter_decay", synth->getFilterDecay());
    addParam("filter_sustain", synth->getFilterSustain());
    addParam("filter_release", synth->getFilterRelease());
    addParam("filter_env_curve", (int)synth->getFilterEnvelopeCurve());
    addParam("volume", synth->getVolume());
    
    addParam("delay_enable", synth->getDelay()->isEnabled() ? 1 : 0);
    addParam("delay_time", synth->getDelay()->getDelayTime());
    addParam("delay_feedback", synth->getDelay()->getFeedback());
    addParam("delay_mix", synth->getDelay()->getMix());
    
    addParam("reverb_enable", synth->getReverb()->isEnabled() ? 1 : 0);
    addParam("reverb_decay", synth->getReverb()->getDecay());
    addParam("reverb_mix", synth->getReverb()->getMix());
    
    addParam("chorus_enable", synth->getChorus()->isEnabled() ? 1 : 0);
    addParam("chorus_depth", synth->getChorus()->getDepth());
    addParam("chorus_rate", synth->getChorus()->getRate());
    addParam("chorus_mix", synth->getChorus()->getMix());
    
    addParam("distortion_enable", synth->getDistortion()->isEnabled() ? 1 : 0);
    addParam("distortion_drive", synth->getDistortion()->getDrive());
    addParam("distortion_mix", synth->getDistortion()->getMix());
    
    std::ofstream outfile(p.filename.c_str());
    if (!outfile.is_open()) return false;
    
    for (auto& kv : p.params) {
        outfile << kv.first << "=" << kv.second << std::endl;
    }
    
    outfile.close();
    return true;
}

std::string PresetManager::getPresetName(int index) const {
    if (index < 0 || index >= (int)presets.size()) return "";
    return presets[index].name;
}

std::string PresetManager::getPresetFile(int index) const {
    if (index < 0 || index >= (int)presets.size()) return "";
    return presets[index].filename;
}

bool PresetManager::saveCurrentAsNew(const std::string& name, SynthArchitecture* synth) {
    if (name.empty()) return false;
    
    // Create filename from name (replace spaces with underscores)
    std::string filename = name;
    for (char& c : filename) {
        if (c == ' ' || c == '-' || c == '(' || c == ')') c = '_';
        else c = std::tolower(c);
    }
    filename += ".txt";
    
    // Try multiple path possibilities
    std::string fullPath = bankPath + "/" + filename;
    printf("DEBUG: Initial fullPath = %s\n", fullPath.c_str());
    
    // Check if path exists, if not try alternatives
    
    // Check if path exists, if not try alternatives
    struct stat st;
    if (stat(fullPath.c_str(), &st) != 0) {
        // Try relative to current dir
        fullPath = "./bank/ncursesynth/" + filename;
        if (stat(fullPath.c_str(), &st) != 0) {
            // Try one directory up
            fullPath = "../bank/ncursesynth/" + filename;
            if (stat(fullPath.c_str(), &st) != 0) {
                // Use absolute from executable location
                char cwd[1024];
                if (getcwd(cwd, sizeof(cwd))) {
                    fullPath = std::string(cwd) + "/bank/ncursesynth/" + filename;
                }
            }
        }
    }
    
    printf("DEBUG: Trying path = %s\n", fullPath.c_str());
    
    // Update the global bankPath to match where we actually saved
    // Find the directory part of fullPath
    size_t dirSlash = fullPath.rfind("/");
    if (dirSlash != std::string::npos) {
        std::string savedDir = fullPath.substr(0, dirSlash);
        bankPath = savedDir;
    }
    
    printf("DEBUG: Final bankPath = %s\n", bankPath.c_str());
    
    // Save the preset to file
    std::map<std::string, std::string> params;
    
    auto addParam = [&params](const std::string& key, float value) {
        std::ostringstream oss;
        oss << value;
        params[key] = oss.str();
    };
    
    addParam("polyphony", (float)synth->getPolyphony());
    addParam("filter_type", (int)synth->getCurrentFilterType());
    addParam("cutoff", synth->getCutoff());
    addParam("resonance", synth->getResonance());
    addParam("filter_env_amount", synth->getFilterEnvelopeAmount());
    addParam("waveform", (int)synth->getWaveform());
    addParam("osc_mix", synth->getOscMix());
    addParam("osc2_detune", synth->getOsc2Detune());
    addParam("amp_attack", synth->getAmpAttack());
    addParam("amp_decay", synth->getAmpDecay());
    addParam("amp_sustain", synth->getAmpSustain());
    addParam("amp_release", synth->getAmpRelease());
    addParam("amp_env_curve", (int)synth->getAmpEnvelopeCurve());
    addParam("filter_attack", synth->getFilterAttack());
    addParam("filter_decay", synth->getFilterDecay());
    addParam("filter_sustain", synth->getFilterSustain());
    addParam("filter_release", synth->getFilterRelease());
    addParam("filter_env_curve", (int)synth->getFilterEnvelopeCurve());
    addParam("volume", synth->getVolume());
    
    addParam("delay_enable", synth->getDelay()->isEnabled() ? 1 : 0);
    addParam("delay_time", synth->getDelay()->getDelayTime());
    addParam("delay_feedback", synth->getDelay()->getFeedback());
    addParam("delay_mix", synth->getDelay()->getMix());
    
    addParam("reverb_enable", synth->getReverb()->isEnabled() ? 1 : 0);
    addParam("reverb_decay", synth->getReverb()->getDecay());
    addParam("reverb_mix", synth->getReverb()->getMix());
    
    addParam("chorus_enable", synth->getChorus()->isEnabled() ? 1 : 0);
    addParam("chorus_depth", synth->getChorus()->getDepth());
    addParam("chorus_rate", synth->getChorus()->getRate());
    addParam("chorus_mix", synth->getChorus()->getMix());
    
    addParam("distortion_enable", synth->getDistortion()->isEnabled() ? 1 : 0);
    addParam("distortion_drive", synth->getDistortion()->getDrive());
    addParam("distortion_mix", synth->getDistortion()->getMix());
    
    std::ofstream outfile(fullPath.c_str());
    if (!outfile.is_open()) {
        std::cerr << "Failed to open file for writing: " << fullPath << std::endl;
        return false;
    }
    
    for (auto& kv : params) {
        outfile << kv.first << "=" << kv.second << std::endl;
    }
    outfile.close();
    
    // Find and select the newly saved preset
    for (int i = 0; i < (int)presets.size(); i++) {
        if (presets[i].name == name) {
            currentPresetIndex = i;
            break;
        }
    }
    
    // Update index file - use the directory containing the preset file
    size_t lastSlash = fullPath.rfind("/");
    if (lastSlash != std::string::npos) {
        std::string directory = fullPath.substr(0, lastSlash);
        std::string indexPath = directory + "/index.txt";
        std::ofstream indexFile(indexPath.c_str(), std::ios::app);
        if (indexFile.is_open()) {
            indexFile << filename << ":" << name << std::endl;
            indexFile.close();
        }
    }
    
    return true;
}

void PresetManager::setCurrentPreset(int index) {
    if (index >= 0 && index < (int)presets.size()) {
        currentPresetIndex = index;
    }
}