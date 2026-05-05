#include "midi_simulator.h"
#include <unistd.h>
#include <cstring>

MidiSimulator::MidiSimulator() : initialized_(false) {}

MidiSimulator::~MidiSimulator() {}

bool MidiSimulator::initialize() {
    initialized_ = true;
    return true;
}

void MidiSimulator::sendNoteOn(int note, int velocity) {
    MidiMessage msg;
    msg.status = 0x90;  // Note On
    msg.data1 = note;
    msg.data2 = velocity;
    
    std::lock_guard<std::mutex> lock(msgMutex_);
    sentMessages_.push_back(msg);
}

void MidiSimulator::sendNoteOff(int note, int velocity) {
    MidiMessage msg;
    msg.status = 0x80;  // Note Off
    msg.data1 = note;
    msg.data2 = velocity;
    
    std::lock_guard<std::mutex> lock(msgMutex_);
    sentMessages_.push_back(msg);
}

void MidiSimulator::sendCC(int cc, int value) {
    MidiMessage msg;
    msg.status = 0xB0;  // Control Change
    msg.data1 = cc;
    msg.data2 = value;
    
    std::lock_guard<std::mutex> lock(msgMutex_);
    sentMessages_.push_back(msg);
}

void MidiSimulator::sendProgramChange(int prog, int channel) {
    MidiMessage msg;
    msg.status = 0xC0;  // Program Change
    msg.data1 = prog;
    msg.data2 = channel;
    
    std::lock_guard<std::mutex> lock(msgMutex_);
    sentMessages_.push_back(msg);
}

void MidiSimulator::sendPitchBend(int value) {
    MidiMessage msg;
    msg.status = 0xE0;  // Pitch Bend
    msg.data1 = value & 0x7F;
    msg.data2 = (value >> 7) & 0x7F;
    
    std::lock_guard<std::mutex> lock(msgMutex_);
    sentMessages_.push_back(msg);
}

void MidiSimulator::sendMessages(const std::vector<MidiMessage>& messages) {
    std::lock_guard<std::mutex> lock(msgMutex_);
    for (const auto& msg : messages) {
        sentMessages_.push_back(msg);
    }
}

void MidiSimulator::sendSequential(const std::vector<MidiMessage>& messages, 
                                   int delayUs) {
    for (const auto& msg : messages) {
        // Send message
        // In real implementation, this would send via MIDI port
        // For testing, we just record it
        
        std::lock_guard<std::mutex> lock(msgMutex_);
        sentMessages_.push_back(msg);
        
        if (delayUs > 0) {
            usleep(delayUs);
        }
    }
}

const std::vector<MidiMessage>& MidiSimulator::getSentMessages() const {
    return sentMessages_;
}

void MidiSimulator::sendAsync(const std::vector<MidiMessage>& messages) {
    std::thread([this, messages]() {
        std::lock_guard<std::mutex> lock(msgMutex_);
        for (const auto& msg : messages) {
            sentMessages_.push_back(msg);
        }
    }).detach();
}

void MidiSimulator::sendAll() {
    // All messages are already in sentMessages_
    // This is a no-op for the simulator
}

void MidiSimulator::clear() {
    std::lock_guard<std::mutex> lock(msgMutex_);
    sentMessages_.clear();
}
