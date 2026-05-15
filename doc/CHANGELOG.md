# Changelog

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
