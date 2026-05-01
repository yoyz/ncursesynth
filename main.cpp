#include <iostream>
#include <string>
#include <cstring>
#include <signal.h>
#include <atomic>
#include <chrono>
#include <thread>
#include <limits>
#include <memory>
#include "audio/audio_engine.h"
#include "ui/machine_ui.h"
#include "ui/ui.h"
#include "midi/midi_input.h"
#include "machine/MachineManager.h"
#include "machine/Ncursesynth/NcursesynthMachine.h"
#include "machine/PBSynth/PBSynthMachine.h"
#include "machine/Cursynth/CursynthMachine.h"
#include "machine/Twytch/TwytchsynthMachine.h"
#include "ui/pbsynth_ui.h"
#include "ui/twytch_ui.h"

std::atomic<bool> running(true);

void signalHandler(int sig) {
    if (sig == SIGINT) {
        running = false;
    }
}

int main(int argc, char* argv[]) {
    bool midiDebug = false;
    std::string midiPort;
    bool listMidi = false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            std::cout << "ncursesynth - Virtual Analog Synthesizer\n\n";
            std::cout << "Usage: virtual_synth [options]\n\n";
            std::cout << "Options:\n";
            std::cout << "  --help, -h           Show this help message\n";
            std::cout << "  --midi-debug         Enable MIDI debug output\n";
            std::cout << "  --midi-port hw:X,Y,Z Select MIDI port on startup (e.g., hw:1,0,0)\n";
            std::cout << "  --list-midi         List available MIDI devices\n";
            std::cout << "\nControls:\n";
            std::cout << "  TAB     Switch menu (Engine/MIDI/Params)\n";
            std::cout << "  ARROWS  Navigate parameters or menu items\n";
            std::cout << "  1-9,0   Set slider to 10%-100%\n";
            std::cout << "  Q       Quit\n";
            return 0;
        }
        if (strcmp(argv[i], "--midi-debug") == 0) {
            midiDebug = true;
        }
        if (strcmp(argv[i], "--midi-port") == 0 && i + 1 < argc) {
            midiPort = argv[++i];
        }
        if (strcmp(argv[i], "--list-midi") == 0) {
            listMidi = true;
        }
    }

    // If --list-midi, list available MIDI devices
    if (listMidi) {
        MidiInput tmpMidi(nullptr);
        if (!tmpMidi.initialize()) {
            std::cerr << "Error: Cannot initialize MIDI. Check permissions or if MIDI hardware is available." << std::endl;
            return 1;
        }
        std::cout << "Available MIDI devices:\n";
        std::cout << "Dir Device    Name\n";
        int count = tmpMidi.getDeviceCount();
        for (int i = 0; i < count; i++) {
            std::string name = tmpMidi.getDeviceName(i);
            if (!name.empty()) {
                std::cout << "IO  hw:" << i << ",0,0  " << name << "\n";
            }
        }
        if (count == 0) {
            std::cout << "(no MIDI devices found)\n";
        }
        std::cout << "\nUse --midi-port hw:X,Y,Z to select a device\n";
        return 0;
    }

