#include "distortion.h"
#include <algorithm>

DistortionEffect::DistortionEffect() 
    : drive(0.0f), mix(0.0f), enabled(false) {}

DistortionEffect::~DistortionEffect() {}

void DistortionEffect::setSampleRate(float rate) {
    // No rate-dependent processing needed
}

void DistortionEffect::setDrive(float d) {
    drive = std::max(0.0f, std::min(1.0f, d));
}

void DistortionEffect::setMix(float m) {
    mix = std::max(0.0f, std::min(1.0f, m));
}

float DistortionEffect::process(float input) {
    if (!enabled || drive == 0.0f) return input;
    
    // Apply drive
    float gain = 1.0f + drive * 4.0f;
    float distorted = input * gain;
    
    // Soft clipping (tanh)
    distorted = tanhf(distorted);
    
    // Hard clipping for more aggressive sound
    if (drive > 0.7f) {
        if (distorted > 0.8f) distorted = 0.8f;
        if (distorted < -0.8f) distorted = -0.8f;
    }
    
    // Mix
    return input * (1.0f - mix) + distorted * mix;
}

void DistortionEffect::reset() {
    // Nothing to reset
}
