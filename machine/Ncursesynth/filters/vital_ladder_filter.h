#ifndef VITAL_LADDER_FILTER_H
#define VITAL_LADDER_FILTER_H

#include "filter_base.h"
#include "vital_one_pole.h"

class VitalLadderFilter : public FilterBase {
public:
    enum class Style { LP12, HP12, BP12, LP24, HP24, BP24 };

    VitalLadderFilter(float rate = 48000.0f);
    ~VitalLadderFilter() override;

    void setCutoff(float freq) override;
    void setResonance(float res) override;
    void setSampleRate(float rate) override;
    void setStyle(Style style);
    float process(float input) override;
    void reset() override;

private:
    void setupFilter();
    void computeStageScales();

    float sample_rate_;
    float fc_, res_percent_;
    Style style_;
    float resonance_, drive_, post_multiply_;

    VitalOnePole<vitalHardTanh> stages_[4];
    float filter_input_;
    float stage_scales_[5];

    static constexpr float kResonanceTuning = 1.66f;
    static constexpr float kMinResonance = 0.001f;
    static constexpr float kMaxResonance = 4.1f;
};

#endif
