#ifndef MIDI_INPUT_H
#define MIDI_INPUT_H

#include <rtmidi/RtMidi.h>
#include <memory>
#include <atomic>
#include <thread>
#include <vector>
#include <string>
#include "../synth/synth_architecture.h"
#include "midi_mapping.h"

class MidiInput {
private:
    std::unique_ptr<RtMidiIn> midiIn;
    std::atomic<bool> running;
    std::atomic<bool> initialized;
    std::thread midiThread;
    SynthArchitecture* synth;
    int selectedPort;
    MappingManager mappingManager;
    
    int lastCC;
    int lastCCValue;
    std::chrono::steady_clock::time_point lastActivity;
    
    void midiThreadFunc();
    void processMessage(const std::vector<unsigned char>& message);
    
public:
    MidiInput(SynthArchitecture* synthArch);
    ~MidiInput();
    
    bool initialize();
    void start();
    void stop();
    bool isRunning() const { return running.load(); }
    void listDevices() const;
    bool selectDevice(int port);
    int getDeviceCount() const;
    std::string getDeviceName(int port) const;
    int getSelectedPort() const { return selectedPort; }
    
    MappingManager* getMappingManager() { return &mappingManager; }
    bool loadMappings() { return mappingManager.loadMappings(); }
    
    int getLastCC() const { return lastCC; }
    int getLastCCValue() const { return lastCCValue; }
    auto getLastActivity() const { return lastActivity; }
};

#endif
