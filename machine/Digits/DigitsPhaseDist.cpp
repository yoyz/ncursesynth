#include "DigitsPhaseDist.h"
#include <cmath>

DigitsPhaseDist::DigitsPhaseDist()
    : m_pos(0), m_delta(0)
{
}

void DigitsPhaseDist::setPitch(float freqHz, float sampleRate)
{
    m_delta = freqHz / sampleRate;
}

void DigitsPhaseDist::resetPhase()
{
    m_pos = 0;
}

float DigitsPhaseDist::distortPhase(float phase, float contour, float sk)
{
    if (contour <= 0.0001f) contour = 0.0001f;
    if (contour >= 0.9999f) contour = 0.9999f;

    if (phase < contour)
        return (sk / contour) * phase;
    else
        return sk + ((1.0f - sk) / (1.0f - contour)) * (phase - contour);
}

float DigitsPhaseDist::process(float fmIn, float shaperMod, float skew,
                                float brightness, float pwm, int basisWave)
{
    float contour = skew - (brightness * skew);
    if (contour < 0.01f) contour = 0.01f;
    if (contour > 0.99f) contour = 0.99f;

    float pwmWidth = 0.1f + pwm * 0.8f;

    float pwmPos;
    if (m_pos < pwmWidth)
        pwmPos = (0.5f / pwmWidth) * m_pos;
    else
        pwmPos = (0.5f / (1.0f - pwmWidth)) * m_pos + (1.0f - 0.5f / (1.0f - pwmWidth));

    float index;
    bool squareMode = (shaperMod < 0.6f);
    if (squareMode) {
        if (pwmPos < 0.5f) {
            index = distortPhase(pwmPos * 2.0f, contour, skew) * 0.5f;
        } else {
            index = distortPhase((pwmPos - 0.5f) * 2.0f, contour, skew) * 0.5f + 0.5f;
        }
    } else {
        index = distortPhase(pwmPos, contour, skew);
    }

    if (shaperMod > 1.0f)
        index *= shaperMod;

    while (index >= 1.0f) index -= 1.0f;
    while (index < 0.0f) index += 1.0f;

    float angle = index * 2.0f * (float)M_PI;

    float sample;
    if (fmIn != 0.0f)
        angle += fmIn;

    if (basisWave == 0)
        sample = cosf(angle);
    else
        sample = sinf(angle);

    m_pos += m_delta;
    while (m_pos >= 1.0f)
        m_pos -= 1.0f;

    return sample;
}

DigitsResoGen::DigitsResoGen()
    : m_pos1(0), m_pos2(0), m_delta(0), m_flip(1.0f)
{
}

void DigitsResoGen::setPitch(float freqHz, float sampleRate)
{
    m_delta = freqHz / sampleRate;
}

void DigitsResoGen::resetPhase()
{
    m_pos1 = 0;
    m_pos2 = 0;
    m_flip = 1.0f;
}

float DigitsResoGen::process(float shaperMod, float pwm, float brightness,
                              float ampContour, float volume, Style style)
{
    float bright = 0.5f + (brightness - 0.5f) * 18.0f;
    if (bright < 0.0f) bright = 0.0f;
    if (bright > 10.0f) bright = 10.0f;

    float pwmWidth = 0.1f + pwm * 0.8f;
    float m3 = 0.5f / pwmWidth;
    float m4 = 0.5f / (1.0f - pwmWidth);
    float b4 = 1.0f - m4;

    float pwmPos;
    if (m_pos2 < pwmWidth)
        pwmPos = m3 * m_pos2;
    else
        pwmPos = m4 * m_pos2 + b4;

    float sam = 0;
    float angle1 = m_pos1 * bright * 2.0f * (float)M_PI;
    float angle2 = pwmPos * 2.0f * (float)M_PI;

    if (style == kSawQuarter || style == kSquareQuarter) {
        sam = sinf(angle1) * 0.5f * cosf(angle2 * 0.25f) * volume * ampContour * m_flip;
        sam = tanhf(sam);
    } else if (style == kSawHalf || style == kSquareHalf) {
        sam = ((sinf(angle1) * 0.5f) + 1.0f) * sinf(angle2 * 0.5f) * volume * ampContour * m_flip;
    }

    m_pos1 += m_delta;
    while (m_pos1 >= 1.0f) m_pos1 -= 1.0f;

    m_pos2 += m_delta;
    while (m_pos2 >= 1.0f) {
        m_pos2 -= 1.0f;
        if (style == kSquareQuarter || style == kSquareHalf)
            m_flip *= -1.0f;
    }

    return sam;
}
