# Ambika: ncursesynth vs. Original (pichenettes ATMega + softsynth)

> **Status:** the fixes below under "Suggested fixes" have been applied
> (FENV/FLFO unipolar 0–63, engine-based `getI` cache sync, OSC2 default,
> widget range). See the **Changes applied** section for details.

Audit of the ncursesynth Ambika machine (`machine/Ambika/`) and UI
(`ui/ambika_ui.cpp`) against the original repository:

- `pichenettes/` — the ATMega voicecard/controller firmware that drives the
  **external analog filters** (SMR4/SSI2144, SSM2164 SVF, LM13700/SSM2164
  4-pole) via PWM control voltages.
- `softsynth/` — the repo's official 6-voice software port, which replaces the
  analog audio path (DAC + analog filter) with a digital implementation.

## Overall verdict

The synthengine is a faithful port of the repo's `softsynth/` directory (the
documented software replacement for the ATMega firmware). The following files
are **byte-identical** to the original softsynth:

| File | Status |
|------|--------|
| `common/patch.h` | identical (also identical to pichenettes) |
| `common/lfo.h` | identical |
| `voicecard/envelope.h` | identical |
| `voicecard/oscillator.h` / `.cpp` | identical |
| `voicecard/sub_oscillator.h` | identical |
| `voicecard/transient_generator.h` | identical |
| `voicecard/filter.h` | identical |
| `voicecard/resources.h` / `.cpp` | identical |
| `avrlib/*` | identical |

The deviations from the pichenettes original are concentrated in:

1. the **FENV AMT parameter mapping**,
2. a few **UI default values**,
3. the **external → digital filter substitution**.

---

## FENV AMT

### Current implementation

- UI: `ui/ambika_ui.cpp:38` — `Widget("FENV AMT", ParamID::flt_env_depth, 9, 40, WidgetType::BIPOLAR, 0.5f)` → bipolar 0–127, default 64.
- Engine: `machine/Ambika/AmbikaMachine.cpp:228-246` — bipolar 0–127 with a
  power curve (`mag = t^1.5`), centered at 64 → patch `filter_env` −128..+127
  (`plugin_shared.h:594-598`).
- Routing: hardcoded `cutoff += S8U8Mul(patch_.filter_env, ENV_2)` — **faithful**
  to the original's "ENV2TVCF" (`voice.cpp:283-288`).

### Original pichenettes

- `controller/parameter.cc:511-516` — `PRM_PATCH_FILTER1_ENV` ("ENV2TVCF"),
  **unipolar 0..63**, MIDI CC 3; default patch `filter_env = 63` (max).
- Hardcoded routing: `cutoff = S16ClipU14(cutoff + S8U8Mul(patch_.filter_env, ENV_2))`
  (pichenettes `voice.cc:311-315`).

### Findings

1. **Range mismatch — FIXED.** Original is unipolar 0..63; ncursesynth was
   bipolar −100%..+100% → `filter_env` −128..+127. Now unipolar: UI raw 0–127
   maps linearly to patch `filter_env` 0..63 (`AmbikaMachine.cpp` setI, and
   `plugin_shared.h` ApplyParam `(int8_t)(v * 63.0f)`). Same for FLFO AMT.
2. **Default mismatch — FIXED.** Widget default 64 → `filter_env = 0` (no
   filter envelope). Now `GetDefault(PARAM_FILTER_ENV_AMOUNT) = 48/63` and the
   widget default is `48.0f/63.0f`, so the default matches `init_patch`'s
   `filter_env = 48` (and the original softsynth init patch). FLFO defaults
   to 0 like the original.
3. **MIDI CC direction — FIXED (direction only).** Original CC 3 carries FENV
   0..63. `applyCC("FILTER_ENV_AMOUNT")` → `setI(flt_env_depth, val)` now
   interprets 0..127 unipolarly, so higher CC = more positive env amount. The
   0–127→0–63 scaling is still linear rather than the original's raw 0–63
   CC use.
4. **Routing source is faithful** — both use Env2 (amp envelope) hardcoded to
   cutoff, matching "ENV2TVCF". But the ncursesynth default patch *also* routes
   Env1 → cutoff via mod-matrix slot 1 (`voice.cpp:51`, amount 48), so the
   filter sees Env1 *and* Env2 on top of the hardcoded FENV — not the case in
   the original default patch.
5. **Stale cache risk — FIXED.** `getI` no longer returns a local cache; it
   reads the engine state via `m_vm->GetParam()` (with exact denormalization
   for discrete params), so values stay in sync after any preset load
   (file-based or internal bank).
6. **Cosmetic** — widget `%` readout is linear; the unipolar mapping is linear
   so the readout now matches the applied `filter_env` (0..63 = 0..100%).

---

## UI issues (vs original)

