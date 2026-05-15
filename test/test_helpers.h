#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include <cstdint>
#include <cmath>
#include <string>
#include <vector>

// Constants
constexpr int NUM_SAMPLES = 1024;
constexpr float RMS_THRESHOLD = 0.01f;
constexpr float CLIP_THRESHOLD = 0.99f;
constexpr float FREQUENCY_TOLERANCE = 1.0f; // semitones

// Function declarations
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

#endif
