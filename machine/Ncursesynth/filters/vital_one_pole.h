#ifndef VITAL_ONE_POLE_H
#define VITAL_ONE_POLE_H

#include "vital_core.h"

template <float (*saturate)(float) = vitalPassThrough>
class VitalOnePole {
public:
    VitalOnePole() { reset(); }

    void reset() {
        current_state_ = 0.0f;
        filter_state_ = 0.0f;
        sat_filter_state_ = 0.0f;
    }

    float tickBasic(float audio_in, float coefficient) {
        float delta = coefficient * (audio_in - filter_state_);
        filter_state_ += delta;
        current_state_ = filter_state_;
        filter_state_ += delta;
        return current_state_;
    }

    float tick(float audio_in, float coefficient) {
        float delta = coefficient * (audio_in - sat_filter_state_);
        filter_state_ += delta;
        current_state_ = saturate(filter_state_);
        filter_state_ += delta;
        sat_filter_state_ = saturate(filter_state_);
        return current_state_;
    }

    float getCurrentState() const { return current_state_; }
    float getNextSatState() const { return sat_filter_state_; }
    float getNextState() const { return filter_state_; }

    static float computeCoefficient(float cutoff_frequency, float sample_rate) {
        return onePoleLookup(cutoff_frequency, sample_rate);
    }

private:
    float current_state_;
    float filter_state_;
    float sat_filter_state_;
};

#endif
