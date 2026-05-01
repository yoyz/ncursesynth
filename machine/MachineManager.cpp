#include "MachineManager.h"
#include "Machine.h"
#include <iostream>

MachineManager::MachineManager(const std::string& dir)
    : currentMachine_(-1), engineDirectory_(dir) {}

MachineManager::~MachineManager() {}

void MachineManager::setEngineDirectory(const std::string& dir) {
    engineDirectory_ = dir;
}

void MachineManager::registerMachine(Machine* machine) {
    if (machine) {
        machines_.push_back(std::unique_ptr<Machine>(machine));
        if (currentMachine_ == -1) {
            currentMachine_ = 0;
        }
    }
}

void MachineManager::setCurrentMachine(int index) {
    if (index >= 0 && index < static_cast<int>(machines_.size())) {
        currentMachine_ = index;
    }
}

Machine* MachineManager::getMachine(int index) {
    if (index >= 0 && index < static_cast<int>(machines_.size())) {
        return machines_[index].get();
    }
    return nullptr;
}

Machine* MachineManager::getCurrentMachine() {
    if (currentMachine_ >= 0 && currentMachine_ < static_cast<int>(machines_.size())) {
        return machines_[currentMachine_].get();
    }
    return nullptr;
}

const std::string& MachineManager::getMachineName(int index) const {
    static std::string empty = "";
    if (index >= 0 && index < static_cast<int>(machines_.size())) {
        return machines_[index]->getName();
    }
    return empty;
}

int MachineManager::findMachineByName(const std::string& name) const {
    for (int i = 0; i < static_cast<int>(machines_.size()); i++) {
        if (machines_[i]->getName() == name) {
            return i;
        }
    }
    return -1;
}

bool MachineManager::loadMachine(const std::string& name) {
    (void)name;
    return false;
}

void MachineManager::unloadMachine(int index) {
    if (index >= 0 && index < static_cast<int>(machines_.size())) {
        machines_.erase(machines_.begin() + index);
        if (currentMachine_ >= static_cast<int>(machines_.size())) {
            currentMachine_ = machines_.size() - 1;
        }
    }
}

void MachineManager::unloadAll() {
    machines_.clear();
    currentMachine_ = -1;
}