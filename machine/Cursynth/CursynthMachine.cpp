#include <iostream>
#include "cursynth_types.h"
#include "CursynthMachine.h"

#define SAM 64

CursynthMachine::CursynthMachine(int polyphony)
    : polyphony_(polyphony), engine(nullptr), cutoff(125), resonance(10),
      trig_time_mode(0), trig_time_duration(0), trig_time_duration_sample(0)
{
    setName("Cursynth");
    DPRINTF("CursynthMachine::CursynthMachine()");
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
}


CursynthMachine::~CursynthMachine()
{
    DPRINTF("CursynthMachine::~CursynthMachine()");
    if (buffer_f) free(buffer_f);
    if (buffer_i) free(buffer_i);
    delete engine;
}


void CursynthMachine::init()
{
    DPRINTF("CursynthMachine::init()");

    if (buffer_f == nullptr)
        buffer_f = (mopocursynth::mopo_float*)malloc(sizeof(mopocursynth::mopo_float) * SAM);
    if (buffer_i == nullptr)
        buffer_i = (Sint16*)malloc(sizeof(Sint16) * SAM);

    for (int i = 0; i < SAM; i++) {
        buffer_f[i] = 0;
        buffer_i[i] = 0;
    }

    sample_num = 0;
    index = 0;

    if (engine == nullptr) {
        engine = new mopocursynth::CursynthEngine();
    }
    engine->setBufferSize(SAM);
    engine->setSampleRate(DEFAULTFREQ);

    engine->getControls().at("delay time")->set(0.0);
    engine->getControls().at("delay dry/wet")->set(0.0);
    engine->getControls().at("delay feedback")->set(0);
    engine->getControls().at("cross modulation")->set(0);
    engine->getControls().at("osc 2 transpose")->set(0);
    engine->getControls().at("osc 2 tune")->set(0);
    engine->getControls().at("volume")->set(0.1);
    engine->getControls().at("lfo 1 waveform")->set(2);
    engine->getControls().at("lfo 2 waveform")->set(2);
    engine->getControls().at("mod source 1")->set(3);
    engine->getControls().at("mod source 2")->set(4);
    engine->getControls().at("mod destination 1")->set(4);
    engine->getControls().at("mod destination 2")->set(2);
    engine->getControls().at("mod scale 1")->set(0);
    engine->getControls().at("mod scale 2")->set(0);
    engine->getControls().at("lfo 1 frequency")->set(0);
    engine->getControls().at("polyphony")->set((mopocursynth::mopo_float)polyphony_);

    note = 60;
}


int CursynthMachine::checkI(int what, int val)
{
    switch (what) {
    case OSC1_TYPE:
        if (val < 0) return 0;
        if (val >= PICO_CURSYNTH_SIZE) return PICO_CURSYNTH_SIZE - 1;
        return val;

    case OSC2_TYPE:
        if (val < 0) return 0;
        if (val >= PICO_CURSYNTH_SIZE) return PICO_CURSYNTH_SIZE - 1;
        return val;

    case FILTER1_TYPE:
        if (val < 0) return 0;
        if (val >= CURSYNTH_FILTER_TYPE_SIZE) return CURSYNTH_FILTER_TYPE_SIZE - 1;
        return val;

    case LFO1_WAVEFORM:
        if (val < 0) return 0;
        if (val >= PICO_CURSYNTH_SIZE) return PICO_CURSYNTH_SIZE - 1;
        return val;

    case LFO2_WAVEFORM:
        if (val < 0) return 0;
        if (val >= PICO_CURSYNTH_SIZE) return PICO_CURSYNTH_SIZE - 1;
        return val;

    default:
        if (val < 0) return 0;
        if (val > 127) return 127;
        DPRINTF("WARNING: CursynthMachine::checkI(%d,%d)\n", what, val);
        return val;
    }
    return val;
}


