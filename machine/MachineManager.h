#ifndef MACHINE_MANAGER_H
#define MACHINE_MANAGER_H

#include <vector>
#include <memory>
#include <string>

class Machine;

class MachineManager {
private:
    std::vector<std::unique_ptr<Machine>> machines_;
    int currentMachine_;
    std::string engineDirectory_;

public:
    MachineManager(const std::string& dir = "machine");
    ~MachineManager();

    void setEngineDirectory(const std::string& dir);
    void registerMachine(Machine* machine);
    void setCurrentMachine(int index);
    int getCurrentMachine() const { return currentMachine_; }
    int getMachineCount() const { return machines_.size(); }

    Machine* getMachine(int index);
    Machine* getCurrentMachine();
    const std::string& getMachineName(int index) const;
    int findMachineByName(const std::string& name) const;

    bool loadMachine(const std::string& name);
    void unloadMachine(int index);
    void unloadAll();
};

#endif