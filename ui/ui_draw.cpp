#include "ui_draw.h"
#include "ui_parameters.h"
#include "ui_layout.h"
#include "ui_colors.h"
#include "ui_constants.h"
#include "midi/midi_input.h"
#include "midi/midi_mapping.h"
#include "../preset/preset_manager.h"
#include <ncurses.h>



void UIDraw::drawMidiSection(SynthArchitecture* synth, Parameter selectedParam) {
    int row = UILayout::MIDI_SECTION_ROW;
    int col = UILayout::RIGHT_COL;
    
    attron(A_BOLD);
    mvprintw(row, col, "=== MIDI ===");
    attroff(A_BOLD);
    row++;
    
    MidiInput* midi = synth->getMidiInput();
    if (midi) {
        // MIDI Enable/Disable
        if (selectedParam == Parameter::MIDI_ENABLE) {
            attron(A_REVERSE);
            attron(COLOR_PAIR(UIColor::SELECTED));
        }
        mvprintw(row, col, "%-18s: %s", "ENABLE", 
                 UIParameters::getBoolText(midi->isRunning()));
        if (selectedParam == Parameter::MIDI_ENABLE) {
            attroff(COLOR_PAIR(UIColor::SELECTED));
            attroff(A_REVERSE);
        }
        row++;
        
        // MIDI Device Selection
        if (selectedParam == Parameter::MIDI_DEVICE) {
            attron(A_REVERSE);
            attron(COLOR_PAIR(UIColor::SELECTED));
        }
        
        int currentPort = midi->getSelectedPort();
        if (currentPort >= 0 && currentPort < midi->getDeviceCount()) {
            mvprintw(row, col, "%-18s: %s", "DEVICE", 
                     midi->getDeviceName(currentPort).c_str());
        } else {
            mvprintw(row, col, "%-18s: %s", "DEVICE", "NONE");
        }
        
        if (selectedParam == Parameter::MIDI_DEVICE) {
            attroff(COLOR_PAIR(UIColor::SELECTED));
            attroff(A_REVERSE);
        }
        row++;
        
        // MIDI Mapping Selection
        if (selectedParam == Parameter::MIDI_MAPPING) {
            attron(A_REVERSE);
            attron(COLOR_PAIR(UIColor::SELECTED));
        }
        
        MappingManager* mm = midi->getMappingManager();
        if (mm && mm->getMappingCount() > 0) {
            mvprintw(row, col, "%-18s: %s", "MAPPING", 
                     mm->getMappingName(mm->getCurrentMappingIndex()).c_str());
        } else {
            mvprintw(row, col, "%-18s: %s", "MAPPING", "NONE");
        }
        
        if (selectedParam == Parameter::MIDI_MAPPING) {
            attroff(COLOR_PAIR(UIColor::SELECTED));
            attroff(A_REVERSE);
        }
        row++;
        
        // Device info line
        if (midi->getDeviceCount() > 0) {
            mvprintw(row, col, "%-18s: %d devices", "AVAILABLE", midi->getDeviceCount());
        }
        row++;
        
        // MIDI Monitor - show last received MIDI message
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - midi->getLastActivity()).count();
        
        if (elapsed < 2000 && midi->getLastCC() >= 0) {
            int lastCC = midi->getLastCC();
            int lastVal = midi->getLastCCValue();
            
            if (lastCC < 128) {
                mvprintw(row, col, "%-18s: CC%3d = %3d", "MONITOR", lastCC, lastVal);
            } else if (lastCC < 256) {
                mvprintw(row, col, "%-18s: Note%3d ON  %3d", "MONITOR", lastCC - 128, lastVal);
            } else {
                mvprintw(row, col, "%-18s: Note%3d OFF %3d", "MONITOR", lastCC - 256, lastVal);
            }
        }
    } else {
        mvprintw(row, col, "%-18s: %s", "MIDI", "NOT AVAILABLE");
    }
}


