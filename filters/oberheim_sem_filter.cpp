#include "oberheim_sem_filter.h"
#include <cmath>
#include <algorithm>

OberheimSEMFilter::OberheimSEMFilter(float rate) 
    : sampleRate(rate), fc(1000.0f), res(0.5f), 
      lp(0), bp(0), hp(0), drive(1.5f) {
    updateCoefficients();
}

OberheimSEMFilter::~OberheimSEMFilter() {}

void OberheimSEMFilter::updateCoefficients() {
    float cutoff = std::min(fc, sampleRate * 0.45f);
    f = 2.0f * sinf(M_PI * cutoff / sampleRate);
    q = 1.0f / (2.0f * (res + 0.01f));
}

void OberheimSEMFilter::setCutoff(float cutoff) {
    fc = cutoff;
    updateCoefficients();
}

void OberheimSEMFilter::setResonance(float resonance) {
    res = resonance;
    updateCoefficients();
}

void OberheimSEMFilter::setSampleRate(float rate) {
    sampleRate = rate;
    updateCoefficients();
}

void OberheimSEMFilter::setDrive(float driveAmount) {
    drive = driveAmount;
}

float OberheimSEMFilter::process(float input) {
    float saturatedInput = tanhf(drive * input);
    hp = saturatedInput - (q * bp) - lp;
    float bp_temp = bp + (f * hp);
    float lp_temp = lp + (f * bp_temp);
    lp = lp_temp;
    bp = bp_temp;
    return lp;
}

void OberheimSEMFilter::reset() {
    lp = bp = hp = 0.0f;
}
