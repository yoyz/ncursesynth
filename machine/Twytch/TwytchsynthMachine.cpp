#include <iostream>
#include "twytch_types.h"
#include "TwytchsynthMachine.h"

#define SAM 64

TwytchsynthMachine::TwytchsynthMachine()
    : engine(nullptr), cutoff(125), resonance(10), amp_volume(64),
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
    osc1_scale = 0;
    osc2_scale = 0;
    osc1_detune = 0;
    osc2_detune = 0;
    pole = 0;
    need_note_on = 0;
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
    if (controls.count("cutoff")) controls.at("cutoff")->set(80.0);  // Default cutoff
    if (controls.count("resonance")) controls.at("resonance")->set(0.0);

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
    if (what == FILTER1_CUTOFF) return filter1_cutoff;
    if (what == FILTER1_RESONANCE) return filter1_resonance;
    if (what == OSC12_MIX) return osc12_mix;
    if (what == LFO1_FREQ) return (int)(lfo1_freq * 127);
    if (what == LFO2_FREQ) return (int)(lfo2_freq * 127);
    if (what == AMP) return amp_volume;

    return 0;
}


void TwytchsynthMachine::setF(int what, float val)
{
    float f_val = val / 128.0f;

    if (engine == nullptr) return;

    if (what == LFO1_FREQ) {
        lfo1_freq = f_val * 10;
    }
    if (what == LFO2_FREQ) {
        lfo2_freq = f_val * 10;
    }
}


void TwytchsynthMachine::setI(int what, int val)
{
    if (engine == nullptr) return;

    float f_val = (float)val / 128.0f;
    int noteShift = 12;

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
        twytchhelmmopo::Value* ctrl = engine->getControl("osc 1 waveform");
        if (ctrl) ctrl->set(f_val * 127);
    }
    if (what == OSC2_TYPE) {
        osc2_type = val;
        twytchhelmmopo::Value* ctrl = engine->getControl("osc 2 waveform");
        if (ctrl) ctrl->set(f_val * 127);
    }
    if (what == OSC1_DETUNE) {
        osc1_detune = val;
        twytchhelmmopo::Value* ctrl = engine->getControl("osc 2 detune");
        if (ctrl) ctrl->set(f_val);
    }
    if (what == OSC1_SCALE) {
        osc1_scale = val;
        twytchhelmmopo::Value* ctrl = engine->getControl("osc_1_transpose");
        if (ctrl) {
            float semitones = (val - 64) / 64.0f * 24.0f;
            ctrl->set(semitones);
        }
    }
    if (what == OSC2_SCALE) {
        osc2_scale = val;
        twytchhelmmopo::Value* ctrl = engine->getControl("osc_2_transpose");
        if (ctrl) {
            float semitones = (val - 64) / 64.0f * 24.0f;
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
        if (controls.count("amp_release")) controls.at("amp_release")->set(f_val * 4);
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
            buffer_f[i] = engine->output()->buffer[i] * 8192;
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