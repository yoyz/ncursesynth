#include "midi_input.h"
#include <iostream>
#include <cmath>
#include <chrono>
#include <cstring>
#include <set>

MidiInput::MidiInput(SynthArchitecture* synthArch)
    : running(false), initialized(false), synth(synthArch), machine(nullptr), mappingMachine(nullptr), selectedPort(-1),
      lastCC(-1), lastCCValue(0), lastActivity(std::chrono::steady_clock::now()), midiDebug(false) {}

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
    if (message.empty()) return;
    
    int status = message[0];
    int data1 = message.size() > 1 ? message[1] : 0;
    int data2 = message.size() > 2 ? message[2] : 0;
    
    int msgType = status & 0xF0;
    int channel = status & 0x0F;
    int note = data1;
    int velocity = data2;
    
    if (msgType == 0xB0 && data1 >= 0 && data1 <= 127) {
        int cc = data1;
        int value = data2;
        
        lastCC = cc;
        lastCCValue = value;
        lastActivity = std::chrono::steady_clock::now();
        
        if (mappingMachine) {
            MappingEntry entry = mappingManager.getMappingEntry(cc);
            if (!entry.parameterName.empty()) {
                float normalized = static_cast<float>(value - entry.minValue) / static_cast<float>(entry.maxValue - entry.minValue);
                normalized = std::max(0.0f, std::min(1.0f, normalized));
                mappingMachine->applyCC(cc, normalized, entry.parameterName);
                if (midiDebug) std::cerr << "  CC " << cc << " -> " << entry.parameterName << " (" << value << ")" << std::endl;
            }
        } else if (synth) {
            mappingManager.applyMapping(synth, cc, static_cast<float>(value));
        }
        return;
    }
    
    if (msgType == 0x90 || msgType == 0x80) {
        lastCC = note + (msgType == 0x90 ? 128 : 256);
        lastCCValue = velocity;
        lastActivity = std::chrono::steady_clock::now();
    }
    
    float frequency = 440.0f * powf(2.0f, (note - 69) / 12.0f);

    if (midiDebug) {
        std::cerr << "  type=" << std::hex << msgType << std::dec << " note=" << note << " vel=" << velocity << std::endl;
    }

    switch (msgType) {
        case 0x90:
            if (velocity > 0) {
                if (midiDebug) std::cerr << "  NOTE ON note=" << note << " vel=" << velocity << std::endl;
                if (synth) synth->noteOn(frequency);
                if (machine) {
                    activeNotes.insert(note);
                    machine->setI(70, note);
                    machine->setI(150, 1);
                }
            } else {
                if (midiDebug) std::cerr << "  NOTE OFF note=" << note << std::endl;
                if (synth) synth->noteOff(frequency);
                if (machine) {
                    activeNotes.erase(note);
                    machine->setI(70, note);
                    machine->setI(150, 0);
                }
            }
            break;
        case 0x80:
            if (midiDebug) std::cerr << "  NOTE OFF note=" << note << " vel=" << velocity << std::endl;
            if (synth) synth->noteOff(frequency);
            if (machine) {
                activeNotes.erase(note);
                machine->setI(70, note);
                machine->setI(150, 0);
            }
            break;
        default:
            break;
    }
}

void MidiInput::midiThreadFunc() {
    std::cerr << "MIDI thread started, port open=" << (midiIn && midiIn->isPortOpen()) << std::endl;
    while (running) {
        if (midiIn && midiIn->isPortOpen()) {
            std::vector<unsigned char> message;
            midiIn->getMessage(&message);

            if (!message.empty()) {
                if (midiDebug) {
                    std::cerr << "MIDI RAW: ";
                    for (size_t i = 0; i < message.size(); i++) {
                        std::cerr << std::hex << "0x" << (int)message[i] << " ";
                    }
                    std::cerr << std::dec << std::endl;
                }
                processMessage(message);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    std::cerr << "MIDI thread exiting" << std::endl;
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