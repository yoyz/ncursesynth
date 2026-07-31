#include <iostream>
#include <cmath>
#include "twytch_types.h"
#include "TwytchsynthMachine.h"

#define SAM 64

static constexpr int OUTPUT_AMPLITUDE = 8192;

TwytchsynthMachine::TwytchsynthMachine()
    : engine(nullptr), cutoff(125), resonance(10), amp_volume(90),
      trig_time_mode(0), trig_time_duration(0), trig_time_duration_sample(0)
{
    setName("Twytch");
    DPRINTF("TwytchsynthMachine::TwytchsynthMachine()");
    buffer_f = nullptr;
    buffer_i = nullptr;
    note_on = 0;
    index = 0;
    note = 60;
    old_note = 60;
    detune = 0;
    velocity = 64;
    osc1_type = 0;
    osc2_type = 0;
    osc3_type = 0;
    osc4_type = 0;
    osc1_scale = 0;
    osc2_scale = 0;
    osc1_detune = 0;
    osc2_detune = 0;
    osc2_unison = 0;
    osc2_unisondt = 0;
    osc3_amp = 0;
    osc4_amp = 0;
    pole = 0;
    need_note_on = 0;
    env1_depth = 64;
    lfo1_freq_raw = 64;
    lfo2_freq_raw = 64;
}


TwytchsynthMachine::~TwytchsynthMachine()
{
    DPRINTF("TwytchsynthMachine::~TwytchsynthMachine()");
    if (buffer_f) free(buffer_f);
    if (buffer_i) free(buffer_i);
    delete engine;
}


void TwytchsynthMachine::init()
{
    DPRINTF("TwytchsynthMachine::init()");

    if (buffer_f == nullptr)
        buffer_f = (twytchhelmmopo::mopo_float*)malloc(sizeof(twytchhelmmopo::mopo_float) * SAM);
    if (buffer_i == nullptr)
        buffer_i = (Sint16*)malloc(sizeof(Sint16) * SAM);

    for (int i = 0; i < SAM; i++) {
        buffer_f[i] = 0;
        buffer_i[i] = 0;
    }

    sample_num = 0;
    index = 0;

    if (engine == nullptr) {
        engine = new twytchhelmmopo::HelmEngine();
    }
    engine->setBufferSize(SAM);
    engine->setSampleRate(DEFAULTFREQ);

    // Initialize engine controls
    auto controls = engine->getControls();
    if (controls.count("polyphony")) controls.at("polyphony")->set(8);
    if (controls.count("filter_on")) controls.at("filter_on")->set(1);
    if (controls.count("osc_1_tune")) controls.at("osc_1_tune")->set(0);
    if (controls.count("osc_2_tune")) controls.at("osc_2_tune")->set(0);
    if (controls.count("cutoff")) controls.at("cutoff")->set(80.0);
    if (controls.count("resonance")) controls.at("resonance")->set(0.0);
    if (controls.count("volume")) controls.at("volume")->set(0.7);

    note = 60;
}


int TwytchsynthMachine::checkI(int what, int val)
{
    (void)what;
    if (val < 0) return 0;
    if (val > 127) return 127;
    return val;
}


