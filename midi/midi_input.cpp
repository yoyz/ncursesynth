#include "midi_input.h"
#include <iostream>
#include <cmath>
#include <chrono>
#include <cstring>

MidiInput::MidiInput(SynthArchitecture* synthArch) 
    : running(false), initialized(false), synth(synthArch), selectedPort(-1) {}

MidiInput::~MidiInput() {
    stop();
}

bool MidiInput::initialize() {
    try {
        midiIn = std::make_unique<RtMidiIn>(RtMidi::UNSPECIFIED);
        initialized = true;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error initializing MIDI: " << e.what() << std::endl;
        initialized = false;
        return false;
    }
}

void MidiInput::listDevices() const {
    if (!initialized || !midiIn) {
        std::cout << "MIDI not initialized." << std::endl;
        return;
    }
    
    std::cout << "\nAvailable MIDI Input Ports:" << std::endl;
    unsigned int nPorts = midiIn->getPortCount();
    for (unsigned int i = 0; i < nPorts; i++) {
        std::cout << "  " << i << ": " << midiIn->getPortName(i) << std::endl;
    }
    if (nPorts == 0) {
        std::cout << "  No MIDI devices found." << std::endl;
    }
    std::cout << std::endl;
}

int MidiInput::getDeviceCount() const {
    if (!initialized || !midiIn) return 0;
    return midiIn->getPortCount();
}

std::string MidiInput::getDeviceName(int port) const {
    if (!initialized || !midiIn) return "";
    try {
        return midiIn->getPortName(port);
    } catch (...) {
        return "";
    }
}

bool MidiInput::selectDevice(int port) {
    if (!initialized || !midiIn) return false;
    
    try {
        if (port >= 0 && port < static_cast<int>(midiIn->getPortCount())) {
            if (midiIn->isPortOpen()) {
                midiIn->closePort();
            }
            midiIn->openPort(port);
            selectedPort = port;
            return true;
        } else {
            std::cerr << "Invalid MIDI port: " << port << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error opening MIDI port: " << e.what() << std::endl;
        return false;
    }
}

void MidiInput::processMessage(const std::vector<unsigned char>& message) {
    if (message.size() < 3) return;
    
    int status = message[0] & 0xF0;
    int note = message[1];
    int velocity = message[2];
    
    // Convert MIDI note number to frequency (A4 = 440Hz, MIDI note 69)
    float frequency = 440.0f * powf(2.0f, (note - 69) / 12.0f);
    
    switch (status) {
        case 0x90: // Note On
            if (velocity > 0) {
                // Note on
                synth->noteOn(frequency);
            } else {
                // Note on with velocity 0 is treated as note off
                synth->noteOff(frequency);
            }
            break;
        case 0x80: // Note Off
            synth->noteOff(frequency);
            break;
        default:
            // Other MIDI messages (Control Change, etc.) - ignore for now
            break;
    }
}

void MidiInput::midiThreadFunc() {
    while (running) {
        if (midiIn && midiIn->isPortOpen()) {
            std::vector<unsigned char> message;
            double stamp = midiIn->getMessage(&message);
            
            if (!message.empty()) {
                processMessage(message);
            }
        }
        // Small sleep to prevent CPU spinning
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void MidiInput::start() {
    if (running) return;
    
    if (!initialized || !midiIn) {
        std::cerr << "MIDI not initialized. Cannot start." << std::endl;
        return;
    }
    
    if (selectedPort != -1 && midiIn->isPortOpen()) {
        running = true;
        midiThread = std::thread(&MidiInput::midiThreadFunc, this);
    } else {
        std::cout << "No MIDI device selected. MIDI input disabled." << std::endl;
    }
}

void MidiInput::stop() {
    if (!running) return;
    
    running = false;
    if (midiThread.joinable()) {
        midiThread.join();
    }
    
    if (midiIn && midiIn->isPortOpen()) {
        midiIn->closePort();
    }
}
