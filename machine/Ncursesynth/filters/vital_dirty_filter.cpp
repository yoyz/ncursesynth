#include "vital_dirty_filter.h"
#include <algorithm>
#include <cmath>

VitalDirtyFilter::VitalDirtyFilter(float rate)
    : sample_rate_(rate), fc_(1000.0f), res_percent_(0.0f),
      style_(Style::LP12), drive_(1.0f), drive_boost_(0.0f), drive_blend_(0.0f),
      resonance_(0.0f), coefficient_(0.1f),
      low_amount_(1.0f), band_amount_(0.0f), high_amount_(0.0f),
      use_24db_(false) {
    reset();
}

VitalDirtyFilter::~VitalDirtyFilter() {}

void VitalDirtyFilter::setSampleRate(float rate) {
    sample_rate_ = rate;
}

void VitalDirtyFilter::setCutoff(float freq) {
    fc_ = std::max(1.0f, std::min(freq, sample_rate_ * 0.45f));
}

void VitalDirtyFilter::setResonance(float res) {
    res_percent_ = std::max(0.0f, std::min(1.0f, res));
}

void VitalDirtyFilter::setStyle(Style style) {
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

void VitalDirtyFilter::setupFilter() {
    resonance_ = std::sqrt(res_percent_);
    drive_ = 1.0f;
    drive_boost_ = 0.0f;
    drive_blend_ = 1.0f;
}

void VitalDirtyFilter::reset() {
    pre_stage1_.reset();
    pre_stage2_.reset();
    stage1_.reset();
    stage2_.reset();
    stage3_.reset();
    stage4_.reset();
}

float VitalDirtyFilter::process(float input) {
    setupFilter();
    coefficient_ = onePoleLookup(fc_, sample_rate_);

    if (use_24db_)
        return tick24(input, coefficient_);
    else
        return tick12(input, coefficient_);
}

float VitalDirtyFilter::tick12(float audio_in, float coefficient) {
    float csq = coefficient * coefficient;
    float c2 = coefficient * 2.0f;

    float res_in = resonance_ / std::max(1.0f, coefficient * 0.25f + 0.97f);
    float res_val = 0.1f + res_in * (2.15f - 0.1f) + drive_boost_;
    float res_sq = res_val * res_val;

    float normalizer = kSaturationBoost / (res_sq + 1.0f);
    float compute = -res_val * (coefficient - csq) + 1.0f;
    float feed_mult = 1.0f / (compute * (coefficient + 1.0f));

    float scaled_drive = std::max(0.1f, drive_) / (res_sq * 0.5f + 1.0f);
    float drv = drive_ * (1.0f - drive_blend_) + scaled_drive * drive_blend_;

    float s1_in = normalizer * audio_in;
    float s1_out = stage1_.tickBasic(s1_in, coefficient);
    float s2_out = stage2_.tickBasic(s1_out, coefficient);

    float bp = s1_out - s2_out;
    float hp = s1_in - s1_out - bp;
    float pass = low_amount_ * s2_out + band_amount_ * bp + high_amount_ * hp;

    float fb = stage4_.getNextSatState() +
               (pass * coefficient + pass - stage3_.getNextSatState());
    float loop = vitalTanh(drv * pass + res_val * feed_mult * fb);

    stage3_.tick(loop, coefficient);
    float s4_in = loop - stage3_.getCurrentState();
    stage4_.tick(s4_in, coefficient);

    return loop * (1.0f / kSaturationBoost);
}

float VitalDirtyFilter::tick24(float audio_in, float coefficient) {
    float csq = coefficient * coefficient;
    float c2 = coefficient * 2.0f;
    float coeff_diff = csq - coefficient;

    float res_in = resonance_ / std::max(1.0f, coefficient * 0.25f + 0.97f);
    float res_val = 0.1f + res_in * (2.15f - 0.1f) + drive_boost_;
    float res_sq = res_val * res_val;

    float normalizer = kSaturationBoost / (res_sq + 1.0f);
    float compute = -res_val * coeff_diff + 1.0f;
    float feed_mult = 1.0f / (compute * (coefficient + 1.0f));
    float pre_feedback = c2 - csq - 1.0f;
    float pre_normalizer = 1.0f / (coeff_diff * kFlatResonance + 1.0f);

    float scaled_drive = std::max(0.1f, drive_) / (res_sq * 0.5f + 1.0f);
    float drv = drive_ * (1.0f - drive_blend_) + scaled_drive * drive_blend_;

    float mult_s2 = -coefficient + 1.0f;
    float pre_fb = pre_feedback * pre_stage1_.getNextSatState() +
                   mult_s2 * pre_stage2_.getNextSatState();
    pre_fb *= kFlatResonance;
    float pre_input = (audio_in - pre_fb) * pre_normalizer;

    float pre_s1 = pre_stage1_.tickBasic(pre_input, coefficient);
    float pre_s2 = pre_stage2_.tickBasic(pre_s1, coefficient);

    float pre_bp = pre_s1 - pre_s2;
    float pre_hp = pre_input - pre_s1 - pre_bp;
    float pre_out = band_amount_ * pre_bp + high_amount_ * pre_hp + low_amount_ * pre_s2;

    return tick12(pre_out, coefficient);
}
