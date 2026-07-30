#ifndef DIGITS_PHASEDIST_H
#define DIGITS_PHASEDIST_H

#include <cmath>

class DigitsPhaseDist {
public:
    DigitsPhaseDist();

    void setPitch(float freqHz, float sampleRate);
    void resetPhase();

    float process(float fmIn, float shaperMod, float skew,
                  float brightness, float pwm, int basisWave);

    float getDelta() const { return m_delta; }

private:
    float distortPhase(float phase, float contour, float sk);
    float m_pos;
    float m_delta;
};

class DigitsResoGen {
public:
    enum Style {
        kSawQuarter,
        kSawHalf,
        kSquareQuarter,
        kSquareHalf
    };

    DigitsResoGen();

    void setPitch(float freqHz, float sampleRate);
    void resetPhase();

    float process(float shaperMod, float pwm, float brightness,
                  float ampContour, float volume, Style style);


private:
    float m_pos1, m_pos2;
    float m_delta;
    float m_flip;
};

#endif