void UIDraw::drawVoiceDisplay(SynthArchitecture* synth) {
    int startRow = UILayout::VOICE_START_ROW;
    int col = UILayout::LEFT_COL;

    // Draw voice monitor header
    attron(A_BOLD);
    mvprintw(startRow, col, "=== VOICE MONITOR ===");
    attroff(A_BOLD);
    startRow++;

    // Get voices
    const auto& voices = synth->getVoices();
    int polyphony = synth->getPolyphony();

    // Draw each voice status
    for (int i = 0; i < polyphony; i++) {
        const auto& voice = voices[i];
        bool isActive = voice->isActive();
        float freq = voice->getFrequency();
        float envelopeLevel = voice->getEnvelopeLevel();
        // int noteId = voice->getNoteId();  // Remove unused variable or comment out

        // Convert frequency to note name
        const char* noteName = "---";
        for (int n = 0; n < 12; n++) {
            if (std::abs(freq - NOTE_FREQUENCIES[n]) < 1.0f) {
                noteName = NOTE_NAMES[n];
                break;
            }
        }

        // Choose color based on voice activity
        if (isActive) {
            attron(COLOR_PAIR(UIColor::STATUS_OK));
            attron(A_BOLD);
        } else {
            attron(COLOR_PAIR(UIColor::SELECTED));
        }

        // Draw voice box
        mvprintw(startRow + i, col, "Voice %d: ", i + 1);

        if (isActive) {
            // Draw active voice with note info
            printw("[ACTIVE] ");
            printw("Note: %s ", noteName);
            printw("(%.1f Hz) ", freq);

            // Draw envelope level bar
            int levelBar = (int)(envelopeLevel * 10);
            printw("Env: [");
            for (int b = 0; b < levelBar; b++) addch('=');
            for (int b = levelBar; b < 10; b++) addch(' ');
            printw("] %3d%%", (int)(envelopeLevel * 100));
        } else {
            // Draw inactive voice
            printw("[IDLE]   ---");
        }

        if (isActive) {
            attroff(A_BOLD);
            attroff(COLOR_PAIR(UIColor::STATUS_OK));
        } else {
            attroff(COLOR_PAIR(UIColor::SELECTED));
        }
    }

    // Draw voice usage summary
    int activeCount = 0;
    for (int i = 0; i < polyphony; i++) {
        if (voices[i]->isActive()) activeCount++;
    }

    startRow += polyphony + 1;
    attron(A_BOLD);
    mvprintw(startRow, col, "Active Voices: %d / %d", activeCount, polyphony);
    attroff(A_BOLD);
}

void UIDraw::drawTitle() {
    attron(A_BOLD);
    mvprintw(UILayout::TITLE_ROW, 2, "=== POLYPHONIC VIRTUAL ANALOG SYNTHESIZER ===");
    attroff(A_BOLD);
    mvprintw(UILayout::SUBTITLE_ROW, 2, "Dual Envelope | 4 Effects | Polyphonic");
    mvprintw(UILayout::DIVIDER_ROW, 2, "===============================================");
}

void UIDraw::drawStatus(SynthArchitecture* synth) {
    attron(A_REVERSE);
    if (synth->isPlaying()) {
        mvprintw(UILayout::STATUS_ROW, 2, " NOTES PLAYING - ENVELOPES ACTIVE    ");
    } else {
        mvprintw(UILayout::STATUS_ROW, 2, "      NO NOTES PLAYING              ");
    }
    attroff(A_REVERSE);
}

