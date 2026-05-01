#ifndef UI_DRAW_H
#define UI_DRAW_H

#include "../synth/synth_architecture.h"
#include "ui_parameters.h"

namespace UIDraw {
    void drawTitle();
    void drawStatus(SynthArchitecture* synth);

    void drawOscillatorSection(SynthArchitecture* synth, Parameter selectedParam);
    void drawLfoSection(SynthArchitecture* synth, Parameter selectedParam);
    void drawFilterSection(SynthArchitecture* synth, Parameter selectedParam);
    void drawAmpEnvelopeSection(SynthArchitecture* synth, Parameter selectedParam);
    void drawFilterEnvelopeSection(SynthArchitecture* synth, Parameter selectedParam);
    void drawPerformanceSection(SynthArchitecture* synth, Parameter selectedParam);
    void drawVolumeSection(SynthArchitecture* synth, Parameter selectedParam);
    void drawModulationMatrix(SynthArchitecture* synth, Parameter selectedParam);

    void drawLegacySections(SynthArchitecture* synth, Parameter selectedParam);

    void drawPresetSection(SynthArchitecture* synth, Parameter selectedParam);
    void drawDelaySection(SynthArchitecture* synth, Parameter selectedParam);
    void drawReverbSection(SynthArchitecture* synth, Parameter selectedParam);
    void drawChorusSection(SynthArchitecture* synth, Parameter selectedParam);
    void drawDistortionSection(SynthArchitecture* synth, Parameter selectedParam);
    void drawMasterSection(SynthArchitecture* synth, Parameter selectedParam);
    void drawVoiceDisplay(SynthArchitecture* synth);
    void drawMidiSection(SynthArchitecture* synth, Parameter selectedParam);
    void drawPresetBrowser(SynthArchitecture* synth, int selectedIndex, const std::string& saveName, bool isSaving);
    void drawKeyboard();
    void drawControls();
    void drawSelectedParameter(Parameter param);
    void drawHelp();
}

#endif