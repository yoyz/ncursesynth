#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include <cstdint>
#include <cmath>
#include <string>
#include <vector>

constexpr int NUM_SAMPLES = 1024;
constexpr float RMS_THRESHOLD = 0.01f;
constexpr float CLIP_THRESHOLD = 0.99f;
constexpr float FREQUENCY_TOLERANCE = 1.0f;

bool runSoundProductionTests(class Machine* machine, bool useFFT);
bool runVolumeSilenceTest(class Machine* machine, bool useFFT);
bool runVolumeNoClipTest(class Machine* machine, bool useFFT);
bool runNoteOnTests(class Machine* machine, bool useFFT);
bool runNoteReleaseTests(class Machine* machine, bool useFFT);
bool runOctaveTests(class Machine* machine, bool useFFT);
bool runCCControlTests(class Machine* machine, bool useFFT);
bool runPolyphonyTests(class Machine* machine, bool useFFT);
bool runFilterEnvelopeTests(class Machine* machine, bool useFFT);
bool runVoiceLevelTests(class Machine* machine, bool useFFT);
bool runEnvelopeTests(class Machine* machine, bool useFFT);
bool runPresetTests(class Machine* machine, bool useFFT);
bool runFilterFullTests(class Machine* machine, bool useFFT);
bool runFilterFull2Tests(class Machine* machine, bool useFFT);
bool runCCMappingTests(class Machine* machine, bool useFFT);
bool runMidiMappingLoadTests(class Machine* machine, bool useFFT);
bool runPresetRoundtripTests(class Machine* machine, bool useFFT);
bool runLFOModulationTests(class Machine* machine, bool useFFT);
bool runOscCrossModTests(class Machine* machine, bool useFFT);
bool runEffectsTests(class Machine* machine, bool useFFT);
bool runFilterSwitchTests(class Machine* machine, bool useFFT);
bool runNoteStealTests(class Machine* machine, bool useFFT);
bool runPortamentoTests(class Machine* machine, bool useFFT);
bool runEdgeCaseTests(class Machine* machine, bool useFFT);
bool runBipolarParamTests(class Machine* machine, bool useFFT);
bool runLongRunningTests(class Machine* machine, bool useFFT);
bool runDefaultInitPitchTest(class Machine* machine, bool useFFT);

#endif
