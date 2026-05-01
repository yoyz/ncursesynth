#ifndef SAW_OSCILLATOR_H
#define SAW_OSCILLATOR_H

enum class Waveform {
    SAWTOOTH,
    SQUARE,
    TRIANGLE
};

class SawtoothOscillator {
private:
    float sampleRate;
    float phase;
    float freq;
    Waveform waveform;
    
    float generateSawtooth();
    float generateSquare();
    float generateTriangle();
    
public:
    SawtoothOscillator(float rate = 48000.0f);
    ~SawtoothOscillator();
    
    void setFrequency(float frequency);
    void setSampleRate(float rate);
    void setWaveform(Waveform wav);
    Waveform getWaveform() const { return waveform; }
    float process();
    void reset();
};

#endif
