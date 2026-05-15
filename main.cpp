#include <iostream>
#include <string>
#include <cstring>
#include <signal.h>
#include <atomic>
#include <chrono>
#include <thread>
#include <limits>
#include <memory>
#include <fstream>
#include "audio/audio_engine.h"
#include "audio/audio_capture.h"
#include "audio/capture_analysis.h"
#include "ui/machine_ui.h"
#include "midi/midi_input.h"
#include "midi/tcp_midi_server.h"
#include "midi/midi_capture.h"
#include "machine/MachineManager.h"
#include "machine/Ncursesynth/NcursesynthMachine.h"
#include "machine/PBSynth/PBSynthMachine.h"
#include "machine/Cursynth/CursynthMachine.h"
#include "machine/Twytch/TwytchsynthMachine.h"
#include "ui/pbsynth_ui.h"
#include "ui/twytch_ui.h"
#include "ui/ncursesynth_ui.h"
#include "ui/cursynth_ui.h"

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
    int tcpMidiPort = -1;
    int captureAudioPort = -1;
    std::string captureAnalysisPath;
    std::string midiCapturePath;
    std::string synthEngineName;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            std::cout << "ncursesynth - Virtual Analog Synthesizer\n\n";
            std::cout << "Usage: virtual_synth [options]\n\n";
            std::cout << "Options:\n";
            std::cout << "  --help, -h           Show this help message\n";
            std::cout << "  --midi-debug         Enable MIDI debug output\n";
            std::cout << "  --midi-port hw:X,Y,Z Select MIDI port on startup (e.g., hw:1,0,0)\n";
            std::cout << "  --list-midi         List available MIDI devices\n";
            std::cout << "  --tcp-midi-port N   TCP port for remote MIDI input\n";
            std::cout << "  --tcp-capture-audio N  TCP port for audio capture (replaces PortAudio)\n";
            std::cout << "  --capture-audio-plus-fft-rms FILE  Write raw audio + per-second FFT/RMS analysis\n";
            std::cout << "  --capture-midi-plus-analysis FILE  Write MIDI event log (note_on/off/cc) with timestamps\n";
            std::cout << "  --synthengine NAME  Run headless with named engine (ncursesynth/pbsynth/cursynth/twytch)\n";
            std::cout << "\nControls (UI mode):\n";
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
        if (strcmp(argv[i], "--tcp-midi-port") == 0 && i + 1 < argc) {
            tcpMidiPort = atoi(argv[++i]);
        }
        if (strcmp(argv[i], "--tcp-capture-audio") == 0 && i + 1 < argc) {
            captureAudioPort = atoi(argv[++i]);
        }
        if (strcmp(argv[i], "--capture-audio-plus-fft-rms") == 0 && i + 1 < argc) {
            captureAnalysisPath = argv[++i];
        }
        if (strcmp(argv[i], "--capture-midi-plus-analysis") == 0 && i + 1 < argc) {
            midiCapturePath = argv[++i];
        }
        if (strcmp(argv[i], "--synthengine") == 0 && i + 1 < argc) {
            synthEngineName = argv[++i];
        }
    }

    bool headless = !synthEngineName.empty();

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

    // Init the machine fully (Init preset = all defaults)
    if (activeMachine) activeMachine->init();

    // Start capture analysis if requested (intercepts audio from all drivers)
    CaptureAnalyzer* analyzer = nullptr;
    if (!captureAnalysisPath.empty()) {
        analyzer = new CaptureAnalyzer(captureAnalysisPath, 48000);
        CaptureAnalyzer::setInstance(analyzer);
        std::cout << "Audio capture + FFT/RMS analysis enabled -> " << captureAnalysisPath << std::endl;
    }

    // Start MIDI capture if requested
    MidiCapture* midiCap = nullptr;
    if (!midiCapturePath.empty()) {
        midiCap = new MidiCapture(midiCapturePath);
        MidiCapture::setInstance(midiCap);
        std::cout << "MIDI capture enabled -> " << midiCapturePath << ".midi.txt" << std::endl;
    }

    // Audio drivers: either PortAudio (normal) or TCP capture (headless)
    AudioCaptureDriver* captureDriver = nullptr;
    AudioEngine* audioEngine = nullptr;
    SynthArchitecture* synth = nullptr;

    if (captureAudioPort > 0) {
        captureDriver = new AudioCaptureDriver(captureAudioPort, 48000);
        if (!captureDriver->start(activeMachine)) {
            std::cerr << "Failed to start audio capture driver!" << std::endl;
            delete captureDriver;
            return 1;
        }
        synth = new SynthArchitecture(8, 48000);
        std::cout << "Audio capture driver on port " << captureAudioPort << std::endl;
    } else {
        audioEngine = new AudioEngine(48000, 256);
        if (!audioEngine->initialize()) {
            std::cerr << "Failed to initialize audio engine!" << std::endl;
            return 1;
        }
        synth = audioEngine->getSynth();
    }

    // Initialize MIDI input
    MidiInput midiInput(synth);
    synth->setMidiInput(&midiInput);

    // Load mappings (always, regardless of MIDI status)
    midiInput.loadMappings();

    // Initialize MIDI
    if (midiInput.initialize()) {

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
                midiInput.setMachine(activeMachine);
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

    // Start TCP MIDI server if requested
    TcpMidiServer* tcpMidi = nullptr;
    if (tcpMidiPort > 0) {
        tcpMidi = new TcpMidiServer(tcpMidiPort, &machineManager);
        tcpMidi->start();
    }

    // Set machine for audio / capture
    if (audioEngine) {
        audioEngine->setMachine(activeMachine);
        if (!audioEngine->start()) {
            std::cerr << "Failed to start audio!" << std::endl;
            audioEngine->shutdown();
            return 1;
        }
    std::cout << "Audio started successfully!" << std::endl;
    } else if (captureDriver) {
        captureDriver->setMachine(activeMachine);
        std::cout << "Audio capture running (no PortAudio)" << std::endl;
    }

    if (headless) {
        std::cout << "\nHeadless mode. Use --tcp-midi-port to send MIDI commands. Ctrl+C to exit.\n" << std::endl;
        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    } else {
        std::cout << "UI launching...\n" << std::endl;
        std::cout << "Press Ctrl+C to exit\n" << std::endl;
        
        std::unique_ptr<MachineUI> ui;
        
        // Determine UI type based on active machine
        TwytchsynthMachine* twytchMachine = dynamic_cast<TwytchsynthMachine*>(activeMachine);
        PBSynthMachine* pbsynthMachine = dynamic_cast<PBSynthMachine*>(activeMachine);
        CursynthMachine* cursynthMachine = dynamic_cast<CursynthMachine*>(activeMachine);
        NcursesynthMachine* ncursesynthMachine = dynamic_cast<NcursesynthMachine*>(activeMachine);

    // Use new engine UI if available (engine-specific UI exists)
    bool useNewEngineUI = twytchMachine || pbsynthMachine || cursynthMachine || ncursesynthMachine;

    if (useNewEngineUI) {
        if (twytchMachine) {
            ui.reset(new TwytchUI(activeMachine, &machineManager));
        } else if (pbsynthMachine) {
            ui.reset(new PBSynthUI(activeMachine, &machineManager));
        } else if (cursynthMachine) {
            ui.reset(new CursynthUI(activeMachine, &machineManager));
        } else if (ncursesynthMachine) {
            ui.reset(new NcursesynthUI(activeMachine, &machineManager));
        } else {
            ui.reset(new TwytchUI(activeMachine, &machineManager));
        }
    } else {
        // Fallback to default UI
        ui.reset(new TwytchUI(activeMachine, &machineManager));
    }

    ui->setMidiInput(&midiInput);
    ui->setMidiDeviceIndex(midiInput.getSelectedPort());
    ui->init();
    ui->draw();

    Machine* lastMachine = activeMachine;
    while (running && ui->isActive()) {
        int ch = getch();
        
        if (ch != ERR) {
            ui->handleInput(ch);
        }
        ui->updateValues();

        activeMachine = machineManager.getCurrentMachine();
        int currentEngineIndex = machineManager.getCurrentMachineIndex();

        // If machine changed, update audio engine and create new UI
        (void)currentEngineIndex; // Suppress unused warning
        if (activeMachine != lastMachine) {
            if (audioEngine) audioEngine->setMachine(activeMachine);
            if (captureDriver) captureDriver->setMachine(activeMachine);

            // Save state before engine switch
            int savedMenuSelection = ui->getMenuSelection();
            int savedMenuIndex = ui->getMenuIndex();
            int savedMidiDeviceIndex = ui->getMidiDeviceIndex();
            int savedPresetIndex = ui->getPresetIndex();

            // Also update MIDI machine
            PBSynthMachine* pbsynthMachine2 = dynamic_cast<PBSynthMachine*>(activeMachine);
            CursynthMachine* cursynthMachine2 = dynamic_cast<CursynthMachine*>(activeMachine);
            TwytchsynthMachine* twytchMachine2 = dynamic_cast<TwytchsynthMachine*>(activeMachine);
            NcursesynthMachine* ncursesynthMachine2 = dynamic_cast<NcursesynthMachine*>(activeMachine);

            if (pbsynthMachine2) {
                midiInput.setMachine(pbsynthMachine2);
                midiInput.setMappingMachine(pbsynthMachine2);
                pbsynthMachine2->init();
            } else if (cursynthMachine2) {
                midiInput.setMachine(cursynthMachine2);
                midiInput.setMappingMachine(cursynthMachine2);
                cursynthMachine2->init();
            } else if (twytchMachine2) {
                midiInput.setMachine(twytchMachine2);
                midiInput.setMappingMachine(twytchMachine2);
                twytchMachine2->init();
            } else if (ncursesynthMachine2) {
                midiInput.setMachine(ncursesynthMachine2);
                midiInput.setMappingMachine(ncursesynthMachine2);
                ncursesynthMachine2->init();
            }

            // Create new UI for the new machine type
             bool useNewEngineUI = twytchMachine2 || pbsynthMachine2 || cursynthMachine2 || ncursesynthMachine2;
             if (useNewEngineUI) {
                 if (twytchMachine2) {
                     ui.reset(new TwytchUI(activeMachine, &machineManager));
                 } else if (pbsynthMachine2) {
                     ui.reset(new PBSynthUI(activeMachine, &machineManager));
                 } else if (cursynthMachine2) {
                     ui.reset(new CursynthUI(activeMachine, &machineManager));
                 } else if (ncursesynthMachine2) {
                     ui.reset(new NcursesynthUI(activeMachine, &machineManager));
                 } else {
                     ui.reset(new TwytchUI(activeMachine, &machineManager));
                 }
             } else {
                 // Fallback to default UI
                 ui.reset(new TwytchUI(activeMachine, &machineManager));
             }
             ui->setMidiInput(&midiInput);
            ui->setMenuIndex(savedMenuIndex);
            ui->setMenuSelection(savedMenuSelection);
            ui->setMidiDeviceIndex(savedMidiDeviceIndex);
            ui->setPresetIndex(savedPresetIndex);
            ui->scanPresets();

            lastMachine = activeMachine;
        }

        if (activeMachine) {
             // Cast and check keyon for MIDI monitor
             PBSynthMachine* pbsynthMachine3 = dynamic_cast<PBSynthMachine*>(activeMachine);
             CursynthMachine* cursynthMachine3 = dynamic_cast<CursynthMachine*>(activeMachine);
             TwytchsynthMachine* twytchMachine3 = dynamic_cast<TwytchsynthMachine*>(activeMachine);
             NcursesynthMachine* ncursesynthMachine3 = dynamic_cast<NcursesynthMachine*>(activeMachine);

             if (pbsynthMachine3 && pbsynthMachine3->getKeyOn()) {
                 ui->setMidiNote(pbsynthMachine3->getLastNote(), 127);
             } else if (cursynthMachine3 && cursynthMachine3->getKeyOn()) {
                 ui->setMidiNote(cursynthMachine3->getLastNote(), 127);
             } else if (twytchMachine3 && twytchMachine3->getKeyOn()) {
                 ui->setMidiNote(twytchMachine3->getLastNote(), 127);
             } else if (ncursesynthMachine3 && ncursesynthMachine3->getKeyOn()) {
                 ui->setMidiNote(ncursesynthMachine3->getLastNote(), 127);
             }
         }

        ui->draw();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    endwin();
    ui->stop();
    } // end else (UI mode)

    std::cout << "\nShutting down..." << std::endl;
    midiInput.stop();
    if (tcpMidi) tcpMidi->stop();
    if (audioEngine) {
        audioEngine->shutdown();
        delete audioEngine;
    }
    if (captureDriver) {
        captureDriver->stop();
        delete captureDriver;
    }
    if (!audioEngine) delete synth;
    delete tcpMidi;

    if (analyzer) {
        uint64_t samples = analyzer->getTotalSamples();
        analyzer->finalize();
        if (samples > 0 && !analyzer->failed()) {
            std::string apath = analyzer->getAnalysisPath();
            std::cout << "Analysis written to " << apath
                      << " (" << samples << " samples, " << (samples / 48000) << "s)" << std::endl;
            std::ifstream afile(apath);
            if (afile.is_open()) {
                std::string line;
                while (std::getline(afile, line)) {
                    std::cout << "  " << line << std::endl;
                }
            }
        } else if (analyzer->failed()) {
            std::cout << "Warning: could not write audio capture (read-only filesystem?)" << std::endl;
        } else {
            std::cout << "No audio captured" << std::endl;
        }
        delete analyzer;
    }

    if (midiCap) {
        int count = midiCap->getEventCount();
        midiCap->finalize();
        if (!midiCap->failed()) {
            std::string mpath = midiCap->getPath();
            std::cout << "MIDI log written to " << mpath
                      << " (" << count << " events)" << std::endl;
            std::ifstream mfile(mpath);
            if (mfile.is_open()) {
                std::string line;
                while (std::getline(mfile, line))
                    std::cout << "  " << line << std::endl;
            }
        } else {
            std::cout << "Warning: could not write MIDI capture" << std::endl;
        }
        delete midiCap;
    }

    std::cout << "Virtual Synthesizer exited cleanly." << std::endl;
    return 0;
}