void UIDraw::drawFilterSection(SynthArchitecture* synth, Parameter selectedParam) {
    int row = UILayout::FILTER_SECTION_ROW;
    int col = UILayout::LEFT_COL;
    
    attron(A_BOLD);
    mvprintw(row, col, "=== FILTER SECTION ===");
    attroff(A_BOLD);
    row++;
    
    // Polyphony
    if (selectedParam == Parameter::POLYPHONY) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-20s: %2d voices", "POLYPHONY", synth->getPolyphony());
    if (selectedParam == Parameter::POLYPHONY) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
    row++;
    
    // Filter Type
    if (selectedParam == Parameter::FILTER_TYPE) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-20s: %-15s", "FILTER TYPE", UIParameters::getFilterTypeName(synth));
    if (selectedParam == Parameter::FILTER_TYPE) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
    row++;
    
    // Cutoff
    float cutoff = synth->getCutoff();
    if (selectedParam == Parameter::CUTOFF) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-20s: %7.1f Hz", "CUTOFF", cutoff);
    if (selectedParam == Parameter::CUTOFF) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
    row++;
    
    // Cutoff bar
    int cutoffBar = (int)((cutoff / 8000.0f) * 30);
    if (cutoffBar > 30) cutoffBar = 30;
    mvprintw(row, col + 2, "[");
    for (int i = 0; i < cutoffBar; i++) addch('=');
    for (int i = cutoffBar; i < 30; i++) addch(' ');
    mvprintw(row, col + 34, "]");
    row++;
    
    // Resonance
    float resonance = synth->getResonance();
    if (selectedParam == Parameter::RESONANCE) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-20s: %5.2f", "RESONANCE", resonance);
    if (selectedParam == Parameter::RESONANCE) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
    row++;
    
    // HPF Frequency
    float hpfFreq = synth->getHPFCutoff();
    if (selectedParam == Parameter::HPF_FREQ) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-20s: %5.0f Hz", "HPF FREQ", hpfFreq);
    if (selectedParam == Parameter::HPF_FREQ) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
    row++;
    
    // Resonance bar
    int resBar = (int)(resonance * 30);
    mvprintw(row, col + 2, "{");
    for (int i = 0; i < resBar; i++) addch('#');
    for (int i = resBar; i < 30; i++) addch(' ');
    mvprintw(row, col + 34, "}");
    row++;
    
    // Filter Envelope Amount
    float filterEnvAmount = synth->getFilterEnvelopeAmount();
    if (selectedParam == Parameter::FILTER_ENV_AMOUNT) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-20s: %5.2f", "FILTER ENV AMOUNT", filterEnvAmount);
    if (selectedParam == Parameter::FILTER_ENV_AMOUNT) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
    row++;
    
    int envAmountBar = (int)(filterEnvAmount * 30);
    mvprintw(row, col + 2, "(");
    for (int i = 0; i < envAmountBar; i++) addch('*');
    for (int i = envAmountBar; i < 30; i++) addch(' ');
    mvprintw(row, col + 34, ")");
}

void UIDraw::drawAmplitudeEnvelopeSection(SynthArchitecture* synth, Parameter selectedParam) {
    int row = UILayout::AMP_ENV_SECTION_ROW;
    int col = UILayout::LEFT_COL;
    
    attron(A_BOLD);
    mvprintw(row, col, "=== AMPLITUDE ENVELOPE ===");
    attroff(A_BOLD);
    row++;
    
    float attack = synth->getAmpAttack();
    float decay = synth->getAmpDecay();
    float sustain = synth->getAmpSustain();
    float release = synth->getAmpRelease();
    
    if (selectedParam == Parameter::AMP_ATTACK) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-20s: %5.2f", "ATTACK", attack);
    if (selectedParam == Parameter::AMP_ATTACK) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
    row++;
    
    if (selectedParam == Parameter::AMP_DECAY) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-20s: %5.2f", "DECAY", decay);
    if (selectedParam == Parameter::AMP_DECAY) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
    row++;
    
    if (selectedParam == Parameter::AMP_SUSTAIN) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-20s: %5.2f", "SUSTAIN", sustain);
    if (selectedParam == Parameter::AMP_SUSTAIN) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
    row++;
    
    if (selectedParam == Parameter::AMP_RELEASE) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-20s: %5.2f", "RELEASE", release);
    if (selectedParam == Parameter::AMP_RELEASE) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
    row++;
    
    // AMP ENV CURVE - This was missing!
    const char* curveName = UIParameters::getCurveName(synth->getAmpEnvelopeCurve());
    if (selectedParam == Parameter::AMP_ENV_CURVE) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-20s: %-15s", "CURVE", curveName);
    if (selectedParam == Parameter::AMP_ENV_CURVE) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
}

void UIDraw::drawFilterEnvelopeSection(SynthArchitecture* synth, Parameter selectedParam) {
    int row = UILayout::FILTER_ENV_SECTION_ROW;
    int col = UILayout::LEFT_COL;
    
    attron(A_BOLD);
    mvprintw(row, col, "=== FILTER ENVELOPE ===");
    attroff(A_BOLD);
    row++;
    
    float attack = synth->getFilterAttack();
    float decay = synth->getFilterDecay();
    float sustain = synth->getFilterSustain();
    float release = synth->getFilterRelease();
    
    if (selectedParam == Parameter::FILTER_ATTACK) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-20s: %5.2f", "ATTACK", attack);
    if (selectedParam == Parameter::FILTER_ATTACK) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
    row++;
    
    if (selectedParam == Parameter::FILTER_DECAY) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-20s: %5.2f", "DECAY", decay);
    if (selectedParam == Parameter::FILTER_DECAY) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
    row++;
    
    if (selectedParam == Parameter::FILTER_SUSTAIN) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-20s: %5.2f", "SUSTAIN", sustain);
    if (selectedParam == Parameter::FILTER_SUSTAIN) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
    row++;
    
    if (selectedParam == Parameter::FILTER_RELEASE) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-20s: %5.2f", "RELEASE", release);
    if (selectedParam == Parameter::FILTER_RELEASE) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
    row++;
    
    // FILTER ENV CURVE - This was missing!
    const char* curveName = UIParameters::getCurveName(synth->getFilterEnvelopeCurve());
    if (selectedParam == Parameter::FILTER_ENV_CURVE) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-20s: %-15s", "CURVE", curveName);
    if (selectedParam == Parameter::FILTER_ENV_CURVE) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
}