- **OSC2 SHAPE defaults to NONE — FIXED.** (`ambika_ui.cpp:18` default `0.0f`)
  while `init_patch` and the bank presets use SQUARE. Now defaults to SQUARE
  (index 2).
- **`GetDefault(PARAM_OSC2_SHAPE) = 2.0f/22.0f` — FIXED.** Wrong denominator;
  everywhere else uses `WAVEFORM_LAST - 2` (= 36). Now
  `2.0f / (float)(WAVEFORM_LAST - 2)` → SQUARE (idx 2) instead of TRIANGLE.
- **Osc shape `discreteCount=35` vs engine `kNumOscShapes=37` — FIXED.**
  Widgets now use 37 entries, so all 37 waveforms (NONE..WT16) are reachable.
- **F MODE exposes NOTCH (4 modes)** — the original UI caps at HP (mode 2).
  The SVF hardware supports notch (`voicecard.cc:129`), so a harmless
  extension.
- **F TYPE** — not a patch parameter in the original; filter type is a
  physical voicecard choice (SMR4/SVF/4-pole). Making it a 2-pole/4-pole
  toggle is a reasonable softsynth extension.
- **Default CUTOFF=95, RESONANCE=0** widgets vs init_patch `127/32` — the
  shipped init sound is darker. `MachineUI::init()` applies all widget
  defaults via `setI`, so `init_patch` only survives for fields the UI does
  not cover.

---

## Synthengine vs external filter

- The original **has no digital filter**; the voicecard writes CVs to the
  analog chip — `vcf_cutoff_out.Write(voice.cutoff())`,
  `vcf_resonance_out.Write(...)`, mode via parallel port
  (`voicecard.cc:141-143`). ncursesynth replaces it with the TPT SVF in
  `filter.h`, **identical to the official softsynth's filter.h** (documented in
  `softsynth/doc/filter.md`).
- The digital mapping (20 Hz → ~7.5 kHz **linear**, Q 0.5 → 15) differs from
  the original's analog **exponential** 1V/oct response. That is the official
  softsynth's documented design decision, not a regression introduced here.
- **Faithful:** cutoff pitch-tracking (`cutoff + pitch_value_ − 8192`,
  `voice.cpp:228-234`) and the hardcoded `filter_env`/`filter_lfo`
  (Env2/LFO2) modulations both match the original.
- **Additions over the original voicecard:**
  - per-sample VCA gain cell (`voice.cpp:491-496`), emulating the analog gain
    cell;
  - LFO1/2/3 — note LFO2 (used by FLFO AMT) runs at `env_lfo[1].rate`, which
    is 0 by default and **not exposed in the UI**, so FLFO AMT is effectively
    inert unless a patch sets that rate (the original hardware is equally inert
    here since the voicecard never renders LFO1–3).
- **`kPitchTableStart` 116 → 112** (`voice.h:38-40`) is an intentional,
  commented retuning deviation (A4/MIDI-69 on equal temperament).

---

## Changes applied

1. **FENV AMT unipolar 0–63** (`ui/ambika_ui.cpp`, `AmbikaMachine.cpp`,
   `plugin_shared.h`): UI raw 0–127 → patch `filter_env` 0..63 linear; default
   now 48 (matches `init_patch` and the original softsynth init). FLFO AMT made
   unipolar 0–63 as well, default 0.
2. **Stale-cache fix** (`AmbikaMachine.cpp/.h`): removed the `m_osc1Shape`,
   `m_osc2Shape`, `m_filterMode`, `m_filterType`, `m_mixOp`, `m_lfoShape`,
   `m_fltEnvDepth` caches. `getI`/`getDisplayString` now read the engine's
   current parameter state, so values stay correct after any preset load
   (verified: internal-bank `LoadPreset` syncs `osc1_wave` and `flt_env_depth`
   exactly).
3. **OSC2 default** — widget defaults to SQUARE (2); `GetDefault(PARAM_OSC2_SHAPE)`
   fixed from `2/22` to `2/(WAVEFORM_LAST-2)`.
4. **Widget range** — OSC1/OSC2 SHAPE widgets widened to 37 entries so all
   waveforms NONE..WT16 are selectable.

---

## Suggested fixes (priority)

1. ~~Make **FENV AMT unipolar 0–63** to match the original and CC-3 semantics.~~ **Applied.**
2. ~~Align the **OSC2 default** (widget + `GetDefault`) and fix the
   `2.0f/22.0f` denominator.~~ **Applied.**
3. ~~Sync the `m_fltEnvDepth`/discrete caches when the internal bank
   (`PluginVoiceManager::LoadPreset`) is used.~~ **Applied** (caches removed;
   `getI` reads engine state).
4. ~~Widen the osc-shape widget to 37 entries or clamp engine to the widget
   range consistently.~~ **Applied** (widgets now 37 entries).
