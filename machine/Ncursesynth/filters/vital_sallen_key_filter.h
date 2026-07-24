#ifndef VITAL_SALLEN_KEY_FILTER_H
#define VITAL_SALLEN_KEY_FILTER_H

#include "filter_base.h"
#include "vital_one_pole.h"

class VitalSallenKeyFilter : public FilterBase {
public:
    enum class Style { LP12, HP12, BP12, LP24, HP24, BP24 };

    VitalSallenKeyFilter(float rate = 48000.0f);
    ~VitalSallenKeyFilter() override;

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
    float drive_, post_multiply_;

    VitalOnePole<> pre_stage1_, pre_stage2_;
    VitalOnePole<> stage1_, stage2_;
    float stage1_input_;

    float low_amount_, band_amount_, high_amount_;
    bool use_24db_;
};

#endif