void UIDraw::drawOscillatorSection(SynthArchitecture* synth, Parameter selectedParam) {
    int row = UILayout::OSCILLATOR_SECTION_ROW;
    int col = UILayout::RIGHT_COL;
    
    attron(A_BOLD);
    mvprintw(row, col, "=== OSCILLATOR ===");
    attroff(A_BOLD);
    row++;
    
    if (selectedParam == Parameter::WAVEFORM) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-18s: %-10s", "WAVEFORM", UIParameters::getWaveformName(synth));
    if (selectedParam == Parameter::WAVEFORM) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
    row++;
    
    if (selectedParam == Parameter::OSC_MIX) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-18s: %5.2f", "OSC MIX", synth->getOscMix());
    if (selectedParam == Parameter::OSC_MIX) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
    row++;
    
    if (selectedParam == Parameter::OSC2_DETUNE) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-18s: %+5.2f st", "OSC2 DETUNE", synth->getOsc2Detune());
    if (selectedParam == Parameter::OSC2_DETUNE) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
}

void UIDraw::drawPresetSection(SynthArchitecture* synth, Parameter selectedParam) {
    int row = UILayout::PRESET_SECTION_ROW;
    int col = UILayout::RIGHT_COL;
    
    attron(A_BOLD);
    mvprintw(row, col, "=== PRESET ===");
    attroff(A_BOLD);
    row++;
    
    PresetManager* pm = synth->getPresetManager();
    
    if (selectedParam == Parameter::PRESET_LOAD) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    if (pm && pm->exists()) {
        mvprintw(row, col, "%-18s: %s", "LOAD", pm->getPresetName(pm->getCurrentPresetIndex()).c_str());
    } else {
        mvprintw(row, col, "%-18s: %s", "LOAD", "NONE");
    }
    if (selectedParam == Parameter::PRESET_LOAD) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
    row++;
    
    if (selectedParam == Parameter::PRESET_SAVE) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-18s: %s", "SAVE", "current");
    if (selectedParam == Parameter::PRESET_SAVE) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
}

void UIDraw::drawDelaySection(SynthArchitecture* synth, Parameter selectedParam) {
    int row = UILayout::DELAY_SECTION_ROW;
    int col = UILayout::RIGHT_COL;
    
    attron(A_BOLD);
    mvprintw(row, col, "=== DELAY ===");
    attroff(A_BOLD);
    row++;
    
    if (selectedParam == Parameter::DELAY_ENABLE) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-18s: %s", "ENABLE", UIParameters::getBoolText(synth->getDelay()->isEnabled()));
    if (selectedParam == Parameter::DELAY_ENABLE) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
    row++;
    
    if (selectedParam == Parameter::DELAY_TIME) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-18s: %5.1f ms", "TIME", synth->getDelay()->getDelayTime());
    if (selectedParam == Parameter::DELAY_TIME) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
    row++;
    
    if (selectedParam == Parameter::DELAY_FEEDBACK) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-18s: %5.2f", "FEEDBACK", synth->getDelay()->getFeedback());
    if (selectedParam == Parameter::DELAY_FEEDBACK) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
    row++;
    
    if (selectedParam == Parameter::DELAY_MIX) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-18s: %5.2f", "MIX", synth->getDelay()->getMix());
    if (selectedParam == Parameter::DELAY_MIX) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
}

