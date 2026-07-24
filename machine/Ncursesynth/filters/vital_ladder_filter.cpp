#include "vital_ladder_filter.h"
#include <algorithm>
#include <cmath>

VitalLadderFilter::VitalLadderFilter(float rate)
    : sample_rate_(rate), fc_(1000.0f), res_percent_(0.0f),
      style_(Style::LP24), resonance_(0.0f), drive_(1.0f),
      post_multiply_(1.0f), filter_input_(0.0f) {
    reset();
    computeStageScales();
}

VitalLadderFilter::~VitalLadderFilter() {}

void VitalLadderFilter::setSampleRate(float rate) {
    sample_rate_ = rate;
}

void VitalLadderFilter::setCutoff(float freq) {
    fc_ = std::max(10.0f, std::min(freq, sample_rate_ * 0.45f));
}

void VitalLadderFilter::setResonance(float res) {
    res_percent_ = std::max(0.0f, std::min(1.0f, res));
}

void VitalLadderFilter::setStyle(Style style) {
    style_ = style;
    computeStageScales();
}

void VitalLadderFilter::computeStageScales() {
    for (int i = 0; i < 5; ++i)
        stage_scales_[i] = 0.0f;

    switch (style_) {
        case Style::LP24: stage_scales_[4] = 1.0f; break;
        case Style::HP24: stage_scales_[0] = 1.0f; stage_scales_[1] = -4.0f; stage_scales_[2] = 6.0f; stage_scales_[3] = -4.0f; stage_scales_[4] = 1.0f; break;
        case Style::BP24: stage_scales_[2] = -1.0f; stage_scales_[3] = 2.0f; stage_scales_[4] = -1.0f; break;
        case Style::LP12: stage_scales_[2] = 1.0f; break;
        case Style::HP12: stage_scales_[0] = 1.0f; stage_scales_[1] = -2.0f; stage_scales_[2] = 1.0f; break;
        case Style::BP12: stage_scales_[1] = 1.0f; stage_scales_[2] = -1.0f; break;
    }
}

void VitalLadderFilter::setupFilter() {
    float rp = sinf(res_percent_ * float(M_PI) * 0.5f);
    resonance_ = kMinResonance + rp * (kMaxResonance - kMinResonance);
    float res_scale = res_percent_ + 1.0f;
    drive_ = 1.0f * res_scale;
    post_multiply_ = 1.0f / std::sqrt((1.0f - 1.0f) * 0.5f + 1.0f);
}

void VitalLadderFilter::reset() {
    for (int i = 0; i < 4; ++i)
        stages_[i].reset();
    filter_input_ = 0.0f;
}

float VitalLadderFilter::process(float input) {
    setupFilter();
    float coefficient = onePoleLookup(fc_, sample_rate_);

    float g1 = coefficient * kResonanceTuning;
    float g2 = g1 * g1;
    float g3 = g1 * g2;

    float fs1 = stages_[3].getNextSatState() * g1 + stages_[2].getNextSatState();
    float fs2 = fs1 * g2 + stages_[1].getNextSatState();
    float fs = fs2 * g3 + stages_[0].getNextSatState();

    filter_input_ = vitalTanh(input * drive_ - resonance_ * fs);

    float s0 = stages_[0].tick(filter_input_, coefficient);
    float s1 = stages_[1].tick(s0, coefficient);
    float s2 = stages_[2].tick(s1, coefficient);
    stages_[3].tick(s2, coefficient);

    float total = stage_scales_[0] * filter_input_;
    for (int i = 0; i < 4; ++i)
        total += stage_scales_[i + 1] * stages_[i].getCurrentState();

    return total * post_multiply_;
}
