# Parameter ID Systems

## Two Systems

### System A: Ncursynth-style (`MachineParam` enum, `Parameter.h`)

Uses an enum with base offsets for each section:

| Section | Base | Params |
|---------|------|--------|
| OSC     | 0    | OSC_1_WAVEFORM(0), OSC_1_VOLUME(1), ..., OSC_2_TUNE(11) |
| LFO     | 20   | LFO_1_WAVEFORM(20), LFO_1_FREQUENCY(21), LFO_1_AMOUNT(22) |
| FILTER  | 50   | FILTER_TYPE(50), FILTER_CUTOFF(51), RESONANCE(52), ... |
| NOTE    | 70   | NOTE_ON(70), NOTE_HZ(71) |
| AMP_ENV | 80   | AMP_ATTACK(80), AMP_DECAY(81), AMP_SUSTAIN(82), AMP_RELEASE(83) |
| FILTER_ENV | 90 | FILTER_ENV_ATTACK(90), FILTER_ENV_DECAY(91), FILTER_ENV_SUSTAIN(92), FILTER_ENV_RELEASE(93) |
| EFFECTS | 100  | DELAY_TIME(100), DELAY_FEEDBACK(101), ... |
| PERFORMANCE | 110 | POLYPHONY(110), LEGATO(111), ... |
| MOD_MATRIX | 130 | MOD_SOURCE_1(130), ... |

**Used by**: `NcursynthMachine` only.

### System B: PBSynth-style (hardcoded `#define`, `pbsynth_types.h`)

Uses bare integer defines without section offsets:

| Define | Value |
|--------|-------|
| ADSR_ENV0_ATTACK | 0 |
| ADSR_ENV0_DECAY | 1 |
| ADSR_ENV0_SUSTAIN | 2 |
| ADSR_ENV0_RELEASE | 3 |
| ADSR_ENV1_ATTACK | 4 |
| ADSR_ENV1_DECAY | 5 |
| ADSR_ENV1_SUSTAIN | 6 |
| ADSR_ENV1_RELEASE | 7 |
| OSC1_TYPE | 20 |
| OSC2_TYPE | 21 |
| OSC12_MIX / VCO_MIX | 28 |
| AMP | 35 |
| LFO1_DEPTH | 41 |
| LFO1_FREQ | 42 |
| LFO2_DEPTH | 43 |
| LFO2_FREQ | 44 |
| FILTER1_TYPE | 50 |
| FILTER1_CUTOFF | 52 |
| FILTER1_RESONANCE | 53 |
| OSC1_SCALE | 65 |
| OSC2_SCALE | 66 |
| NOTE1 | 70 |
| OSC1_DETUNE | 74 |
| OSC2_DETUNE | 75 |
| ENV1_DEPTH | 94 |
| NOTE_ON | 150 |

**Used by**: `PBSynthMachine`, `CursynthMachine`, `TwytchsynthMachine`.

---

## Per-Engine `setI()` Matrix

Shows what each engine's `setI()` actually handles, and which ID system it uses.

