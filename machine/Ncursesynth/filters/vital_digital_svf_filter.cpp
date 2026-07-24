#include "vital_digital_svf_filter.h"
#include <algorithm>
#include <cmath>

VitalDigitalSvfFilter::VitalDigitalSvfFilter(float rate)
    : sample_rate_(rate), fc_(1000.0f), res_percent_(0.0f),
      style_(Style::LP12), drive_(1.0f), post_multiply_(1.0f),
      resonance_(1.0f), low_amount_(1.0f), band_amount_(0.0f),
      high_amount_(0.0f), use_24db_(false),
      ic1eq_(0.0f), ic2eq_(0.0f), ic1eq_pre_(0.0f), ic2eq_pre_(0.0f) {}

VitalDigitalSvfFilter::~VitalDigitalSvfFilter() {}

void VitalDigitalSvfFilter::setSampleRate(float rate) {
    sample_rate_ = rate;
}

void VitalDigitalSvfFilter::setCutoff(float freq) {
    fc_ = std::max(1.0f, std::min(freq, sample_rate_ * 0.45f));
}

void VitalDigitalSvfFilter::setResonance(float res) {
    res_percent_ = std::max(0.0f, std::min(1.0f, res));
}

void VitalDigitalSvfFilter::setStyle(Style style) {
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

void VitalDigitalSvfFilter::setupFilter() {
    float rp = res_percent_;
    float r_cubed = rp * rp * rp;
    float res_val = 0.5f + r_cubed * (16.0f - 0.5f);
    drive_ = 1.0f / (r_cubed * 2.0f + 1.0f);
    post_multiply_ = 1.0f / std::sqrt(1.0f / drive_);
    resonance_ = 1.0f / res_val;
    if (resonance_ > 2.0f) resonance_ = 2.0f;
}

void VitalDigitalSvfFilter::reset() {
    ic1eq_ = ic2eq_ = 0.0f;
    ic1eq_pre_ = ic2eq_pre_ = 0.0f;
}

float VitalDigitalSvfFilter::process(float input) {
    setupFilter();
    float coefficient = svfLookup(fc_, sample_rate_);

    if (use_24db_)
        return tick24(input, coefficient);
    else
        return tick12(input, coefficient);
}

float VitalDigitalSvfFilter::tick12(float audio_in, float coefficient) {
    float csq = coefficient * coefficient;
    float c0 = 1.0f / (csq + coefficient * resonance_ + 1.0f);
    float c1 = c0 * coefficient;
    float c2 = c0 * csq;

    float v3 = drive_ * audio_in - ic2eq_;
    float v1 = c0 * ic1eq_ + c1 * v3;
    float v2 = ic2eq_ + c1 * ic1eq_ + c2 * v3;
    ic1eq_ = v1 * 2.0f - ic1eq_;
    ic2eq_ = v2 * 2.0f - ic2eq_;

    float blend_v0 = high_amount_;
    float blend_v1 = band_amount_ - resonance_ * high_amount_;
    float blend_v2 = low_amount_ - high_amount_;

    float out = blend_v0 * drive_ * audio_in + blend_v1 * v1 + blend_v2 * v2;
    return vitalHardTanh(out) * post_multiply_;
}

float VitalDigitalSvfFilter::tick24(float audio_in, float coefficient) {
    float csq = coefficient * coefficient;

    float pc0 = 1.0f / (csq + coefficient + 1.0f);
    float pc1 = pc0 * coefficient;
    float pc2 = pc0 * csq;

    float v3p = drive_ * audio_in - ic2eq_pre_;
    float v1p = pc0 * ic1eq_pre_ + pc1 * v3p;
    float v2p = ic2eq_pre_ + pc1 * ic1eq_pre_ + pc2 * v3p;
    ic1eq_pre_ = v1p * 2.0f - ic1eq_pre_;
    ic2eq_pre_ = v2p * 2.0f - ic2eq_pre_;

    float blend_v0 = high_amount_;
    float blend_v1 = band_amount_ - 1.0f * high_amount_;
    float blend_v2 = low_amount_ - high_amount_;
    float pre_out = blend_v0 * drive_ * audio_in + blend_v1 * v1p + blend_v2 * v2p;
    float distort = vitalHardTanh(pre_out);

    float c0 = 1.0f / (csq + coefficient * resonance_ + 1.0f);
    float c1 = c0 * coefficient;
    float c2 = c0 * csq;

    float v3 = distort - ic2eq_;
    float v1 = c0 * ic1eq_ + c1 * v3;
    float v2 = ic2eq_ + c1 * ic1eq_ + c2 * v3;
    ic1eq_ = v1 * 2.0f - ic1eq_;
    ic2eq_ = v2 * 2.0f - ic2eq_;

    float out = blend_v0 * distort + blend_v1 * v1 + blend_v2 * v2;
    return vitalHardTanh(out) * post_multiply_;
}