void UIDraw::drawReverbSection(SynthArchitecture* synth, Parameter selectedParam) {
    int row = UILayout::REVERB_SECTION_ROW;
    int col = UILayout::RIGHT_COL;
    
    attron(A_BOLD);
    mvprintw(row, col, "=== REVERB ===");
    attroff(A_BOLD);
    row++;
    
    if (selectedParam == Parameter::REVERB_ENABLE) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-18s: %s", "ENABLE", UIParameters::getBoolText(synth->getReverb()->isEnabled()));
    if (selectedParam == Parameter::REVERB_ENABLE) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
    row++;
    
    if (selectedParam == Parameter::REVERB_DECAY) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-18s: %5.2f", "DECAY", synth->getReverb()->getDecay());
    if (selectedParam == Parameter::REVERB_DECAY) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
    row++;
    
    if (selectedParam == Parameter::REVERB_MIX) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-18s: %5.2f", "MIX", synth->getReverb()->getMix());
    if (selectedParam == Parameter::REVERB_MIX) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
}

void UIDraw::drawChorusSection(SynthArchitecture* synth, Parameter selectedParam) {
    int row = UILayout::CHORUS_SECTION_ROW;
    int col = UILayout::RIGHT_COL;
    
    attron(A_BOLD);
    mvprintw(row, col, "=== CHORUS ===");
    attroff(A_BOLD);
    row++;
    
    if (selectedParam == Parameter::CHORUS_ENABLE) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-18s: %s", "ENABLE", UIParameters::getBoolText(synth->getChorus()->isEnabled()));
    if (selectedParam == Parameter::CHORUS_ENABLE) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
    row++;
    
    if (selectedParam == Parameter::CHORUS_DEPTH) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-18s: %5.2f", "DEPTH", synth->getChorus()->getDepth());
    if (selectedParam == Parameter::CHORUS_DEPTH) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
    row++;
    
    if (selectedParam == Parameter::CHORUS_RATE) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-18s: %5.2f Hz", "RATE", synth->getChorus()->getRate());
    if (selectedParam == Parameter::CHORUS_RATE) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
    row++;
    
    if (selectedParam == Parameter::CHORUS_MIX) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-18s: %5.2f", "MIX", synth->getChorus()->getMix());
    if (selectedParam == Parameter::CHORUS_MIX) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
}

void UIDraw::drawDistortionSection(SynthArchitecture* synth, Parameter selectedParam) {
    int row = UILayout::DISTORTION_SECTION_ROW;
    int col = UILayout::RIGHT_COL;
    
    attron(A_BOLD);
    mvprintw(row, col, "=== DISTORTION ===");
    attroff(A_BOLD);
    row++;
    
    if (selectedParam == Parameter::DISTORTION_ENABLE) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-18s: %s", "ENABLE", UIParameters::getBoolText(synth->getDistortion()->isEnabled()));
    if (selectedParam == Parameter::DISTORTION_ENABLE) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
    row++;
    
    if (selectedParam == Parameter::DISTORTION_DRIVE) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-18s: %5.2f", "DRIVE", synth->getDistortion()->getDrive());
    if (selectedParam == Parameter::DISTORTION_DRIVE) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
    row++;
    
    if (selectedParam == Parameter::DISTORTION_MIX) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-18s: %5.2f", "MIX", synth->getDistortion()->getMix());
    if (selectedParam == Parameter::DISTORTION_MIX) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
}

void UIDraw::drawMasterSection(SynthArchitecture* synth, Parameter selectedParam) {
    int row = UILayout::MASTER_SECTION_ROW;
    int col = UILayout::RIGHT_COL;
    
    attron(A_BOLD);
    mvprintw(row, col, "=== MASTER ===");
    attroff(A_BOLD);
    row++;
    
    float volume = synth->getVolume();
    if (selectedParam == Parameter::VOLUME) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }
    mvprintw(row, col, "%-18s: %5.2f", "VOLUME", volume);
    if (selectedParam == Parameter::VOLUME) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
    row++;
    
    int volBar = (int)(volume * 25);
    mvprintw(row, col + 2, "|");
    for (int i = 0; i < volBar; i++) addch('|');
    for (int i = volBar; i < 25; i++) addch(' ');
    mvprintw(row, col + 29, "|");
}

