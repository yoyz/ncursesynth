# ncursesynth

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

A polyphonic virtual analog synthesizer with MIDI support and an ncurses terminal UI. Play it using a MIDI controller or your computer keyboard, all from within your terminal.

## Status: Alpha (v0.2.1)

## Features

- **Six synth engines** in one binary — switch between them on the fly without interrupting the audio stream.
- **Terminal User Interface (TUI)** built with `ncurses` for headless setups, minimal environments, distraction-free sound design.
- **Robust MIDI integration** — external MIDI hardware with device discovery and port selection, plus CC mapping files.
- **Master effects chain** — shared post-mix delay/reverb/chorus/distortion, editable at runtime (F key).
- **Preset management** — save/load patches per engine (S/C keys); Ambika and Twytch also ship embedded factory patch banks.
- **Visual feedback** — real-time parameter, MIDI note and output-level indicators.
- **Headless operation** — run engines without the UI for automated testing and TCP remote control.

## Engines

| Engine | Description |
| :--- | :--- |
| Ncursesynth | Original engine (11 filter types, flexible envelopes, effects) |
| PBSynth | 8-voice polyphonic VA engine |
| Cursynth | Cursynth engine port |
| Twytch | Helm-based engine with 274 factory patches |
| Digits | Casio CZ-style phase-distortion engine |
| Ambika | 6-voice Ambika softsynth port with 208 factory patches |

See [doc/adding_engine_guide.md](doc/adding_engine_guide.md) to integrate a new engine.

## Dependencies

To build and run ncursesynth, you'll need the following libraries installed:

- `libncurses5-dev` / `libncursesw5-dev` (terminal UI)
- `portaudio19-dev` (audio output)
- `librtmidi-dev` (MIDI input)

On **Debian/Ubuntu**:
```bash
sudo apt-get install libncurses5-dev portaudio19-dev librtmidi-dev
```

## Building

```bash
git clone https://github.com/yoyz/ncursesynth.git
cd ncursesynth
make
```

This produces the `virtual_synth` binary.

## Running

```bash
./virtual_synth
```

The synth requires an active audio device (or use `--tcp-capture-audio` for headless TCP capture).

## Command-Line Options

| Option | Description |
| :--- | :--- |
| `--help`, `-h` | Show the help message and exit. |
| `--list-midi` | List all available MIDI devices. |
| `--midi-port <hw:X,Y,Z>` | Connect to a specific MIDI port on startup. |
| `--midi-debug` | Enable verbose MIDI debugging output. |
| `--synthengine NAME` | Run headless with a named engine (ncursesynth/pbsynth/cursynth/twytch/digits/ambika). |
| `--mapping NAME` | Select a MIDI CC mapping on startup. |
| `--pc-keyboard` | Use the PC keyboard as a piano (AZERTY layout, Z/X to change octave). |
| `--tcp-midi-port N` | TCP port for remote MIDI input. |
| `--tcp-capture-audio N` | TCP port for audio capture (replaces PortAudio). |
| `--capture-audio-plus-fft-rms FILE` | Write raw audio + per-second FFT/RMS analysis. |
| `--capture-midi-plus-analysis FILE` | Write MIDI event log with timestamps. |
| `--headless` | Run without the UI (combine with `--synthengine`). |

`--fps N`, `--buffer-size N`, `--latency-ms N`, `--limiter-threshold N` and `--version` are also available; run `./virtual_synth --help` for the full list. The TCP/headless/capture features are documented in [doc/remote_control.md](doc/remote_control.md).

## Keyboard Controls

| Key | Action |
| :--- | :--- |
| **TAB** | Switch between parameter mode and menu mode. |
| **UP/DOWN** | Navigate parameters or menu items. |
| **LEFT/RIGHT** | Adjust parameter value, or switch engine/device/mapping/preset in menu mode. |
| **PGUP/PGDN** | Increase/decrease parameter by ~10%. |
| **1-9, 0** | Set parameter slider to 0%-90%. |
| **F** | Toggle the master effects view. |
| **S** | Save current preset. |
| **C** | Create new preset (enter name, ENTER to confirm). |
| **Q** | Quit the application. |

Menu mode cycles through Engine / MIDI / Mapping / Preset, plus **Factory Patch** on engines with an embedded bank. In the master FX view, `A` adds an effect, `X` removes it, and `[`/`]` reorder the chain. With `--pc-keyboard`, `a-z...` keys play notes and `Z`/`X` shift the octave.

## MIDI Support

### DeepMind 12 CC Mapping

- Set DeepMind 12 to USB-CTRL CC mode
- Select MIDI device in the UI
- Select "deepmind12" mapping

### CC Mappings

Mapping files live in `mapping/` and are auto-discovered on startup (deepmind12, summit, prologue, mysynth). The `midi_learn` / `midi_client` tools can learn and generate mappings for a hardware synth — see [doc/midi_learn_midi_client.txt](doc/midi_learn_midi_client.txt).

Parameter names: CUTOFF, RESONANCE, FILTER_ENV_AMOUNT, HPF_FREQ, FILTER_ATTACK,
FILTER_DECAY, FILTER_SUSTAIN, FILTER_RELEASE, AMP_ATTACK, AMP_DECAY, AMP_SUSTAIN,
AMP_RELEASE, VOLUME.

## Presets

Presets are stored per-engine in `bank/<engine>/` directories. The ncursesynth, PBSynth, Cursynth, Twytch and Digits engines each ship 10 factory presets (Bright Lead, Warm Pad, Deep Bass, Electric Piano, Synth Brass, Soft Strings, Funky Clav, Ambient Swell, Percussive Pluck, Filter Sweep); Ambika ships its own bank. Save the current preset with **S**, create a new one with **C**.

Ambika and Twytch additionally embed large factory patch banks compiled into the binary — see [doc/patch.md](doc/patch.md).

## Testing

```bash
make test_runner
./test_runner --all-engines --all-tests
```

The suite runs 29 tests (audio, envelope, filter, MIDI mapping, preset round-trips, master FX, ...) across all six engines. UI layout checks build with `make test_ui`. See [doc/TEST_PLAN.md](doc/TEST_PLAN.md).

## Documentation

- [doc/CHANGELOG.md](doc/CHANGELOG.md) — changelog
- [doc/remote_control.md](doc/remote_control.md) — TCP MIDI, audio capture, headless mode
- [doc/TEST_PLAN.md](doc/TEST_PLAN.md) — testing framework
- [doc/param_mapping.md](doc/param_mapping.md) — parameter ID systems
- [doc/adding_engine_guide.md](doc/adding_engine_guide.md) — integrating a new engine
- [doc/patch.md](doc/patch.md) — embedded factory patch banks
- [doc/ambika_original_softsynth_difference.md](doc/ambika_original_softsynth_difference.md) — Ambika port differences
- [doc/midi_learn_midi_client.txt](doc/midi_learn_midi_client.txt) — MIDI learn tools

## License

This project is licensed under the **GNU General Public License v3.0**. See the [LICENSE](LICENSE) file for details.

## Author

Created by [yoyz](https://github.com/yoyz)

---

*Built with C++, ncurses, PortAudio, and RtMidi.*
