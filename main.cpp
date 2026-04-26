#include <iostream>
#include <signal.h>
#include <atomic>
#include <chrono>
#include <thread>
#include "audio/audio_engine.h"
#include "ui/ui.h"
#include "midi/midi_input.h"

std::atomic<bool> running(true);

void signalHandler(int sig) {
    if (sig == SIGINT) {
        running = false;
    }
}

int main() {
    signal(SIGINT, signalHandler);
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "  VIRTUAL ANALOG SYNTHESIZER v1.0" << std::endl;
    std::cout << "  with MIDI Support" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // Create audio engine
    AudioEngine audioEngine(48000, 256);
    
    if (!audioEngine.initialize()) {
        std::cerr << "Failed to initialize audio engine!" << std::endl;
        return 1;
    }
    
    // Get synth reference
    SynthArchitecture* synth = audioEngine.getSynth();
    
    // Load first preset (Electric Piano will be first in index)
    if (synth->getPresetManager()->exists()) {
        synth->getPresetManager()->loadPreset(0, synth);
    }
    
    // Initialize MIDI input (but don't auto-select device)
    MidiInput midiInput(synth);
    // After creating midiInput, add:
    synth->setMidiInput(&midiInput);

    if (midiInput.initialize()) {
        midiInput.listDevices();
        midiInput.loadMappings();
        std::cout << "✓ MIDI initialized. Use UI to select device.\n" << std::endl;
    } else {
        std::cout << "✗ Failed to initialize MIDI.\n" << std::endl;
    }

    
    // Store MIDI input reference in synth for UI access
    // We need to add a method to SynthArchitecture to return MIDI input pointer
    // For now, we'll make it accessible through a global or add to synth class
    
    // Create UI
    UI ui(synth);
    
    // Pass MIDI input to UI (need to add this to UI class)
    // For simplicity, let's make midiInput accessible globally or add to synth
    
    // Start audio
    if (!audioEngine.start()) {
        std::cerr << "Failed to start audio!" << std::endl;
        audioEngine.shutdown();
        return 1;
    }
    
    std::cout << "✓ Audio started successfully!" << std::endl;
    std::cout << "✓ UI launching...\n" << std::endl;
    std::cout << "Press Ctrl+C to exit\n" << std::endl;
    
    // Start UI (this will block until UI exits)
    ui.start();
    
    // Main loop - wait for exit
    while (running && ui.isRunning()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Cleanup
    std::cout << "\nShutting down..." << std::endl;
    midiInput.stop();
    ui.stop();
    audioEngine.shutdown();
    
    std::cout << "Virtual Synthesizer exited cleanly." << std::endl;
    return 0;
}