signal(SIGINT, signalHandler);
    
    std::cout << "ncursesynth - Virtual Analog Synthesizer\n";
    std::cout << "TAB: Switch Engine | ARROWS: Navigate Params | Q: Quit\n\n";
    
    // Create machine manager and register engines
    MachineManager machineManager;
    machineManager.registerMachine(new NcursesynthMachine());
    machineManager.registerMachine(new PBSynthMachine());
    machineManager.registerMachine(new CursynthMachine(8));
    machineManager.registerMachine(new TwytchsynthMachine());

    // Start with first engine, user can switch in UI
    machineManager.setCurrentMachine(0);

    Machine* activeMachine = machineManager.getCurrentMachine();
    std::cout << "Selected engine: " << activeMachine->getName() << "\n" << std::endl;

    // Create audio engine
    AudioEngine audioEngine(48000, 256);

    if (!audioEngine.initialize()) {
        std::cerr << "Failed to initialize audio engine!" << std::endl;
        return 1;
    }

    // Get synth reference (for ncursesynth engine)
    SynthArchitecture* synth = audioEngine.getSynth();

    // If using ncursesynth, set synth on the machine and load presets
    NcursesynthMachine* ncSynth = dynamic_cast<NcursesynthMachine*>(activeMachine);
    if (ncSynth) {
        ncSynth->setSynth(synth);
        if (synth->getPresetManager()->exists()) {
            synth->getPresetManager()->loadPreset(0, synth);
        }
    }

    // Initialize MIDI input
    MidiInput midiInput(synth);
    synth->setMidiInput(&midiInput);

    // If using PBSynth, set machine for MIDI
    PBSynthMachine* pbsynthMachine = dynamic_cast<PBSynthMachine*>(activeMachine);
    if (pbsynthMachine) {
        midiInput.setMachine(pbsynthMachine);
        midiInput.setMappingMachine(pbsynthMachine);
    }

    // If using Cursynth, set machine for MIDI
    CursynthMachine* cursynthMachine = dynamic_cast<CursynthMachine*>(activeMachine);
    if (cursynthMachine) {
        midiInput.setMachine(cursynthMachine);
        midiInput.setMappingMachine(cursynthMachine);
    }

    // If using Twytch, set machine for MIDI
    TwytchsynthMachine* twytchMachine = dynamic_cast<TwytchsynthMachine*>(activeMachine);
    if (twytchMachine) {
        midiInput.setMachine(twytchMachine);
        midiInput.setMappingMachine(twytchMachine);
    }

    // Use PBSynth-style UI for all machines
    bool usePBSynthUI = (activeMachine != nullptr);

    // Initialize MIDI
    if (midiInput.initialize()) {
        midiInput.loadMappings();

        // If --midi-port specified, try to select that device
        if (!midiPort.empty()) {
            int selectedIndex = -1;
            // Parse "hw:X,Y,Z" format - the port number is the first number
            if (midiPort.substr(0, 3) == "hw:") {
                std::string portNumStr = midiPort.substr(3);
                int portNum = atoi(portNumStr.c_str());
                if (portNum >= 0 && portNum < midiInput.getDeviceCount()) {
                    selectedIndex = portNum;
                }
            }

            if (selectedIndex >= 0) {
                midiInput.selectDevice(selectedIndex);
                midiInput.setMappingMachine(activeMachine);
                midiInput.start();
                std::cout << "MIDI connected to: " << midiInput.getDeviceName(selectedIndex) << "\n" << std::endl;
            } else {
                std::cout << "MIDI device '" << midiPort << "' not found. Use --list-midi to see available devices.\n" << std::endl;
            }
        } else {
            std::cout << "MIDI initialized. Use TAB in UI to select MIDI device.\n" << std::endl;
        }
    } else {
        std::cout << "Failed to initialize MIDI.\n" << std::endl;
    }

    if (midiDebug) {
        midiInput.setMidiDebug(true);
        if (activeMachine) activeMachine->setMidiDebug(true);
        std::cout << "MIDI debug enabled." << std::endl;
    }

    // Set machine for audio
    audioEngine.setMachine(activeMachine);

    // Start audio
    if (!audioEngine.start()) {
        std::cerr << "Failed to start audio!" << std::endl;
        audioEngine.shutdown();
        return 1;
    }

    std::cout << "Audio started successfully!" << std::endl;
    std::cout << "UI launching...\n" << std::endl;
    std::cout << "Press Ctrl+C to exit\n" << std::endl;

    if (usePBSynthUI) {
        TwytchsynthMachine* twytchMachine = dynamic_cast<TwytchsynthMachine*>(activeMachine);

        std::unique_ptr<MachineUI> ui;
        if (twytchMachine) {
            ui.reset(new TwytchUI(activeMachine, &machineManager));
        } else {
            ui.reset(new PBSynthUI(activeMachine, &machineManager));
        }
        ui->setMidiInput(&midiInput);
        ui->setMidiDeviceIndex(midiInput.getSelectedPort());
        ui->init();
        ui->draw();

        int ch;
        Machine* lastMachine = activeMachine;
        while ((ch = getch()) != 'q' && ch != 'Q' && ch != 27) {
            if (ch != ERR) {
                ui->handleInput(ch);
            }
            ui->updateValues();

            // Get current active machine from manager
            activeMachine = machineManager.getCurrentMachine();
            const MachineManager& mgr = machineManager;
            int currentEngineIndex = mgr.getCurrentMachine();

            // If machine changed, update audio engine and create new UI
            if (activeMachine != lastMachine) {
                audioEngine.setMachine(activeMachine);

                // Also update MIDI machine
                PBSynthMachine* pbsynthMachine = dynamic_cast<PBSynthMachine*>(activeMachine);
                CursynthMachine* cursynthMachine = dynamic_cast<CursynthMachine*>(activeMachine);
                TwytchsynthMachine* newTwytchMachine = dynamic_cast<TwytchsynthMachine*>(activeMachine);

                if (pbsynthMachine) {
                    midiInput.setMachine(pbsynthMachine);
                    midiInput.setMappingMachine(pbsynthMachine);
                } else if (cursynthMachine) {
                    midiInput.setMachine(cursynthMachine);
                    midiInput.setMappingMachine(cursynthMachine);
                } else if (newTwytchMachine) {
                    midiInput.setMachine(newTwytchMachine);
                    midiInput.setMappingMachine(newTwytchMachine);
                }

                // Create new UI for the new machine type
                int currentMidiDeviceIndex = ui->getMidiDeviceIndex();
                if (newTwytchMachine) {
                    ui.reset(new TwytchUI(activeMachine, &machineManager));
                } else {
                    ui.reset(new PBSynthUI(activeMachine, &machineManager));
                }
                ui->setMidiInput(&midiInput);
                ui->setEngineIndex(currentEngineIndex);
                ui->setMidiDeviceIndex(currentMidiDeviceIndex);

                lastMachine = activeMachine;
            }

            if (activeMachine) {
                // Cast and check keyon
                PBSynthMachine* pbsynthMachine = dynamic_cast<PBSynthMachine*>(activeMachine);
                CursynthMachine* cursynthMachine = dynamic_cast<CursynthMachine*>(activeMachine);
                TwytchsynthMachine* twytchMachine = dynamic_cast<TwytchsynthMachine*>(activeMachine);

                if (pbsynthMachine && pbsynthMachine->getKeyOn()) {
                    ui->setMidiNote(pbsynthMachine->getLastNote(), 127);
                } else if (cursynthMachine && cursynthMachine->getKeyOn()) {
                    ui->setMidiNote(cursynthMachine->getLastNote(), 127);
                } else if (twytchMachine && twytchMachine->getKeyOn()) {
                    ui->setMidiNote(twytchMachine->getLastNote(), 127);
                }
            }

            ui->draw();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        endwin();
    } else {
        UI ui(synth);
        ui.start();

        while (running && ui.isRunning()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        std::cout << "\nShutting down..." << std::endl;
        midiInput.stop();
        ui.stop();
    }

    audioEngine.shutdown();
    
    std::cout << "Virtual Synthesizer exited cleanly." << std::endl;
    return 0;
}
