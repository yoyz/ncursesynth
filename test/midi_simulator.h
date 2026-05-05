#ifndef MIDI_SIMULATOR_H
#define MIDI_SIMULATOR_H

#include <vector>
#include <string>
#include <memory>
#include <thread>
#include <mutex>

struct MidiMessage {
    int status;      // 0x90=note on, 0x80=note off, 0xB0=CC
    int data1;      // Note number or CC number
    int data2;      // Velocity or CC value
};

class MidiSimulator {
public:
    MidiSimulator();
    ~MidiSimulator();

    // Initialize MIDI (can use virtual device or none)
    bool initialize();
    
    // Send MIDI messages
    void sendNoteOn(int note, int velocity = 100);
    void sendNoteOff(int note, int velocity = 0);
    void sendCC(int cc, int value);
    void sendProgramChange(int prog, int channel);
    void sendPitchBend(int value);
    
    // Send message vector
    void sendMessages(const std::vector<MidiMessage>& messages);
    
    // Sequential send with delays
    void sendSequential(const std::vector<MidiMessage>& messages, 
                       int delayUs = 10000);
    
    // Get sent messages for verification
    const std::vector<MidiMessage>& getSentMessages() const;
    
    // Thread-safe async send
    void sendAsync(const std::vector<MidiMessage>& messages);
    
    // Send all pending messages
    void sendAll();
    
    // Clear sent messages
    void clear();

private:
    std::vector<MidiMessage> sentMessages_;
    std::mutex msgMutex_;
    bool initialized_;
};

#endif
