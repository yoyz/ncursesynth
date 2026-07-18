#include "svf_filter.h"
#include <cmath>
#include <algorithm>

StateVariableFilter::StateVariableFilter(float rate)
    : sampleRate(rate), fc(1000.0f), resQ(0.5f),
      lp(0), bp(0), hp(0), f(0), q(1.0f),
      currentType(Type::LP12) {
    computeCoefficients();
}

StateVariableFilter::~StateVariableFilter() {}

void StateVariableFilter::setSampleRate(float rate) {
    sampleRate = rate;
    computeCoefficients();
}

void StateVariableFilter::setCutoff(float freq) {
    fc = std::max(20.0f, std::min(freq, sampleRate * 0.45f));
    computeCoefficients();
}

void StateVariableFilter::setResonance(float resonance) {
    resQ = std::max(0.0f, std::min(1.0f, resonance));
    computeCoefficients();
}

void StateVariableFilter::setType(Type type) {
    currentType = type;
}

void StateVariableFilter::computeCoefficients() {
    f = 2.0f * sinf(M_PI * fc / sampleRate);
    q = 1.0f - resQ * 0.95f;
}

void StateVariableFilter::reset() {
    lp = bp = hp = 0;
}

float StateVariableFilter::process(float input) {
    hp = input - q * bp - lp;
    bp = bp + f * hp;
    lp = lp + f * bp;

    // Flush denormals to zero
    if (fabsf(lp) < 1e-18f) lp = 0.0f;
    if (fabsf(bp) < 1e-18f) bp = 0.0f;

    switch (currentType) {
        case Type::LP12: return lp;
        case Type::HP12: return hp;
        case Type::BP12: return bp;
        case Type::AP12: return hp + lp;
    }
    return lp;
}
