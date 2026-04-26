#ifndef UI_DRAW_H
#define UI_DRAW_H

#include "../synth/synth_architecture.h"
#include "ui_parameters.h"

namespace UIDraw {
    void drawTitle();
    void drawStatus(SynthArchitecture* synth);
    void drawFilterSection(SynthArchitecture* synth, Parameter selectedParam);
    void drawAmplitudeEnvelopeSection(SynthArchitecture* synth, Parameter selectedParam);
    void drawFilterEnvelopeSection(SynthArchitecture* synth, Parameter selectedParam);
    void drawOscillatorSection(SynthArchitecture* synth, Parameter selectedParam);
    void drawDelaySection(SynthArchitecture* synth, Parameter selectedParam);
    void drawReverbSection(SynthArchitecture* synth, Parameter selectedParam);
    void drawChorusSection(SynthArchitecture* synth, Parameter selectedParam);
    void drawDistortionSection(SynthArchitecture* synth, Parameter selectedParam);
    void drawMasterSection(SynthArchitecture* synth, Parameter selectedParam);
    void drawVoiceDisplay(SynthArchitecture* synth);
    void drawMidiSection(SynthArchitecture* synth, Parameter selectedParam);  // Add this
    void drawKeyboard();
    void drawControls();
    void drawSelectedParameter(Parameter param);
}

#endif