int TwytchsynthMachine::getI(int what)
{
    if (engine == nullptr) return 0;

    if (what == NOTE_ON) return note_on;
    if (what == NOTE1) return note;
    if (what == OSC1_TYPE) return osc1_type;
    if (what == OSC2_TYPE) return osc2_type;
    if (what == OSC1_SCALE) return osc1_scale;
    if (what == OSC2_SCALE) return osc2_scale;
    if (what == OSC1_DETUNE) return osc1_detune;
    if (what == OSC2_DETUNE) return osc2_detune;

    if (what == ADSR_ENV0_ATTACK) return adsr_env0_attack;
    if (what == ADSR_ENV0_DECAY) return adsr_env0_decay;
    if (what == ADSR_ENV0_SUSTAIN) return adsr_env0_sustain;
    if (what == ADSR_ENV0_RELEASE) return adsr_env0_release;
    if (what == ADSR_ENV1_ATTACK) return adsr_env1_attack;
    if (what == ADSR_ENV1_DECAY) return adsr_env1_decay;
    if (what == ADSR_ENV1_SUSTAIN) return adsr_env1_sustain;
    if (what == ADSR_ENV1_RELEASE) return adsr_env1_release;
    if (what == FILTER1_TYPE) return filter1_type;
    if (what == FILTER1_CUTOFF) return filter1_cutoff;
    if (what == FILTER1_RESONANCE) return filter1_resonance;
    if (what == OSC12_MIX) return osc12_mix;
    if (what == ENV1_DEPTH) return env1_depth;
    if (what == OSC2_DETUNE) return osc2_detune;
    if (what == OSC2_UNISON) return osc2_unison;
    if (what == OSC2_UNISONDT) return osc2_unisondt;
    if (what == OSC3_TYPE) return osc3_type;
    if (what == OSC3_AMP) return osc3_amp;
    if (what == OSC4_TYPE) return osc4_type;
    if (what == OSC4_AMP) return osc4_amp;
    if (what == LFO1_FREQ) return lfo1_freq_raw;
    if (what == LFO2_FREQ) return lfo2_freq_raw;
    if (what == LFO1_DEPTH) return lfo1_env_amount;
    if (what == LFO2_DEPTH) return lfo2_env_amount;
    if (what == AMP) return amp_volume;
    if (what == VELOCITY) return velocity;

    return 0;
}


void TwytchsynthMachine::setF(int what, float val)
{
    float f_val = val / 128.0f;

    if (engine == nullptr) return;

    if (what == LFO1_FREQ) {
        lfo1_freq_raw = (int)val;
        lfo1_freq = f_val * 10;
        if (engine->getControls().count("lfo_1_rate"))
            engine->getControls().at("lfo_1_rate")->set(lfo1_freq);
    }
    if (what == LFO2_FREQ) {
        lfo2_freq_raw = (int)val;
        lfo2_freq = f_val * 10;
        if (engine->getControls().count("lfo_2_rate"))
            engine->getControls().at("lfo_2_rate")->set(lfo2_freq);
    }
}


