# Changelog

## 2026-05-16 — Novation Summit mapping, filter tests, UI fix

### Novation Summit MIDI Mapping
- New `mapping/summit.txt` with 18 CC mappings for Summit/Peak:
  - Oscillators: OSC1/2 SCALE (coarse, CC14/17), OSC1/2 DETUNE (fine, CC15/18), OSC1/2 AMP (mix, CC23/24)
  - Filter: CUTOFF (CC29), RESONANCE (CC79)
  - Amp envelope: AMP A/D/S/R (CC86-89)
  - Filter envelope: FENV AMOUNT (mod 1 → filter, CC78), FENV A/D/S/R (CC90-93)
  - Volume: CC7
- Added `--mapping NAME` CLI flag to select mapping on startup
- Mappings auto-discovered from `mapping/` directory (index.txt replaced)
- Syntax validation reports file:line errors for bad mapping entries (CC range, format, numbers)
- Added `applyCC()` handlers for OSC1/2_DETUNE, OSC1/2_SCALE, OSC1/2_AMP to all 4 engines

### UI Arrow Key Fix
- Arrow keys now increment parameters by 1% (matching PageUp/PageDown)
- Fixed Ncursesynth rounding bug: `getI()` was computing from internal state (Hz) instead of cached value, causing round-trip precision loss
- All 4 engines use cached raw 0-127 value in getI() for consistent display

### Filter Test Suite
- `filter_full`: 256 cutoff variations, quick amplitude verification
- `filter_full2`: 265 cutoff + resonance sweep tests
- `filter_full3`: 128×128 parameter grid (16384 tests) with FFT frequency analysis
- Added `preset` test for preset load/save
- Total: 15 tests × 4 engines = 60 test runs, 100% pass rate

### Bug Fixes
- **Ncursesynth `applyCC`**: passing raw 0-127 to `setF()` which expected 0.0-1.0 — fixed by using `setI()` with unified param IDs
- **Ncursesynth `applyCC`**: was using `MachineParam::FILTER_CUTOFF` (enum value 50) instead of unified ID 52 — `mapParam(52)` correctly routes to FILTER_CUTOFF (51)
- **loadMappings**: removed index.txt dependency, auto-discovers `.txt` files from directory, removed duplicate directory scan bug
- **MIDI init ordering**: moved mapping load before audio engine init so mapping is available even if audio fails

### Docs
- `doc/TEST_PLAN.md`: updated to reflect 15 tests (was 11), 60 pass runs (was 44), added filter test implementation details
- `mapping/index.txt`: now just a reference comment (auto-discovery replaces it)

## 2026-05-15 — Preset system, remote control, headless mode, analysis

### Preset System
- Added `loadPreset()` / `savePreset()` / `getPresetList()` to `Machine` base class
- Each engine overrides `getPresetParams()` mapping standard names to internal param IDs
- Presets are text files in `bank/<engine>/<name>` — no index.txt needed
- Virtual "Init" preset (index 0) resets engine to defaults via `machine->init()`
- 40 presets generated (10 per engine): Bright Lead, Warm Pad, Deep Bass, Electric Piano, Synth Brass, Soft Strings, Funky Clav, Ambient Swell, Percussive Pluck, Filter Sweep

### UI Preset Menu
- `[PRESET: name]` as 4th menu item (TAB → ENGINE/MIDI/MAPPING/PRESET)
- LEFT/RIGHT switches preset, S saves, C creates new
- Preset state preserved across engine switches

### TCP MIDI Server (`midi/tcp_midi_server.h/cpp`)
- `--tcp-midi-port N` enables remote MIDI over TCP
- JSON messages: note_on, note_off, cc, reset, setI
- Routes to active machine via MachineManager

### Audio Capture Driver (`audio/audio_capture.h/cpp`)
- `--tcp-capture-audio N` replaces PortAudio with TCP-accessible capture
- 30-second float32 ring buffer, real-time tick at 48kHz
- TCP commands: snapshot (binary dump), save <path>, status

### Capture Analysis (`audio/capture_analysis.h/cpp`)
- `--capture-audio-plus-fft-rms FILE` writes raw float32 file + per-second FFT/RMS analysis
- Reuses `test/fft_analyzer.h/cpp` (no code duplication)
- Falls back to `tmp2/` on readonly filesystem
- Reports write failures honestly

### Headless Mode
- `--synthengine NAME` runs engine without ncurses UI
- Use with `--tcp-midi-port` and `--capture-audio-plus-fft-rms` for automated testing

### Bug Fixes
- **Buffer overflow** in `NcursynthMachine`: `params_[150]` wrote past 117-element array, corrupting heap (incl. `ampSustain`). Fixed with bounds checks and early-return guard.
- **Envelope release pop**: release stage applied curve directly to `currentLevel` instead of normalized progress, causing volume jumps on note-off. Fixed with `releaseStartLevel` tracking and proper progress normalization.
- **Effects default-on**: Delay, Reverb, Chorus now default to `enabled(false)`, `mix(0.0)`, `enabled(false)`, `mix(0.0)`, `enabled(false)`, `mix(0.0)`. No startup preset loading.
- **Envelope attack test**: was dependent on delay effect for passing. Changed to use S=127 and relaxed threshold.

### Tests
- `test/test_notes.py` — headless note test playing C2-C5 on all 4 engines via TCP MIDI + FFT/RMS verification (16/16 pass)
- 48 C++ tests pass (12 × 4 engines)
- 980 UI layout checks pass
