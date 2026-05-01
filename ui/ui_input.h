#ifndef UI_INPUT_H
#define UI_INPUT_H

#include "machine/Ncursesynth/synth/synth_architecture.h"
#include "ui_parameters.h"

namespace UIInput {
    void handleNavigation(int ch, Parameter& selectedParam);
    void handleNoteInput(int ch, SynthArchitecture* synth);
    void handleNoteRelease(int ch, SynthArchitecture* synth);
    void clearAllNotes(SynthArchitecture* synth);
    void checkKeyReleases(SynthArchitecture* synth);
    void panicButton(SynthArchitecture* synth);  // New panic function
    void releaseVoice(int voiceNumber, SynthArchitecture* synth);  // Manual voice release
}

#endif