void TwytchsynthMachine::setI(int what, int val)
{
    if (engine == nullptr) return;

    float f_val = (float)val / 128.0f;
    int noteShift = 0;

    if (what == TRIG_TIME_MODE) trig_time_mode = val;
    if (what == TRIG_TIME_DURATION) {
        trig_time_duration = val;
        trig_time_duration_sample = val * 512;
    }

    if (what == NOTE_ON && val == 1) {
        if (midiDebug_) std::cerr << ">>> NoteOn TWYTCH NOTE=" << note << std::endl;
        int adjustedNote = note - noteShift;
        engine->noteOn(adjustedNote, velocity / 127.0f);
        note_on = 1;
        if (midiDebug_) std::cerr << "  NOTE on -> " << adjustedNote << std::endl;
    }

    if (what == NOTE_ON && val == 0) {
        if (midiDebug_) std::cerr << ">>> NoteOff TWYTCH NOTE=" << note << std::endl;
        int adjustedNote = note - noteShift;
        engine->noteOff(adjustedNote);
        note_on = 0;
        if (midiDebug_) std::cerr << "  NOTE off -> " << adjustedNote << std::endl;
    }

    if (what == OSC1_TYPE) {
        osc1_type = val;
        twytchhelmmopo::Value* ctrl = engine->getControl("osc_1_waveform");
        if (ctrl) ctrl->set((float)val);
    }
    if (what == OSC2_TYPE) {
        osc2_type = val;
        twytchhelmmopo::Value* ctrl = engine->getControl("osc_2_waveform");
        if (ctrl) ctrl->set((float)val);
    }
    if (what == OSC1_DETUNE) {
        osc1_detune = val;
        twytchhelmmopo::Value* ctrl = engine->getControl("osc_1_tune");
        if (ctrl) ctrl->set((f_val - 0.5f) * 2.0f);
    }
    if (what == OSC2_DETUNE) {
        osc2_detune = val;
        twytchhelmmopo::Value* ctrl = engine->getControl("osc_2_tune");
        if (ctrl) ctrl->set((f_val - 0.5f) * 2.0f);
    }
    if (what == OSC1_SCALE) {
        osc1_scale = val;
        twytchhelmmopo::Value* ctrl = engine->getControl("osc_1_transpose");
        if (ctrl) {
            int idx = (val > 4) ? std::min(4, (val * 4 + 63) / 127) : val;
            float semitones = (float)((idx - 2) * 12);
            ctrl->set(semitones);
        }
    }
    if (what == OSC2_SCALE) {
        osc2_scale = val;
        twytchhelmmopo::Value* ctrl = engine->getControl("osc_2_transpose");
        if (ctrl) {
            int idx = (val > 4) ? std::min(4, (val * 4 + 63) / 127) : val;
            float semitones = (float)((idx - 2) * 12);
            ctrl->set(semitones);
        }
    }

    if (what == ADSR_ENV0_ATTACK) {
        adsr_env0_attack = val;
        auto controls = engine->getControls();
        if (controls.count("amp_attack")) controls.at("amp_attack")->set(f_val * 4);
    }
    if (what == ADSR_ENV0_DECAY) {
        adsr_env0_decay = val;
        auto controls = engine->getControls();
        if (controls.count("amp_decay")) controls.at("amp_decay")->set(f_val * 4);
    }
    if (what == ADSR_ENV0_SUSTAIN) {
        adsr_env0_sustain = val;
        auto controls = engine->getControls();
        if (controls.count("amp_sustain")) controls.at("amp_sustain")->set(f_val);
    }
    if (what == ADSR_ENV0_RELEASE) {
        adsr_env0_release = val;
        auto controls = engine->getControls();
        if (controls.count("amp_release")) controls.at("amp_release")->set(0.02f * powf(200.0f, f_val));
    }

    if (what == ADSR_ENV1_ATTACK) {
        adsr_env1_attack = val;
        auto controls = engine->getControls();
        if (controls.count("fil_attack")) controls.at("fil_attack")->set(f_val * 4);
    }
    if (what == ADSR_ENV1_DECAY) {
        adsr_env1_decay = val;
        auto controls = engine->getControls();
        if (controls.count("fil_decay")) controls.at("fil_decay")->set(f_val * 4);
    }
    if (what == ADSR_ENV1_SUSTAIN) {
        adsr_env1_sustain = val;
        auto controls = engine->getControls();
        if (controls.count("fil_sustain")) controls.at("fil_sustain")->set(f_val);
    }
    if (what == ADSR_ENV1_RELEASE) {
        adsr_env1_release = val;
        auto controls = engine->getControls();
        if (controls.count("fil_release")) controls.at("fil_release")->set(f_val * 4);
    }

    if (what == FILTER1_TYPE) {
        filter1_type = val;
    }
    if (what == FILTER1_CUTOFF) {
        filter1_cutoff = val;
        if (midiDebug_) std::cerr << ">>> Twytch FILTER1_CUTOFF: val=" << val << " f_val=" << f_val << std::endl;
        auto controls = engine->getControls();
        
        if (controls.count("cutoff")) {
            double newVal = 28.0 + f_val * 99.0;
            if (midiDebug_) std::cerr << "    Setting cutoff to " << newVal << std::endl;
            controls.at("cutoff")->set(newVal);
        } else {
            if (midiDebug_) std::cerr << "    ERROR: 'cutoff' control not found!" << std::endl;
        }
    }
    if (what == FILTER1_RESONANCE) {
        filter1_resonance = val;
        if (midiDebug_) std::cerr << ">>> Twytch FILTER1_RESONANCE: val=" << val << " f_val=" << f_val << std::endl;
        auto controls = engine->getControls();
        if (controls.count("resonance")) {
            if (midiDebug_) std::cerr << "    Setting resonance to " << f_val << std::endl;
            controls.at("resonance")->set(f_val);
        } else {
            if (midiDebug_) std::cerr << "    ERROR: 'resonance' control not found!" << std::endl;
        }
    }

    if (what == ENV1_DEPTH) {
        env1_depth = val;
        auto controls = engine->getControls();
        if (controls.count("fil_env_depth")) {
            // fil_env_depth range: -128 to +127 (center is 0)
            controls.at("fil_env_depth")->set(((f_val * 2.0f) - 1.0f) * 128.0f);
        }
    }

    if (what == OSC12_MIX) {
        osc12_mix = val;
        auto controls = engine->getControls();
        if (controls.count("osc_mix")) {
            controls.at("osc_mix")->set(f_val);
        }
    }
    if (what == VELOCITY) velocity = val;
    if (what == OSC1_MOD) {
        osc1_mod = val;
        auto controls = engine->getControls();
        if (controls.count("osc_modulation")) {
            controls.at("osc_modulation")->set(f_val);
        }
    }

    if (what == NOTE1) {
        old_note = note;
        note = val;
        if (midiDebug_) std::cerr << "Twytch: NOTE1 set to " << val << std::endl;
    }

    if (what == OSC2_UNISON) {
        osc2_unison = val;
        auto controls = engine->getControls();
        if (controls.count("osc_2_unison_voices")) {
            controls.at("osc_2_unison_voices")->set(1.0f + f_val * 14.0f);
        }
    }
    if (what == OSC2_UNISONDT) {
        osc2_unisondt = val;
        auto controls = engine->getControls();
        if (controls.count("osc_2_unison_detune")) {
            controls.at("osc_2_unison_detune")->set(f_val * 100.0f);
        }
    }
    if (what == OSC3_TYPE) {
        osc3_type = val;
        auto controls = engine->getControls();
        if (controls.count("sub_waveform")) {
            controls.at("sub_waveform")->set(f_val * 10.0f);
        }
    }
    if (what == OSC3_AMP) {
        osc3_amp = val;
        auto controls = engine->getControls();
        if (controls.count("sub_volume")) {
            controls.at("sub_volume")->set(f_val);
        }
    }
    if (what == OSC4_AMP) {
        osc4_amp = val;
        auto controls = engine->getControls();
        if (controls.count("noise_volume")) {
            controls.at("noise_volume")->set(f_val);
        }
    }
    if (what == LFO1_FREQ) {
        lfo1_freq_raw = val;
        lfo1_freq = f_val * 10;
        auto controls = engine->getControls();
        if (controls.count("lfo_1_rate"))
            controls.at("lfo_1_rate")->set(lfo1_freq);
    }
    if (what == LFO2_FREQ) {
        lfo2_freq_raw = val;
        lfo2_freq = f_val * 10;
        auto controls = engine->getControls();
        if (controls.count("lfo_2_rate"))
            controls.at("lfo_2_rate")->set(lfo2_freq);
    }
    if (what == LFO1_DEPTH) {
        lfo1_env_amount = val;
        auto controls = engine->getControls();
        if (controls.count("lfo_1_amount"))
            controls.at("lfo_1_amount")->set(f_val);
    }
    if (what == LFO2_DEPTH) {
        lfo2_env_amount = val;
        auto controls = engine->getControls();
        if (controls.count("lfo_2_amount"))
            controls.at("lfo_2_amount")->set(f_val);
    }
    if (what == AMP) {
        amp_volume = val;
        auto controls = engine->getControls();
        if (controls.count("volume")) {
            controls.at("volume")->set(f_val);
        }
    }
}


