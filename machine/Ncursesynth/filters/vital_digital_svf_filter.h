#ifndef VITAL_DIGITAL_SVF_FILTER_H
#define VITAL_DIGITAL_SVF_FILTER_H

#include "filter_base.h"
#include "vital_core.h"

class VitalDigitalSvfFilter : public FilterBase {
public:
    enum class Style { LP12, HP12, BP12, LP24, HP24, BP24 };

    VitalDigitalSvfFilter(float rate = 48000.0f);
    ~VitalDigitalSvfFilter() override;

    void setCutoff(float freq) override;
    void setResonance(float res) override;
    void setSampleRate(float rate) override;
    void setStyle(Style style);
    float process(float input) override;
    void reset() override;

private:
    void setupFilter();
    float tick12(float audio_in, float coefficient);
    float tick24(float audio_in, float coefficient);

    float sample_rate_;
    float fc_, res_percent_;
    Style style_;
    float drive_, post_multiply_, resonance_;
    float low_amount_, band_amount_, high_amount_;
    bool use_24db_;

    float ic1eq_, ic2eq_;
    float ic1eq_pre_, ic2eq_pre_;
};

#endif
