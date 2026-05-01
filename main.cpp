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
#include "machine/Ncursesynth/NcursesynthMachine.h"
#include "machine/PBSynth/PBSynthMachine.h"
#include "machine/Cursynth/CursynthMachine.h"
#include "machine/Twytch/TwytchsynthMachine.h"
#include "ui/pbsynth_ui.h"

std::atomic<bool> running(true);

void signalHandler(int sig) {
    if (sig == SIGINT) {
        running = false;
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

    // Initialize MIDI (device selection is done in UI)
    if (midiInput.initialize()) {
        midiInput.loadMappings();
        std::cout << "MIDI initialized. Use TAB in UI to select MIDI device.\n" << std::endl;
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
        PBSynthUI pui(activeMachine, &machineManager);
        pui.setMidiInput(&midiInput);
        pui.init();
        pui.draw();

        int ch;
        Machine* lastMachine = activeMachine;
        while ((ch = getch()) != 'q' && ch != 'Q' && ch != 27) {
            if (ch != ERR) {
                pui.handleInput(ch);
            }
            pui.updateValues();
            
            // Get current active machine from manager
            activeMachine = machineManager.getCurrentMachine();
            
            // If machine changed, update audio engine and MIDI
            if (activeMachine != lastMachine) {
                audioEngine.setMachine(activeMachine);
                
                // Also update MIDI machine
                PBSynthMachine* pbsynthMachine = dynamic_cast<PBSynthMachine*>(activeMachine);
                CursynthMachine* cursynthMachine = dynamic_cast<CursynthMachine*>(activeMachine);
                TwytchsynthMachine* twytchMachine = dynamic_cast<TwytchsynthMachine*>(activeMachine);
                
                if (pbsynthMachine) {
                    midiInput.setMachine(pbsynthMachine);
                    midiInput.setMappingMachine(pbsynthMachine);
                } else if (cursynthMachine) {
                    midiInput.setMachine(cursynthMachine);
                    midiInput.setMappingMachine(cursynthMachine);
                } else if (twytchMachine) {
                    midiInput.setMachine(twytchMachine);
                    midiInput.setMappingMachine(twytchMachine);
                }
                
                lastMachine = activeMachine;
            }
            
            if (activeMachine) {
                // Cast and check keyon
                PBSynthMachine* pbsynthMachine = dynamic_cast<PBSynthMachine*>(activeMachine);
                CursynthMachine* cursynthMachine = dynamic_cast<CursynthMachine*>(activeMachine);
                TwytchsynthMachine* twytchMachine = dynamic_cast<TwytchsynthMachine*>(activeMachine);
                
                if (pbsynthMachine && pbsynthMachine->getKeyOn()) {
                    pui.setMidiNote(pbsynthMachine->getLastNote(), 127);
                } else if (cursynthMachine && cursynthMachine->getKeyOn()) {
                    pui.setMidiNote(cursynthMachine->getLastNote(), 127);
                } else if (twytchMachine && twytchMachine->getKeyOn()) {
                    pui.setMidiNote(twytchMachine->getLastNote(), 127);
                }
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
