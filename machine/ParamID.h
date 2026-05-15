#ifndef PARAM_ID_H
#define PARAM_ID_H

// Unified parameter IDs shared across all engines.
// Lowercase names avoid conflicts with engine-specific UPPERCASE #defines.
// Usage: machine->setI(ParamID::cutoff, 80);
// Engine-specific params not listed here use raw integers in their UI code.

namespace ParamID {
    // Amp envelope (0-3)
    constexpr int amp_attack  = 0;
    constexpr int amp_decay   = 1;
    constexpr int amp_sustain = 2;
    constexpr int amp_release = 3;
    // Filter envelope (4-7)
    constexpr int flt_attack  = 4;
    constexpr int flt_decay   = 5;
    constexpr int flt_sustain = 6;
    constexpr int flt_release = 7;
    // LFO waveforms
    constexpr int lfo1_wave = 2;
    constexpr int lfo2_wave = 3;
    // Oscillators
    constexpr int osc1_wave   = 20;
    constexpr int osc2_wave   = 21;
    constexpr int osc1_amp    = 31;
    constexpr int osc2_amp    = 32;
    constexpr int mix         = 28;
    constexpr int osc1_scale  = 65;
    constexpr int osc2_scale  = 66;
    constexpr int osc1_detune = 74;
    constexpr int osc2_detune = 75;
    // Filter
    constexpr int cutoff    = 52;
    constexpr int resonance = 53;
    constexpr int flt_env_depth = 94;
    // LFO
    constexpr int lfo1_freq  = 42;
    constexpr int lfo1_depth = 41;
    constexpr int lfo2_freq  = 44;
    constexpr int lfo2_depth = 43;
    // Misc
    constexpr int volume     = 35;
    constexpr int note       = 70;
    constexpr int note_on    = 150;
    constexpr int velocity   = 140;
    constexpr int filter_type = 50;
    constexpr int polyphony  = 110;
    constexpr int keytrack   = 55;
    // Twytch-specific extras (common enough to name)
    constexpr int osc3_wave = 22;
    constexpr int osc4_wave = 23;
    constexpr int osc3_amp  = 33;
    constexpr int osc4_amp  = 34;
}

// MIDI CC numbers for reference.
// These map the standard MIDI CC assignments to the parameters they typically control.
// Usage: machine->applyCC(cc::mod_wheel, val/127.0f, "");
namespace cc {
    // Standard continuous controllers
    constexpr int bank_select     = 0;
    constexpr int mod_wheel       = 1;
    constexpr int breath          = 2;
    constexpr int foot_pedal      = 4;
    constexpr int portamento_time = 5;
    constexpr int volume          = 7;
    constexpr int balance         = 8;
    constexpr int pan             = 10;
    constexpr int expression      = 11;
    constexpr int effect_ctl_1    = 12;
    constexpr int effect_ctl_2    = 13;
    constexpr int general_1       = 16;
    constexpr int general_2       = 17;
    constexpr int general_3       = 18;
    constexpr int general_4       = 19;
    // On/off switches (value <64 = off, >=64 = on)
    constexpr int sustain         = 64;
    constexpr int portamento_sw   = 65;
    constexpr int sostenuto       = 66;
    constexpr int soft_pedal      = 67;
    constexpr int legato_sw       = 68;
    constexpr int hold_2          = 69;
    // Sound controllers (often mapped to synth parameters)
    constexpr int sound_var       = 70;  // Sound variation
    constexpr int resonance       = 71;  // Sound Controller 2 — filter resonance
    constexpr int release         = 72;  // Sound Controller 3 — release time
    constexpr int attack          = 73;  // Sound Controller 4 — attack time
    constexpr int cutoff          = 74;  // Sound Controller 5 — filter cutoff
    constexpr int sound_ctl_6     = 75;
    constexpr int sound_ctl_7     = 76;
    constexpr int sound_ctl_8     = 77;
    constexpr int sound_ctl_9     = 78;
    constexpr int sound_ctl_10    = 79;
    // General purpose
    constexpr int gp_decay        = 80;  // often decay time
    constexpr int gp_hpf          = 81;  // often high-pass filter
    constexpr int gp_83           = 83;
    constexpr int portamento_cc   = 84;
    // Effect sends
    constexpr int reverb_depth    = 91;
    constexpr int tremolo_depth   = 92;
    constexpr int chorus_depth    = 93;
    constexpr int detune_depth    = 94;
    constexpr int phaser_depth    = 95;
    // Data entry
    constexpr int data_increment  = 96;
    constexpr int data_decrement  = 97;
    constexpr int nrpn_lsb        = 98;
    constexpr int nrpn_msb        = 99;
    constexpr int rpn_lsb         = 100;
    constexpr int rpn_msb         = 101;
    // Channel mode messages
    constexpr int all_sound_off   = 120;
    constexpr int reset_all       = 121;
    constexpr int local_control   = 122;
    constexpr int all_notes_off   = 123;
    constexpr int omni_off        = 124;
    constexpr int omni_on         = 125;
    constexpr int mono_mode       = 126;
    constexpr int poly_mode       = 127;
}

#endif
