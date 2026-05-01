#include "svf_filter.h"
#include <cmath>
#include <algorithm>

StateVariableFilter::StateVariableFilter(float rate)
    : sampleRate(rate), fc(1000.0f), res(0.5f),
      ic1eq(0.0f), ic2eq(0.0f),
      in_0_(0.0f), in_1_(0.0f), in_2_(0.0f),
      out_0_(0.0f), out_1_(0.0f),
      past_in_1_(0.0f), past_in_2_(0.0f),
      past_out_1_(0.0f), past_out_2_(0.0f),
      current_cutoff_(0.0f), current_resonance_(0.0f),
      currentType(Type::LP12) {
    computeCoefficients(Type::LP12, fc, res);
}

StateVariableFilter::~StateVariableFilter() {}

void StateVariableFilter::setCutoff(float freq) {
    fc = freq;
    if (fc < 20.0f) fc = 20.0f;
    if (fc > sampleRate * 0.45f) fc = sampleRate * 0.45f;
}

void StateVariableFilter::setResonance(float resonance) {
    res = resonance;
    if (res < 0.0f) res = 0.0f;
    if (res > 1.0f) res = 1.0f;
}

void StateVariableFilter::setSampleRate(float rate) {
    sampleRate = rate;
}

void StateVariableFilter::setType(Type type) {
    currentType = type;
}

void StateVariableFilter::reset() {
    ic1eq = ic2eq = 0.0f;
    past_in_1_ = past_in_2_ = past_out_1_ = past_out_2_ = 0.0f;
}

void StateVariableFilter::computeCoefficients(Type type,
                                              float cutoff,
                                              float resonance) {
    float sf = 1.0f / tanf(PI * cutoff / sampleRate);
    float sf_squared = sf * sf;
    float norm = 1.0f + 1.0f / resonance * sf + sf_squared;

    switch(type) {
        case Type::LP12: {
            in_2_ = in_0_ = 1.0f / norm;
            in_1_ = 2.0f / norm;
            out_0_ = 2.0f * (1.0f - sf_squared) / norm;
            out_1_ = (1.0f - 1.0f / resonance * sf + sf_squared) / norm;
            break;
        }
        case Type::HP12: {
            in_2_ = in_0_ = sf_squared / norm;
            in_1_ = -2.0f * sf_squared / norm;
            out_0_ = 2.0f * (1.0f - sf_squared) / norm;
            out_1_ = (1.0f - 1.0f / resonance * sf + sf_squared) / norm;
            break;
        }
        case Type::BP12: {
            in_2_ = in_0_ = sf / norm;
            in_1_ = 0.0f;
            out_0_ = 2.0f * (1.0f - sf_squared) / norm;
            out_1_ = (1.0f - 1.0f / resonance * sf + sf_squared) / norm;
            break;
        }
        case Type::AP12: {
            in_0_ = 1.0f / norm;
            in_1_ = -2.0f * (1.0f - sf_squared) / norm;
            in_2_ = (1.0f - 1.0f / resonance * sf + sf_squared) / norm;
            out_0_ = -in_1_;
            out_1_ = in_2_;
            break;
        }
    }

    current_cutoff_ = cutoff;
    current_resonance_ = resonance;
}

float StateVariableFilter::process(float input) {
    return processLP(input);
}

float StateVariableFilter::processLP(float input) {
    if (fc != current_cutoff_ || res != current_resonance_)
        computeCoefficients(Type::LP12, fc, res);

    float out = input * in_0_ + past_in_1_ * in_1_ + past_in_2_ * in_2_ -
                past_out_1_ * out_0_ - past_out_2_ * out_1_;
    past_in_2_ = past_in_1_;
    past_in_1_ = input;
    past_out_2_ = past_out_1_;
    past_out_1_ = out;
    return out;
}

float StateVariableFilter::processHP(float input) {
    if (fc != current_cutoff_ || res != current_resonance_)
        computeCoefficients(Type::HP12, fc, res);

    float out = input * in_0_ + past_in_1_ * in_1_ + past_in_2_ * in_2_ -
                past_out_1_ * out_0_ - past_out_2_ * out_1_;
    past_in_2_ = past_in_1_;
    past_in_1_ = input;
    past_out_2_ = past_out_1_;
    past_out_1_ = out;
    return out;
}

float StateVariableFilter::processBP(float input) {
    if (fc != current_cutoff_ || res != current_resonance_)
        computeCoefficients(Type::BP12, fc, res);

    float out = input * in_0_ + past_in_1_ * in_1_ + past_in_2_ * in_2_ -
                past_out_1_ * out_0_ - past_out_2_ * out_1_;
    past_in_2_ = past_in_1_;
    past_in_1_ = input;
    past_out_2_ = past_out_1_;
    past_out_1_ = out;
    return out;
}

float StateVariableFilter::processAP(float input) {
    if (fc != current_cutoff_ || res != current_resonance_)
        computeCoefficients(Type::AP12, fc, res);

    float out = input * in_0_ + past_in_1_ * in_1_ + past_in_2_ * in_2_ -
                past_out_1_ * out_0_ - past_out_2_ * out_1_;
    past_in_2_ = past_in_1_;
    past_in_1_ = input;
    past_out_2_ = past_out_1_;
    past_out_1_ = out;
    return out;
}