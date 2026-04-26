#ifndef SYNTH_ARCHITECTURE_H
#define SYNTH_ARCHITECTURE_H

#include "voice.h"
#include "../effects/effect_chain.h"
#include "../preset/preset_manager.h"
#include <vector>
#include <memory>

// Forward declaration
class MidiInput;

class SynthArchitecture {
private:
    std::vector<std::unique_ptr<Voice>> voices;
    EffectChain effectChain;
    int maxPolyphony;
    int nextNoteId;
    float sampleRate;
    
    // Global parameters
    FilterType currentFilterType;
    Waveform currentWaveform;
    float cutoff;
    float hpfCutoff;
    float resonance;
    float filterEnvelopeAmount;
    EnvelopeCurve ampEnvelopeCurve;
    EnvelopeCurve filterEnvelopeCurve;
    float ampAttack;
    float ampDecay;
    float ampSustain;
    float ampRelease;
    float filtAttack;
    float filtDecay;
    float filtSustain;
    float filtRelease;
    float volume;
    
    // Oscillator parameters
    float oscMix;
    float osc2Detune;
    
    // MIDI reference
    MidiInput* midiInput;
    PresetManager presetManager;
    
    int findFreeVoice();
    int findVoiceByNoteId(int noteId);
    void updateAllVoices();
    
public:
    SynthArchitecture(int polyphony = 4, float rate = 48000.0f);
    ~SynthArchitecture();
    
    void setSampleRate(float rate);
    void setPolyphony(int voices);
    void noteOn(float frequency);
    void noteOff(float frequency);
    void reset();
    float process();
    void allNotesOff();
    bool isPlaying() const;
    
    // Voice getter for UI
    const std::vector<std::unique_ptr<Voice>>& getVoices() const { return voices; }
    int getPolyphony() const { return maxPolyphony; }
    
    // MIDI access
    void setMidiInput(MidiInput* input) { midiInput = input; }
    MidiInput* getMidiInput() { return midiInput; }
    
    // Global parameter setters
    void setFilterType(FilterType type);
    void setCutoff(float freq);
    void setHPFCutoff(float freq);
    void setResonance(float res);
    void setFilterEnvelopeAmount(float amount);
    void setWaveform(Waveform wav);
    void setAmpEnvelopeCurve(EnvelopeCurve curve);
    void setFilterEnvelopeCurve(EnvelopeCurve curve);
    
    void setAmpAttack(float value);
    void setAmpDecay(float value);
    void setAmpSustain(float value);
    void setAmpRelease(float value);
    
    void setFilterAttack(float value);
    void setFilterDecay(float value);
    void setFilterSustain(float value);
    void setFilterRelease(float value);
    
    void setVolume(float vol);
    
    // Effect controls
    void setEffectEnabled(int effectIndex, bool enabled);
    void setDelayParams(float timeMs, float feedback, float mix);
    void setReverbParams(float decay, float mix);
    void setChorusParams(float depth, float rate, float mix);
    void setDistortionParams(float drive, float mix);
    
    // Getters
    FilterType getCurrentFilterType() const { return currentFilterType; }
    float getCutoff() const { return cutoff; }
    float getHPFCutoff() const { return hpfCutoff; }
    float getResonance() const { return resonance; }
    float getFilterEnvelopeAmount() const { return filterEnvelopeAmount; }
    Waveform getWaveform() const { return currentWaveform; }
    EnvelopeCurve getAmpEnvelopeCurve() const { return ampEnvelopeCurve; }
    EnvelopeCurve getFilterEnvelopeCurve() const { return filterEnvelopeCurve; }
    
    float getAmpAttack() const { return ampAttack; }
    float getAmpDecay() const { return ampDecay; }
    float getAmpSustain() const { return ampSustain; }
    float getAmpRelease() const { return ampRelease; }
    
    float getFilterAttack() const { return filtAttack; }
    float getFilterDecay() const { return filtDecay; }
    float getFilterSustain() const { return filtSustain; }
    float getFilterRelease() const { return filtRelease; }
    
    float getVolume() const { return volume; }
    
    // Oscillator getters
    float getOscMix() const { return oscMix; }
    float getOsc2Detune() const { return osc2Detune; }
    void setOscMix(float mix);
    void setOsc2Detune(float detune);
    
    // Effect getters
    DelayEffect* getDelay() { return effectChain.getDelay(); }
    ReverbEffect* getReverb() { return effectChain.getReverb(); }
    ChorusEffect* getChorus() { return effectChain.getChorus(); }
    DistortionEffect* getDistortion() { return effectChain.getDistortion(); }
    
    // Preset methods
    PresetManager* getPresetManager() { return &presetManager; }
    bool loadPreset(int index) { return presetManager.loadPreset(index, this); }
    bool savePreset(int index) { return presetManager.savePreset(index, this); }
};

#endif