int CursynthMachine::getI(int what)
{
    if (engine == nullptr) return 0;

    mopocursynth::control_map controls = engine->getControls();

    if (what == NOTE_ON) return note_on;
    if (what == NOTE1) return note;
    if (what == OSC1_TYPE) return osc1_type;
    if (what == OSC2_TYPE) return osc2_type;
    if (what == OSC1_SCALE) return osc1_scale;
    if (what == OSC2_SCALE) return osc2_scale;
    if (what == OSC1_DETUNE) return osc1_detune;
    if (what == OSC2_DETUNE) return osc2_detune;

    if (what == ADSR_ENV0_ATTACK) {
        float val = controls.at("amp attack")->current_value();
        return (int)((val / 3.0f) * 127);
    }
    if (what == ADSR_ENV0_DECAY) {
        float val = controls.at("amp decay")->current_value();
        return (int)((val / 3.0f) * 127);
    }
    if (what == ADSR_ENV0_SUSTAIN) {
        float val = controls.at("amp sustain")->current_value();
        return (int)(val * 127);
    }
    if (what == ADSR_ENV0_RELEASE) {
        float val = controls.at("amp release")->current_value();
        return (int)((val / 3.0f) * 127);
    }
    if (what == ADSR_ENV1_ATTACK) {
        float val = controls.at("fil attack")->current_value();
        return (int)((val / 3.0f) * 127);
    }
    if (what == ADSR_ENV1_DECAY) {
        float val = controls.at("fil decay")->current_value();
        return (int)((val / 3.0f) * 127);
    }
    if (what == ADSR_ENV1_SUSTAIN) {
        float val = controls.at("fil sustain")->current_value();
        return (int)(val * 127);
    }
    if (what == ADSR_ENV1_RELEASE) {
        float val = controls.at("fil release")->current_value();
        return (int)((val / 3.0f) * 127);
    }
    if (what == FILTER1_CUTOFF) {
        float val = controls.at("cutoff")->current_value();
        return (int)((val - 28) / 100.0f * 127);
    }
    if (what == FILTER1_RESONANCE) {
        float val = controls.at("resonance")->current_value();
        return (int)(val / 10.0f * 127);
    }
    if (what == OSC12_MIX) {
        float val = controls.at("osc mix")->current_value();
        return (int)(val * 127);
    }
    if (what == LFO1_FREQ) {
        float val = controls.at("lfo 1 frequency")->current_value();
        return (int)(val / 10.0f * 127);
    }
    if (what == LFO2_FREQ) {
        float val = controls.at("lfo 2 frequency")->current_value();
        return (int)(val / 10.0f * 127);
    }
    if (what == LFO1_DEPTH) {
        float val = controls.at("mod scale 1")->current_value();
        return (int)(val * 127);
    }
    if (what == LFO2_DEPTH) {
        float val = controls.at("mod scale 2")->current_value();
        return (int)(val * 127);
    }
    if (what == ENV1_DEPTH) {
        float val = controls.at("fil env depth")->current_value();
        return (int)((val / 128.0f + 1.0f) / 2.0f * 127);
    }
    if (what == AMP) {
        float val = controls.at("volume")->current_value();
        return (int)(val * 127);
    }
    return 0;
}


void CursynthMachine::setF(int what, float val)
{
    float f_val = val / 128.0f;

    if (engine == nullptr) return;

    if (what == LFO1_FREQ)
        engine->getControls().at("lfo 1 frequency")->set(f_val * 10);
    if (what == LFO2_FREQ)
        engine->getControls().at("lfo 2 frequency")->set(f_val * 10);
}


