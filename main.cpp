#include <iostream>
#include <string>
#include <cstring>
#include <signal.h>
#include <atomic>
#include <chrono>
#include <thread>
#include <limits>
#include "audio/audio_engine.h"
#include "ui/ui.h"
#include "midi/midi_input.h"
#include "machine/MachineManager.h"
#include "machine/NcursesynthMachine.h"
#include "machine/PBSynth/PBSynthMachine.h"
#include "ui/pbsynth_ui.h"

std::atomic<bool> running(true);

void signalHandler(int sig) {
    if (sig == SIGINT) {
        running = false;
    }
}

void listMachines(MachineManager& mm) {
    std::cout << "\nAvailable Synth Engines:\n";
    std::cout << "-------------------------\n";
    for (int i = 0; i < mm.getMachineCount(); i++) {
        std::cout << "  " << (i + 1) << ". " << mm.getMachineName(i) << "\n";
    }
    std::cout << "\n";
}

int selectMachine(MachineManager& mm) {
    while (true) {
        std::cout << "Select engine (1-" << mm.getMachineCount() << "): ";
        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }
        if (choice >= 1 && choice <= mm.getMachineCount()) {
            return choice - 1;
        }
        std::cout << "Invalid choice.\n";
    }
}

int main(int argc, char* argv[]) {
    bool midiDebug = false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--midi-debug") == 0) {
            midiDebug = true;
        }
    }

    signal(SIGINT, signalHandler);
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "  VIRTUAL ANALOG SYNTHESIZER v1.0" << std::endl;
    std::cout << "  with MIDI Support" << std::endl;
    std::cout << "========================================\n" << std::endl;

    // Create machine manager and register engines
    MachineManager machineManager;
    machineManager.registerMachine(new NcursesynthMachine());
    machineManager.registerMachine(new PBSynthMachine());

    // Let user select engine
    listMachines(machineManager);
    int selected = selectMachine(machineManager);
    machineManager.setCurrentMachine(selected);

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

    bool usePBSynthUI = (pbsynthMachine != nullptr);

    if (midiInput.initialize()) {
        midiInput.listDevices();
        midiInput.loadMappings();

        int deviceCount = midiInput.getDeviceCount();
        if (deviceCount > 0) {
            std::cout << "\nSelect MIDI input device:\n";
            std::cout << "  0. None (keyboard only)\n";
            for (int i = 0; i < deviceCount; i++) {
                std::cout << "  " << (i+1) << ". " << midiInput.getDeviceName(i) << "\n";
            }
            std::cout << "Choice: ";

            int choice;
            if (std::cin >> choice) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (choice >= 1 && choice <= deviceCount) {
                    if (midiInput.selectDevice(choice - 1)) {
                        std::cout << "MIDI device selected.\n" << std::endl;
                        midiInput.start();
                    }
                } else if (choice == 0) {
                    std::cout << "No MIDI device selected.\n" << std::endl;
                } else {
                    std::cout << "Invalid choice.\n" << std::endl;
                }
            }
        } else {
            std::cout << "No MIDI devices found.\n" << std::endl;
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
        PBSynthUI pui(activeMachine);
        pui.init();
        pui.draw();

        int ch;
        while ((ch = getch()) != 'q' && ch != 'Q' && ch != 27) {
            if (ch != ERR) {
                pui.handleInput(ch);
            }
            pui.updateValues();

            if (pbsynthMachine && pbsynthMachine->getKeyOn()) {
                pui.setMidiNote(pbsynthMachine->getLastNote(), 127);
            }

            pui.draw();
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