void UIDraw::drawKeyboard() {
    int row = UILayout::KEYBOARD_ROW;
    
    mvprintw(row, 2, "KEYBOARD LAYOUT (C4 to B4):");
    row++;
    mvprintw(row, 2, "  A   Z   E   R   T   Y   U   I   O   P   ^   $");
    row++;
    mvprintw(row, 2, "  C   C#  D   D#  E   F   F#  G   G#  A   A#  B");
    
    // Add note: Press ESC to clear all notes
    row += 2;
    mvprintw(row, 2, "Press ESC to stop all playing notes");
}


void UIDraw::drawControls() {
    int row = UILayout::CONTROLS_ROW;

    attron(A_BOLD);
    mvprintw(row, 2, "=== CONTROLS ===");
    attroff(A_BOLD);
    row++;
    mvprintw(row, 4, "NAVIGATION:");
    row++;
    mvprintw(row, 6, "[Up/Down/Left/Right] - Move through all parameters");
    row++;
    mvprintw(row, 4, "LOAD PRESET:");
    row++;
    mvprintw(row, 6, "[+] or [-] - Cycle presets");
    row++;
    mvprintw(row, 4, "ADJUST VALUES:");
    row++;
    mvprintw(row, 6, "[PageUp]   - Increase value");
    row++;
    mvprintw(row, 6, "[PageDown] - Decrease value");
    row++;
    mvprintw(row, 4, "PLAY NOTES:");
    row++;
    mvprintw(row, 6, "[A..$ keys] - Press once to play, again to stop");
    row++;
    mvprintw(row, 4, "VOICE CONTROL:");
    row++;
    mvprintw(row, 6, "[F5] - PANIC (stop all notes immediately)");
    row++;
    mvprintw(row, 4, "OTHER:");
    row++;
    mvprintw(row, 6, "[ESC] - Stop all playing notes (graceful)");
    row++;
    mvprintw(row, 6, "[Ctrl+C] - Exit program");
}

void UIDraw::drawSelectedParameter(Parameter param) {
    int row = UILayout::SELECTED_ROW;
    
    attron(A_BOLD);
    attron(COLOR_PAIR(UIColor::SELECTED));
    mvprintw(row, 2, " SELECTED: %-24s ", UIParameters::getParameterName(param));
    attroff(COLOR_PAIR(UIColor::SELECTED));
    attroff(A_BOLD);
    row++;
    mvprintw(row, 2, "Press PageUp to increase, PageDown to decrease the selected parameter");
}

void UIDraw::drawPresetBrowser(SynthArchitecture* synth, int selectedIndex, const std::string& saveName, bool isSaving) {
    int height = 20;
    int width = 50;
    int startY = (LINES - height) / 2;
    int startX = (COLS - width) / 2;
    
    // Draw border
    attron(A_BOLD);
    mvprintw(startY, startX, "+------------------------------------------------+");
    for (int i = 1; i < height - 1; i++) {
        mvprintw(startY + i, startX, "|");
        mvprintw(startY + i, startX + width - 1, "|");
    }
    mvprintw(startY + height - 1, startX, "+------------------------------------------------+");
    attroff(A_BOLD);
    
    // Title
    if (isSaving) {
        attron(A_BOLD);
        mvprintw(startY + 1, startX + 15, " SAVE PRESET ");
        attroff(A_BOLD);
        mvprintw(startY + 3, startX + 2, "Name: %s", saveName.c_str());
        mvprintw(startY + 4, startX + 2, "[Enter] to save, [Esc] to cancel");
    } else {
        attron(A_BOLD);
        mvprintw(startY + 1, startX + 15, " PRESET BROWSER ");
        attroff(A_BOLD);
        
        // Draw preset list
        PresetManager* pm = synth->getPresetManager();
        int presetCount = pm->getPresetCount();
        int displayCount = std::min(presetCount, 15);
        
        for (int i = 0; i < displayCount; i++) {
            int y = startY + 3 + i;
            if (i == selectedIndex) {
                attron(A_REVERSE);
                attron(COLOR_PAIR(UIColor::SELECTED));
            }
            std::string name = pm->getPresetName(i);
            if (name.length() > 40) name = name.substr(0, 40);
            mvprintw(y, startX + 2, "%-40s", name.c_str());
            if (i == selectedIndex) {
                attroff(COLOR_PAIR(UIColor::SELECTED));
                attroff(A_REVERSE);
            }
        }
        
        mvprintw(startY + height - 3, startX + 2, "[Up/Down] Navigate  [l] Load  [s] Save  [n] New  [Esc] Exit");
    }
}