const char* TwytchsynthMachine::getMachineParamCharStar(int machineParam, int paramValue)
{
    static const char* str_null = "NULL ";
    return str_null;
}


void TwytchsynthMachine::noteOn()
{
    if (engine == nullptr) return;
    setI(NOTE_ON, 1);
    setI(NOTE1, note);
}

void TwytchsynthMachine::noteOff()
{
    if (engine == nullptr) return;
    setI(NOTE_ON, 0);
}

void TwytchsynthMachine::reset()
{
    sample_num = 0;
    last_sample = 0;
    trig_time_mode = 0;
    trig_time_duration = 0;
    trig_time_duration_sample = 0;
}


void TwytchsynthMachine::applyCC(int cc, float normalized, const std::string& paramName)
{
    int val = (int)(normalized * 127.0f);
    if (midiDebug_) std::cerr << ">>> Twytch applyCC: cc=" << cc << " param=" << paramName << " normalized=" << normalized << " val=" << val << std::endl;

    if (paramName == "CUTOFF") {
        if (midiDebug_) std::cerr << "    -> calling setI(FILTER1_CUTOFF, " << val << ")" << std::endl;
        setI(FILTER1_CUTOFF, val);
    } else if (paramName == "RESONANCE") {
        if (midiDebug_) std::cerr << "    -> calling setI(FILTER1_RESONANCE, " << val << ")" << std::endl;
        setI(FILTER1_RESONANCE, val);
    } else if (paramName == "FILTER_ENV_AMOUNT") {
        setI(ENV1_DEPTH, val);
    } else if (paramName == "FILTER_ATTACK") {
        setI(ADSR_ENV1_ATTACK, val);
    } else if (paramName == "FILTER_DECAY") {
        setI(ADSR_ENV1_DECAY, val);
    } else if (paramName == "FILTER_SUSTAIN") {
        setI(ADSR_ENV1_SUSTAIN, val);
    } else if (paramName == "FILTER_RELEASE") {
        setI(ADSR_ENV1_RELEASE, val);
    } else if (paramName == "AMP_ATTACK") {
        setI(ADSR_ENV0_ATTACK, val);
    } else if (paramName == "AMP_DECAY") {
        setI(ADSR_ENV0_DECAY, val);
    } else if (paramName == "AMP_SUSTAIN") {
        setI(ADSR_ENV0_SUSTAIN, val);
    } else if (paramName == "AMP_RELEASE") {
        setI(ADSR_ENV0_RELEASE, val);
    } else if (paramName == "OSC1_DETUNE") {
        setI(OSC1_DETUNE, val);
    } else if (paramName == "OSC2_DETUNE") {
        setI(OSC2_DETUNE, val);
    } else if (paramName == "OSC1_SCALE") {
        setI(OSC1_SCALE, val);
    } else if (paramName == "OSC2_SCALE") {
        setI(OSC2_SCALE, val);
    } else if (paramName == "VOLUME") {
        setI(AMP, val);
    } else if (paramName == "HPF_FREQ") {
    } else {
        if (midiDebug_) std::cerr << "CC " << cc << " param=" << paramName << " unmapped in Twytch" << std::endl;
    }
}


