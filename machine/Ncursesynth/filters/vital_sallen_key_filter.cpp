#include "vital_sallen_key_filter.h"
#include <algorithm>
#include <cmath>

VitalSallenKeyFilter::VitalSallenKeyFilter(float rate)
    : sample_rate_(rate), fc_(1000.0f), res_percent_(0.0f),
      style_(Style::LP12), drive_(1.0f), post_multiply_(1.0f),
      stage1_input_(0.0f), low_amount_(1.0f), band_amount_(0.0f),
      high_amount_(0.0f), use_24db_(false) {
    reset();
}

VitalSallenKeyFilter::~VitalSallenKeyFilter() {}

void VitalSallenKeyFilter::setSampleRate(float rate) {
    sample_rate_ = rate;
}

void VitalSallenKeyFilter::setCutoff(float freq) {
    fc_ = std::max(1.0f, std::min(freq, sample_rate_ * 0.45f));
}

void VitalSallenKeyFilter::setResonance(float res) {
    res_percent_ = std::max(0.0f, std::min(1.0f, res));
}

void VitalSallenKeyFilter::setStyle(Style style) {
    style_ = style;
    switch (style) {
        case Style::LP12: use_24db_ = false; low_amount_ = 1.0f; band_amount_ = 0.0f; high_amount_ = 0.0f; break;
        case Style::HP12: use_24db_ = false; low_amount_ = 0.0f; band_amount_ = 0.0f; high_amount_ = 1.0f; break;
        case Style::BP12: use_24db_ = false; low_amount_ = 0.0f; band_amount_ = 1.0f; high_amount_ = 0.0f; break;
        case Style::LP24: use_24db_ = true;  low_amount_ = 1.0f; band_amount_ = 0.0f; high_amount_ = 0.0f; break;
        case Style::HP24: use_24db_ = true;  low_amount_ = 0.0f; band_amount_ = 0.0f; high_amount_ = 1.0f; break;
        case Style::BP24: use_24db_ = true;  low_amount_ = 0.0f; band_amount_ = 1.0f; high_amount_ = 0.0f; break;
    }
}

void VitalSallenKeyFilter::setupFilter() {
    float res_sqrt = std::sqrt(res_percent_);
    float resonance = res_sqrt * 2.15f;
    float res_scale = res_percent_ * res_percent_ * 2.0f + 1.0f;
    drive_ = 1.0f / res_scale;
    post_multiply_ = 1.0f / std::sqrt(res_scale);
}

void VitalSallenKeyFilter::reset() {
    pre_stage1_.reset();
    pre_stage2_.reset();
    stage1_.reset();
    stage2_.reset();
    stage1_input_ = 0.0f;
}

float VitalSallenKeyFilter::process(float input) {
    setupFilter();
    float coefficient = onePoleLookup(fc_, sample_rate_);

    if (use_24db_)
        return tick24(input, coefficient);
    else
        return tick12(input, coefficient);
}

float VitalSallenKeyFilter::tick12(float audio_in, float coefficient) {
    float csq = coefficient * coefficient;
    float c2 = coefficient * 2.0f;
    float coeff_diff = csq - coefficient;

    float res_tuned = (res_percent_ * 2.15f) / std::max(1.0f, coefficient * 0.09f + 0.97f);
    float stage1_fb = c2 - csq - 1.0f;
    float normalizer = 1.0f / (res_tuned * coeff_diff + 1.0f);

    float mult_s2 = -coefficient + 1.0f;
    float feedback = stage1_fb * stage1_.getNextState() + mult_s2 * stage2_.getNextState();
    stage1_input_ = vitalTanh((drive_ * audio_in - res_tuned * feedback) * normalizer);

    float s1_out = stage1_.tickBasic(stage1_input_, coefficient);
    stage2_.tickBasic(s1_out, coefficient);

    float s2_input = stage1_.getCurrentState();
    float lp = stage2_.getCurrentState();
    float bp = s2_input - lp;
    float hp = stage1_input_ - s2_input - bp;

    return (low_amount_ * lp + band_amount_ * bp + high_amount_ * hp) * post_multiply_;
}

float VitalSallenKeyFilter::tick24(float audio_in, float coefficient) {
    float csq = coefficient * coefficient;
    float c2 = coefficient * 2.0f;
    float coeff_diff = csq - coefficient;

    float res_tuned = (res_percent_ * 2.15f) / std::max(1.0f, coefficient * 0.09f + 0.97f);
    float stage1_fb = c2 - csq - 1.0f;
    float pre_normalizer = 1.0f / (coeff_diff + 1.0f);
    float normalizer = 1.0f / (res_tuned * coeff_diff + 1.0f);

    float mult_s2 = -coefficient + 1.0f;
    float pre_fb = stage1_fb * pre_stage1_.getNextState() + mult_s2 * pre_stage2_.getNextState();
    float pre_input = (audio_in - pre_fb) * pre_normalizer;

    float pre_s1 = pre_stage1_.tickBasic(pre_input, coefficient);
    float pre_s2 = pre_stage2_.tickBasic(pre_s1, coefficient);

    float bp_out = pre_s1 - pre_s2;
    float hp_out = pre_input - pre_s1 - bp_out;
    float pre_out = low_amount_ * pre_s2 + band_amount_ * bp_out + high_amount_ * hp_out;

    float fb = stage1_fb * stage1_.getNextState() + mult_s2 * stage2_.getNextState();
    stage1_input_ = vitalTanh((drive_ * pre_out - res_tuned * fb) * normalizer);

    float s1_out = stage1_.tickBasic(stage1_input_, coefficient);
    stage2_.tickBasic(s1_out, coefficient);

    float s2_input = stage1_.getCurrentState();
    float lp = stage2_.getCurrentState();
    float bp = s2_input - lp;
    float hp = stage1_input_ - s2_input - bp;

    return (low_amount_ * lp + band_amount_ * bp + high_amount_ * hp) * post_multiply_;
}