| Concept | Ncursynth (MachineParam) | PBSynth (pbsynth defines) | Cursynth (pbsynth defines) | Twytch (pbsynth defines) |
|---------|--------------------------|---------------------------|---------------------------|--------------------------|
| OSC1 WAVEFORM | `OSC_1_WAVEFORM` **0** | `OSC1_TYPE` **20** | `OSC1_TYPE` **20** | `OSC1_TYPE` **20** |
| OSC2 WAVEFORM | `OSC_2_WAVEFORM` **4** | `OSC2_TYPE` **21** | `OSC2_TYPE` **21** | `OSC2_TYPE` **21** |
| OSC MIX | `OSC_MIX` **9** | `VCO_MIX` / `OSC12_MIX` **28** | `OSC12_MIX` **28** | `OSC12_MIX` **28** |
| OSC1 DETUNE | `OSC_2_TUNE` **11** | `OSC1_DETUNE` **74** | `OSC1_DETUNE` **74** | `OSC1_DETUNE` **74** |
| OSC2 DETUNE | — | `OSC2_DETUNE` **75** | — | `OSC2_DETUNE` **75** |
| OSC1 SCALE | — | `OSC1_SCALE` **65** | `OSC1_SCALE` **65** | `OSC1_SCALE` **65** |
| OSC2 SCALE | — | `OSC2_SCALE` **66** | `OSC2_SCALE` **66** | `OSC2_SCALE` **66** |
| CUTOFF | `FILTER_CUTOFF` **51** | `FILTER1_CUTOFF` **52** | `FILTER1_CUTOFF` **52** | `FILTER1_CUTOFF` **52** |
| RESONANCE | `FILTER_RESONANCE` **52** | `FILTER1_RESONANCE` **53** | `FILTER1_RESONANCE` **53** | `FILTER1_RESONANCE` **53** |
| AMP ATTACK | `AMP_ATTACK` **80** | `ADSR_ENV0_ATTACK` **0** | `ADSR_ENV0_ATTACK` **0** | `ADSR_ENV0_ATTACK` **0** |
| AMP DECAY | `AMP_DECAY` **81** | `ADSR_ENV0_DECAY` **1** | `ADSR_ENV0_DECAY` **1** | `ADSR_ENV0_DECAY` **1** |
| AMP SUSTAIN | `AMP_SUSTAIN` **82** | `ADSR_ENV0_SUSTAIN` **2** | `ADSR_ENV0_SUSTAIN` **2** | `ADSR_ENV0_SUSTAIN` **2** |
| AMP RELEASE | `AMP_RELEASE` **83** | `ADSR_ENV0_RELEASE` **3** | `ADSR_ENV0_RELEASE` **3** | `ADSR_ENV0_RELEASE` **3** |
| FILTER ATTACK | `FILTER_ENV_ATTACK` **90** | `ADSR_ENV1_ATTACK` **4** | `ADSR_ENV1_ATTACK` **4** | `ADSR_ENV1_ATTACK` **4** |
| FILTER DECAY | `FILTER_ENV_DECAY` **91** | `ADSR_ENV1_DECAY` **5** | `ADSR_ENV1_DECAY` **5** | `ADSR_ENV1_DECAY` **5** |
| FILTER SUSTAIN | `FILTER_ENV_SUSTAIN` **92** | `ADSR_ENV1_SUSTAIN` **6** | `ADSR_ENV1_SUSTAIN` **6** | `ADSR_ENV1_SUSTAIN` **6** |
| FILTER RELEASE | `FILTER_ENV_RELEASE` **93** | `ADSR_ENV1_RELEASE` **7** | `ADSR_ENV1_RELEASE` **7** | `ADSR_ENV1_RELEASE` **7** |
| FILTER ENV DEPTH | `FILTER_ENV_DEPTH` **54** | `ENV1_DEPTH` **94** | `ENV1_DEPTH` **94** | `ENV1_DEPTH` **94** |
| VOLUME | **35** | `AMP` **35** | `AMP` **35** | `AMP` **35** |
| LFO1 WAVEFORM | `LFO_1_WAVEFORM` **20** | — | — | — |
| LFO1 FREQ | `LFO_1_FREQUENCY` **21** | `LFO1_FREQ` **42** | `LFO1_FREQ` **42** | `LFO1_FREQ` **42** |
| LFO1 AMOUNT | `LFO_1_AMOUNT` **22** | `LFO1_DEPTH` **41** | `LFO1_DEPTH` **41** | `LFO1_DEPTH` **41** |
| FILTER TYPE | `FILTER_TYPE` **50** | `FILTER1_TYPE` **50** | — | — |

---

## What "Ncursynth-style MachineParam" Means

In `Parameter.h`, `MachineParam::AMP_ATTACK` = **80**. This is because the enum uses base offsets: `AMP_ENV` = 80, so `AMP_ATTACK = 80`, `AMP_DECAY = 81`, etc.

In `pbsynth_types.h`, `ADSR_ENV0_ATTACK` = **0**. This is a flat namespace with no section offsets.

So when Cursynth UI defined `P_AMP_ATTACK = 80` and passed it to `CursynthMachine::setI(80, val)`, the setI method checked `if (what == ADSR_ENV0_ATTACK)` which is `if (what == 0)` — and **80 != 0**, so nothing happened.

---

## Potential Pitfalls

### Common ID 50 means different things

Both systems define **50** as the filter type, but the values are:
- Ncursynth: `FILTER_TYPE` (50) → selects MOOG/KORG_MS20/etc.
- PBSynth: `FILTER1_TYPE` (50) → selects LP12/LP24/HP12/HP24 etc.

Same number, different meaning. They happen to coincide numerically but the engine-specific enums are different.

### Common ID 35 = VOLUME

Both systems use **35** for volume:
- Ncursynth: `case 35: synth_->setVolume(value / 127.0f)`
- PBSynth/Cursynth/Twytch: `AMP` = 35, sets `amp_volume`

This one works across all engines.

### LFO frequencies

Ncursynth uses 21/22 (offset-based), PBSynth uses 42/44 (flat).

### Preset system cross-mapping

The `getPresetParams()` in each engine maps common names to engine-specific IDs:
```
"CUTOFF" → Ncursynth: 51, PBSynth: 52, Cursynth: 52, Twytch: 52
"RESONANCE" → Ncursynth: 52, PBSynth: 53, Cursynth: 53, Twytch: 53
"AMP_ATTACK" → Ncursynth: 80, PBSynth: 0, Cursynth: 0, Twytch: 0
"FILTER_ENV_ATTACK" → Ncursynth: 90, PBSynth: 4, Cursynth: 4, Twytch: 4
```

This is why the same `.raw` preset file works across all engines despite different internal IDs.
