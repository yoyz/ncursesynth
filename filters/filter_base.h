#ifndef FILTER_BASE_H
#define FILTER_BASE_H

class FilterBase {
public:
    virtual ~FilterBase() {}
    virtual void setCutoff(float freq) = 0;
    virtual void setResonance(float res) = 0;
    virtual void setSampleRate(float rate) = 0;
    virtual float process(float input) = 0;
    virtual void reset() = 0;
};

#endif