Sint32 TwytchsynthMachine::tick()
{
    if (engine == nullptr) return 0;

    if (index >= SAM || index < 0)
        index = 0;

    if (index == 0) {
        engine->process();
        for (int i = 0; i < SAM; i++) {
            buffer_f[i] = engine->output()->buffer[i] * OUTPUT_AMPLITUDE;
            buffer_i[i] = (Sint16)buffer_f[i];
        }
    }

    Sint32 s = buffer_i[index];
    if (s > 32000) s = 32000;
    if (s < -32000) s = -32000;

    index++;
    sample_num++;
    last_sample = s;

    return s;
}

std::vector<std::pair<std::string, int>> TwytchsynthMachine::getPresetParams() const {
    return {
        {"OSC1_WAVEFORM", OSC1_TYPE},
        {"OSC2_WAVEFORM", OSC2_TYPE},
        {"OSC_MIX", OSC12_MIX},
        {"OSC1_DETUNE", OSC1_DETUNE},
        {"OSC2_DETUNE", OSC2_DETUNE},
        {"FILTER_TYPE", FILTER1_TYPE},
        {"CUTOFF", FILTER1_CUTOFF},
        {"RESONANCE", FILTER1_RESONANCE},
        {"FILTER_ENV_DEPTH", ENV1_DEPTH},
        {"AMP_ATTACK", ADSR_ENV0_ATTACK},
        {"AMP_DECAY", ADSR_ENV0_DECAY},
        {"AMP_SUSTAIN", ADSR_ENV0_SUSTAIN},
        {"AMP_RELEASE", ADSR_ENV0_RELEASE},
        {"FILTER_ENV_ATTACK", ADSR_ENV1_ATTACK},
        {"FILTER_ENV_DECAY", ADSR_ENV1_DECAY},
        {"FILTER_ENV_SUSTAIN", ADSR_ENV1_SUSTAIN},
        {"FILTER_ENV_RELEASE", ADSR_ENV1_RELEASE},
        {"VOLUME", AMP},
        {"LFO1_FREQ", LFO1_FREQ},
        {"LFO1_AMOUNT", LFO1_DEPTH}
    };
}

const char* TwytchsynthMachine::getDisplayString(int index) {
    if (index == OSC1_TYPE || index == OSC2_TYPE) {
        static const char* names[] = {"SIN", "TRGL", "SQR", "DSAW",
                                       "USAW", "3STEP", "4STEP", "8STEP",
                                       "3PYR", "5PYR", "9PYR", "NOISE"};
        int val = (index == OSC1_TYPE) ? osc1_type : osc2_type;
        if (val < 0) val = 0;
        if (val >= 12) val = 11;
        return names[val];
    }
    return "";
}