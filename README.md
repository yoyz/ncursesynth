# ncursesynth

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

A polyphonic virtual analog synthesizer with MIDI support and ncurses terminal UI. Play it using a MIDI controller or your computer keyboard, all from within your terminal.

## Status: Alpha

## Features

- **Terminal UI**: Clean, real-time interface using ncurses
- **Dual Input**: Play via connected MIDI devices or computer keyboard
- **Dual Oscillators**: 2 oscillators with mix and detune controls
- **Multiple Waveforms**: Sawtooth, Square, Triangle
- **Polyphonic**: Configurable voice count (up to 16 voices)
- **Filters**: 8 filter types (Moog, Korg MS-20, Oberheim SEM, SVF variants)
- **Envelopes**: ADSR envelopes for amplitude and filter with curve options
- **Effects**: Delay, Reverb, Chorus, Distortion
- **Preset System**: Save and load presets
- **MIDI Mapping**: Configurable CC mappings (includes DeepMind 12 preset)

## Project Structure

```
ncursesynth/
├── audio/           # PortAudio stream management
├── effects/         # Audio effects (reverb, delay, chorus, distortion)
├── envelopes/       # ADSR envelope generators with curve options
├── filters/         # Filter implementations (Moog, Korg, Oberheim, SVF)
├── midi/            # RtMidi input handling and CC mapping
├── oscillators/     # Waveform generation
├── synth/           # Core synthesis engine and voice management
├── preset/          # Preset loading and saving
├── ui/              # Ncurses terminal interface
├── bank/            # Preset files
├── mapping/         # MIDI CC mapping files
├── main.cpp         # Program entry point
└── Makefile         # Build configuration
```

## Dependencies

To build and run ncursesynth, you'll need the following libraries installed:

- `libncurses5-dev` or `libncursesw5-dev` (terminal UI)
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

## Running

```bash
./ncursesynth
```

**Note**: The synth currently requires an active audio device and will exit cleanly if none is found.

## Controls

| Key | Action |
|-----|--------|
| Arrow Keys | Navigate parameters |
| PageUp | Increase selected parameter |
| PageDown | Decrease selected parameter |
| F2 | Open preset browser |
| A-$, ^ | Play notes (C4-B4) |
| F5 | Panic (stop all notes) |
| ESC | Release all notes gracefully / Exit browser |
| Ctrl+C | Exit |

### Preset Browser (F2)

| Key | Action |
|-----|--------|
| Up/Down | Navigate presets |
| l | Load selected preset |
| s | Save to selected preset |
| n | Create new preset |
| Enter | Confirm save |
| ESC | Exit browser |

### Keyboard Layout

```
Key:  A  Z  E  R  T  Y  U  I  O  P  ^  $
Note: C  C# D  D# E  F  F# G  G# A  A# B
```

## MIDI Support

### DeepMind 12 CC Mapping

The synth includes a preset mapping for Behringer DeepMind 12:
- Set DeepMind 12 to USB-CTRL CC mode
- Select MIDI device in the UI
- Select "deepmind12" mapping

### MIDI CC Mappings

Custom mapping files can be placed in the `mapping/` directory.

## Presets

Presets are stored in `bank/ncursesynth/`:
- Default presets included: epiano, bass, pad, lead
- Save custom presets using the preset browser (F2)

## License

This project is licensed under the **GNU General Public License v3.0**. See the [LICENSE](LICENSE) file for details.

## Author

Created by [yoyz](https://github.com/yoyz)

---

*Built with C++, ncurses, PortAudio, and RtMidi.*