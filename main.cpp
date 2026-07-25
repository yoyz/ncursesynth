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
#include "ui/ncurses_renderer.h"
#include "version.h"
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

static bool g_pcKeyboardMode = false;

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
    std::string mappingName;
    int fpsLimit = 30;
    int bufferSize = 256;
    double latencyMs = 20.0;
    float limiterThreshold = 0.85f;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0) {
            std::cout << "ncursesynth v" << VERSION_STRING << std::endl;
            return 0;
        }
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            std::cout << "ncursesynth v" << VERSION_STRING << " - Virtual Analog Synthesizer\n\n";
            std::cout << "Usage: virtual_synth [options]\n\n";
            std::cout << "Options:\n";
            std::cout << "  --version, -v        Show version\n";
            std::cout << "  --help, -h           Show this help message\n";
            std::cout << "  --midi-debug         Enable MIDI debug output\n";
            std::cout << "  --midi-port hw:X,Y,Z Select MIDI port on startup (e.g., hw:1,0,0)\n";
            std::cout << "  --list-midi         List available MIDI devices\n";
            std::cout << "  --tcp-midi-port N   TCP port for remote MIDI input\n";
            std::cout << "  --tcp-capture-audio N  TCP port for audio capture (replaces PortAudio)\n";
            std::cout << "  --capture-audio-plus-fft-rms FILE  Write raw audio + per-second FFT/RMS analysis\n";
            std::cout << "  --capture-midi-plus-analysis FILE  Write MIDI event log (note_on/off/cc) with timestamps\n";
            std::cout << "  --synthengine NAME  Run headless with named engine (ncursesynth/pbsynth/cursynth/twytch)\n";
            std::cout << "  --mapping NAME      Set MIDI mapping (e.g., deepmind12, summit)\n";
            std::cout << "  --fps N             Max refresh rate (default 30)\n";
            std::cout << "  --buffer-size N     Audio buffer size in frames (16-4096, default 256, power-of-2)\n";
            std::cout << "  --latency-ms N      PortAudio suggested latency in ms (1-200, default 20)\n";
            std::cout << "  --limiter-threshold N  Limiter threshold 0.0-1.0 (default 0.85, lower = less headroom)\n";
            std::cout << "  --pc-keyboard         Enable PC keyboard as piano (AZERTY layout)\n";
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
        if (strcmp(argv[i], "--mapping") == 0 && i + 1 < argc) {
            mappingName = argv[++i];
        }
        if (strcmp(argv[i], "--fps") == 0 && i + 1 < argc) {
            fpsLimit = atoi(argv[++i]);
            if (fpsLimit < 1) fpsLimit = 1;
            if (fpsLimit > 200) fpsLimit = 200;
        }
        if (strcmp(argv[i], "--buffer-size") == 0 && i + 1 < argc) {
            bufferSize = atoi(argv[++i]);
            if (bufferSize < 16) bufferSize = 16;
            if (bufferSize > 4096) bufferSize = 4096;
            // Ensure power of 2
            int p2 = 1;
            while (p2 < bufferSize) p2 <<= 1;
            bufferSize = p2;
        }
        if (strcmp(argv[i], "--latency-ms") == 0 && i + 1 < argc) {
            latencyMs = atof(argv[++i]);
            if (latencyMs < 1.0) latencyMs = 1.0;
            if (latencyMs > 200.0) latencyMs = 200.0;
        }
        if (strcmp(argv[i], "--pc-keyboard") == 0) {
            g_pcKeyboardMode = true;
        }
        if (strcmp(argv[i], "--limiter-threshold") == 0 && i + 1 < argc) {
            limiterThreshold = atof(argv[++i]);
            if (limiterThreshold < 0.1f) limiterThreshold = 0.1f;
            if (limiterThreshold > 1.0f) limiterThreshold = 1.0f;
        }
    }

    bool headless = !synthEngineName.empty();

    if (listMidi) {
        MidiInput tmpMidi(nullptr);
        if (!tmpMidi.initialize()) {
            std::cerr << "Error: Cannot initialize MIDI." << std::endl;
            return 1;
        }
        std::cout << "Available MIDI devices:\n";
        int count = tmpMidi.getDeviceCount();
        for (int i = 0; i < count; i++) {
            std::string name = tmpMidi.getDeviceName(i);
            if (!name.empty())
                std::cout << "  " << i << ": " << name << "\n";
        }
        if (count == 0) std::cout << "(no MIDI devices found)\n";
        return 0;
    }

    signal(SIGINT, signalHandler);

    std::cout << "ncursesynth v" << VERSION_STRING << " - Virtual Analog Synthesizer\n";
    std::cout << "TAB: Switch Engine | ARROWS: Navigate Params | Q: Quit\n\n";

    MachineManager machineManager;
    machineManager.registerMachine(new NcursesynthMachine());
    machineManager.registerMachine(new PBSynthMachine());
    machineManager.registerMachine(new CursynthMachine(8));
    machineManager.registerMachine(new TwytchsynthMachine());

    machineManager.setCurrentMachine(0);
    Machine* activeMachine = machineManager.getCurrentMachine();
    std::cout << "Selected engine: " << activeMachine->getName() << "\n" << std::endl;

    if (activeMachine) activeMachine->init();

    SynthArchitecture* tempSynth = new SynthArchitecture(8, 48000);
    MidiInput midiInput(tempSynth);
    tempSynth->setMidiInput(&midiInput);

    midiInput.loadMappings();
    if (!mappingName.empty()) {
        auto* manager = midiInput.getMappingManager();
        for (int i = 0; i < manager->getMappingCount(); i++) {
            if (manager->getMappingName(i) == mappingName) {
                manager->setCurrentMapping(i);
                break;
            }
        }
    }

    CaptureAnalyzer* analyzer = nullptr;
    if (!captureAnalysisPath.empty()) {
        analyzer = new CaptureAnalyzer(captureAnalysisPath, 48000);
        CaptureAnalyzer::setInstance(analyzer);
    }

    MidiCapture* midiCap = nullptr;
    if (!midiCapturePath.empty()) {
        midiCap = new MidiCapture(midiCapturePath);
        MidiCapture::setInstance(midiCap);
    }

    AudioCaptureDriver* captureDriver = nullptr;
    AudioEngine* audioEngine = nullptr;
    SynthArchitecture* synth = nullptr;

    if (captureAudioPort > 0) {
        captureDriver = new AudioCaptureDriver(captureAudioPort, 48000);
        captureDriver->limiter.setThreshold(limiterThreshold);
        if (!captureDriver->start(activeMachine)) {
            std::cerr << "Failed to start audio capture driver!" << std::endl;
            delete captureDriver;
            return 1;
        }
        synth = new SynthArchitecture(8, 48000);
    } else {
        audioEngine = new AudioEngine(48000, bufferSize, latencyMs, limiterThreshold);
        if (!audioEngine->initialize()) {
            std::cerr << "Failed to initialize audio engine!" << std::endl;
            return 1;
        }
        synth = audioEngine->getSynth();
    }

    if (midiInput.initialize()) {
        if (!midiPort.empty()) {
            int selectedIndex = -1;
            if (midiPort.substr(0, 3) == "hw:") {
                int portNum = atoi(midiPort.substr(3).c_str());
                if (portNum >= 0 && portNum < midiInput.getDeviceCount())
                    selectedIndex = portNum;
            } else {
                bool isNumeric = !midiPort.empty() && midiPort.find_first_not_of("0123456789") == std::string::npos;
                if (isNumeric)
                    selectedIndex = atoi(midiPort.c_str());
            }
            if (selectedIndex < 0 || selectedIndex >= midiInput.getDeviceCount()) {
                for (int i = 0; i < midiInput.getDeviceCount(); i++) {
                    std::string name = midiInput.getDeviceName(i);
                    if (name.find(midiPort) != std::string::npos) {
                        selectedIndex = i;
                        break;
                    }
                }
            }
            if (selectedIndex >= 0) {
                midiInput.selectDevice(selectedIndex);
                midiInput.setMachine(activeMachine);
                midiInput.setMappingMachine(activeMachine);
                midiInput.start();
            }
        }
    }

    if (midiDebug) {
        midiInput.setMidiDebug(true);
        if (activeMachine) activeMachine->setMidiDebug(true);
    }

    TcpMidiServer* tcpMidi = nullptr;
    if (tcpMidiPort > 0) {
        tcpMidi = new TcpMidiServer(tcpMidiPort, &machineManager);
        tcpMidi->start();
    }

    if (audioEngine) {
        audioEngine->setMachine(activeMachine);
        if (!audioEngine->start()) {
            std::cerr << "Failed to start audio!" << std::endl;
            audioEngine->shutdown();
            return 1;
        }
        std::cout << "Audio started: " << bufferSize << " frames/buffer, " << latencyMs << "ms suggested latency" << std::endl;
    } else if (captureDriver) {
        captureDriver->setMachine(activeMachine);
    }

    if (headless) {
        while (running)
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
    } else {
        std::cout << "UI launching...\n" << std::endl;
        std::cout << "Press Ctrl+C to exit\n" << std::endl;

        NcursesRenderer renderer;
        renderer.init();
        renderer.setRefreshIntervalMs(1000 / fpsLimit);

        std::unique_ptr<MachineUI> ui;

        TwytchsynthMachine* twytchMachine = dynamic_cast<TwytchsynthMachine*>(activeMachine);
        PBSynthMachine* pbsynthMachine = dynamic_cast<PBSynthMachine*>(activeMachine);
        CursynthMachine* cursynthMachine = dynamic_cast<CursynthMachine*>(activeMachine);
        NcursesynthMachine* ncursesynthMachine = dynamic_cast<NcursesynthMachine*>(activeMachine);

        if (twytchMachine)
            ui.reset(new TwytchUI(activeMachine, &machineManager));
        else if (pbsynthMachine)
            ui.reset(new PBSynthUI(activeMachine, &machineManager));
        else if (cursynthMachine)
            ui.reset(new CursynthUI(activeMachine, &machineManager));
        else
            ui.reset(new NcursesynthUI(activeMachine, &machineManager));

        ui->setRenderer(&renderer);
        ui->setMidiInput(&midiInput);
        ui->setMidiDeviceIndex(midiInput.getSelectedPort());
        ui->setPcKeyboardMode(g_pcKeyboardMode);
        ui->init();
        ui->draw();

        Machine* lastMachine = activeMachine;
        while (running && ui->isActive()) {
            int ch = renderer.getKey();

            if (ch != Key::NONE)
                ui->handleInput(ch);

            ui->updateValues();

            activeMachine = machineManager.getCurrentMachine();
            if (activeMachine != lastMachine) {
                if (audioEngine) audioEngine->setMachine(activeMachine);
                if (captureDriver) captureDriver->setMachine(activeMachine);

                int savedMenuSelection = ui->getMenuSelection();
                int savedMenuIndex = ui->getMenuIndex();
                int savedMidiDeviceIndex = ui->getMidiDeviceIndex();
                int savedPresetIndex = ui->getPresetIndex();

                PBSynthMachine* pm2 = dynamic_cast<PBSynthMachine*>(activeMachine);
                CursynthMachine* cm2 = dynamic_cast<CursynthMachine*>(activeMachine);
                TwytchsynthMachine* tm2 = dynamic_cast<TwytchsynthMachine*>(activeMachine);
                NcursesynthMachine* nm2 = dynamic_cast<NcursesynthMachine*>(activeMachine);

                if (pm2) { midiInput.setMachine(pm2); midiInput.setMappingMachine(pm2); pm2->init(); }
                else if (cm2) { midiInput.setMachine(cm2); midiInput.setMappingMachine(cm2); cm2->init(); }
                else if (tm2) { midiInput.setMachine(tm2); midiInput.setMappingMachine(tm2); tm2->init(); }
                else if (nm2) { midiInput.setMachine(nm2); midiInput.setMappingMachine(nm2); nm2->init(); }

                if (tm2) ui.reset(new TwytchUI(activeMachine, &machineManager));
                else if (pm2) ui.reset(new PBSynthUI(activeMachine, &machineManager));
                else if (cm2) ui.reset(new CursynthUI(activeMachine, &machineManager));
                else ui.reset(new NcursesynthUI(activeMachine, &machineManager));

                ui->setRenderer(&renderer);
                ui->setMidiInput(&midiInput);
                ui->setMenuIndex(savedMenuIndex);
                ui->setMenuSelection(savedMenuSelection);
                ui->setMidiDeviceIndex(savedMidiDeviceIndex);
                ui->setPresetIndex(savedPresetIndex);
                ui->setPcKeyboardMode(g_pcKeyboardMode);
                ui->init();
                ui->scanPresets();

                lastMachine = activeMachine;
            }

            if (activeMachine) {
                int note = -1;
                if (auto* p = dynamic_cast<PBSynthMachine*>(activeMachine)) { if (p->getKeyOn()) note = p->getLastNote(); }
                else if (auto* c = dynamic_cast<CursynthMachine*>(activeMachine)) { if (c->getKeyOn()) note = c->getLastNote(); }
                else if (auto* t = dynamic_cast<TwytchsynthMachine*>(activeMachine)) { if (t->getKeyOn()) note = t->getLastNote(); }
                else if (auto* n = dynamic_cast<NcursesynthMachine*>(activeMachine)) { if (n->getKeyOn()) note = n->getLastNote(); }
                if (note >= 0) ui->setMidiNote(note, 127);
            }

            ui->draw();
            int sleepMs = (fpsLimit > 0) ? (1000 / fpsLimit) : 30;
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));
        }

        ui->stop();
    }

    std::cout << "\nShutting down..." << std::endl;
    midiInput.stop();
    if (tcpMidi) tcpMidi->stop();
    if (audioEngine) { audioEngine->shutdown(); delete audioEngine; }
    if (captureDriver) { captureDriver->stop(); delete captureDriver; }
    if (!audioEngine) delete synth;
    delete tcpMidi;
    delete analyzer;
    delete midiCap;
    std::cout << "Virtual Synthesizer exited cleanly." << std::endl;
    return 0;
}