void CursynthMachine::setI(int what, int val)
{
    if (engine == nullptr) return;

    float f_val = (float)val / 128.0f;
    int noteShift = 11;

    if (what == TRIG_TIME_MODE) trig_time_mode = val;
    if (what == TRIG_TIME_DURATION) {
        trig_time_duration = val;
        trig_time_duration_sample = val * 512;
    }

    if (what == NOTE_ON && val == 1) {
        if (midiDebug_) std::cerr << ">>> NoteOn CURSYNTH NOTE=" << note << std::endl;
        int adjustedNote = note + osc1_scale - noteShift;
        engine->noteOn(adjustedNote, velocity / 32.0f);
        note_on = 1;
        if (midiDebug_) std::cerr << "  NOTE on -> " << adjustedNote << std::endl;
    }

    if (what == NOTE_ON && val == 0) {
        if (midiDebug_) std::cerr << ">>> NoteOff CURSYNTH NOTE=" << note << std::endl;
        int adjustedNote = note + osc1_scale - noteShift;
        engine->noteOff(adjustedNote);
        note_on = 0;
        if (midiDebug_) std::cerr << "  NOTE off -> " << adjustedNote << std::endl;
    }

    if (what == OSC1_TYPE) {
        osc1_type = val;
        engine->getControls().at("osc 1 waveform")->set(f_val * 128);
    }
    if (what == OSC2_TYPE) {
        osc2_type = val;
        engine->getControls().at("osc 2 waveform")->set(f_val * 128);
    }
    if (what == OSC1_DETUNE) {
        osc1_detune = val;
        engine->getControls().at("osc 2 tune")->set(((f_val * 2) - 1));
    }
    if (what == OSC1_SCALE) osc1_scale = val;
    if (what == OSC2_SCALE) osc2_scale = val;

    if (what == ADSR_ENV0_ATTACK) {
        engine->getControls().at("amp attack")->set(f_val * 3);
    }
    if (what == ADSR_ENV0_DECAY) {
        engine->getControls().at("amp decay")->set(f_val * 3);
    }
    if (what == ADSR_ENV0_SUSTAIN) {
        engine->getControls().at("amp sustain")->set(f_val);
    }
    if (what == ADSR_ENV0_RELEASE) {
        engine->getControls().at("amp release")->set(f_val * 3);
    }

    if (what == ADSR_ENV1_ATTACK) {
        engine->getControls().at("fil attack")->set(f_val * 3);
    }
    if (what == ADSR_ENV1_DECAY) {
        engine->getControls().at("fil decay")->set(f_val * 3);
    }
    if (what == ADSR_ENV1_SUSTAIN) {
        engine->getControls().at("fil sustain")->set(f_val);
    }
    if (what == ADSR_ENV1_RELEASE) {
        engine->getControls().at("fil release")->set(f_val * 3);
    }

    if (what == LFO1_ENV_AMOUNT) {
        engine->getControls().at("mod scale 1")->set(f_val);
    }
    if (what == LFO2_ENV_AMOUNT) {
        engine->getControls().at("mod scale 2")->set(f_val);
    }

    if (what == VELOCITY) velocity = val;
    if (what == OSC12_MIX) {
        engine->getControls().at("osc mix")->set(f_val);
    }
    if (what == OSC1_MOD) {
        engine->getControls().at("cross modulation")->set(f_val);
    }
    if (what == KEYTRACK) {
        engine->getControls().at("keytrack")->set(((f_val * 2) - 1) * 128);
    }
    if (what == ENV1_DEPTH) {
        engine->getControls().at("fil env depth")->set(((f_val * 2) - 1) * 128);
    }
    if (what == FILTER1_TYPE) {
        engine->getControls().at("filter type")->set(f_val * 128);
    }
    if (what == FILTER1_CUTOFF) {
        engine->getControls().at("cutoff")->set(28 + f_val * 100);
    }
    if (what == FILTER1_RESONANCE) {
        engine->getControls().at("resonance")->set(0 + f_val * 10);
    }
    if (what == FX1_DEPTH) {
        engine->getControls().at("delay dry/wet")->set(f_val * 128);
    }
    if (what == FX1_SPEED) {
        engine->getControls().at("delay time")->set(f_val * 128);
    }

    if (what == NOTE1) {
        old_note = note;
        note = val;
        if (midiDebug_) std::cerr << "Cursynth: NOTE1 set to " << val << std::endl;
    }
}


