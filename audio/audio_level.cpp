#include "audio_level.h"

std::atomic<float> AudioLevel::peak{0.0f};
std::atomic<float> AudioLevel::rms{0.0f};