const char* CursynthMachine::getMachineParamCharStar(int machineParam, int paramValue)
{
    static const char* str_null = "NULL ";
    static const char* str_sin = "SIN  ";
    static const char* str_triangle = "TRGL ";
    static const char* str_square = "SQR  ";
    static const char* str_downsaw = "DSAW ";
    static const char* str_upsaw = "USAW ";
    static const char* str_threestep = "TSTEP";
    static const char* str_fourstep = "FSTEP";
    static const char* str_heightstep = "HSTEP";
    static const char* str_threepyramid = "TPYR ";
    static const char* str_fivepyramid = "FPYR ";
    static const char* str_ninepyramid = "NPYR ";
    static const char* str_whitenoise = "NOISE";

    const char* str_osc[PICO_CURSYNTH_SIZE];
    str_osc[PICO_CURSYNTH_SIN] = str_sin;
    str_osc[PICO_CURSYNTH_TRIANGLE] = str_triangle;
    str_osc[PICO_CURSYNTH_SQUARE] = str_square;
    str_osc[PICO_CURSYNTH_DOWNSAW] = str_downsaw;
    str_osc[PICO_CURSYNTH_UPSAW] = str_upsaw;
    str_osc[PICO_CURSYNTH_THREESTEP] = str_threestep;
    str_osc[PICO_CURSYNTH_FOURSTEP] = str_fourstep;
    str_osc[PICO_CURSYNTH_HEIGHTSTEP] = str_heightstep;
    str_osc[PICO_CURSYNTH_THREEPYRAMID] = str_threepyramid;
    str_osc[PICO_CURSYNTH_FIVEPYRAMID] = str_fivepyramid;
    str_osc[PICO_CURSYNTH_NINEPYRAMID] = str_ninepyramid;
    str_osc[PICO_CURSYNTH_WHITENOISE] = str_whitenoise;

    static const char* str_fltr_type_lp = "LP";
    static const char* str_fltr_type_hp = "HP";
    const char* str_fltr_type[CURSYNTH_FILTER_TYPE_SIZE];
    str_fltr_type[CURSYNTH_FILTER_TYPE_LP] = str_fltr_type_lp;
    str_fltr_type[CURSYNTH_FILTER_TYPE_HP] = str_fltr_type_hp;

    static const char* str_lfo_type_lfo = "PLFO";
    static const char* str_lfo_type_pb = "PB";
    const char* str_lfo_type[LFO_TYPE_SIZE];
    str_lfo_type[LFO_TYPE_PITCHLFO] = str_lfo_type_lfo;
    str_lfo_type[LFO_TYPE_PITCHBEND] = str_lfo_type_pb;

    switch (machineParam) {
    case OSC1_TYPE:
        return str_osc[this->checkI(OSC1_TYPE, paramValue)];
    case OSC2_TYPE:
        return str_osc[this->checkI(OSC2_TYPE, paramValue)];
    case FILTER1_TYPE:
        return str_fltr_type[this->checkI(FILTER1_TYPE, paramValue)];
    case LFO_TYPE:
        return str_lfo_type[this->checkI(LFO_TYPE, paramValue)];
    case LFO1_WAVEFORM:
        return str_osc[this->checkI(LFO1_WAVEFORM, paramValue)];
    case LFO2_WAVEFORM:
        return str_osc[this->checkI(LFO2_WAVEFORM, paramValue)];
    }
    return str_null;
}


void CursynthMachine::reset()
{
    sample_num = 0;
    last_sample = 0;
    trig_time_mode = 0;
    trig_time_duration = 0;
    trig_time_duration_sample = 0;
}


void CursynthMachine::applyCC(int cc, float normalized, const std::string& paramName)
{
    int val = (int)(normalized * 127.0f);

    if (paramName == "CUTOFF") {
        setI(FILTER1_CUTOFF, val);
    } else if (paramName == "RESONANCE") {
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
        if (midiDebug_) std::cerr << "CC " << cc << " param=" << paramName << " unmapped in Cursynth" << std::endl;
    }
}


Sint32 CursynthMachine::tick()
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