# ncursesynth

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

A simple, terminal-based software synthesizer built with **ncurses** for the user interface, **PortAudio** for audio playback, and **RtMidi** for MIDI input. 
Play it using a MIDI controller or your computer keyboard, all from within your terminal.

## Status: Alpha 🚧

This is the commit and a work in progress. 

## Features (Planned)

- **Terminal UI**: Clean, real-time interface using ncurses.
- **Dual Input**: Play via connected MIDI devices or computer keyboard.
- **Oscillators**: Multiple waveforms (sine, saw, square, triangle).
- **Filters**: Classic low-pass and high-pass filters with cutoff and resonance.
- **Envelopes**: ADSR envelopes for amplitude and filter control.
- **Effects**: Basic effects like reverb and delay.
- **Lightweight**: Runs entirely in a terminal window with minimal resources.

## Project Structure

```
ncursesynth/
├── audio/         # PortAudio stream management
├── effects/       # Audio effects (reverb, delay, etc.)
├── envelopes/     # ADSR envelope generators
├── filters/       # Filter implementations (LPF, HPF)
├── midi/          # RtMidi input handling
├── oscillators/   # Waveform generation
├── synth/         # Core synthesis engine
├── ui/            # Ncurses terminal interface
├── main.cpp       # Program entry point
├── Makefile       # Build configuration
└── LICENSE        # GPLv3 license
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

## Usage (Planned)

- `MIDI Controller`: Connect and play
- `Computer Keyboard`: Map keys to notes (e.g., `A` = C4, `S` = D4, etc.).
- `UI Navigation`: Use arrow keys, `Tab`, and `Enter` to adjust parameters.
- `Quit`: Press  ^C to exit.

## Contributing

This is a personal project, feel free to clone it.
Suggestions and feedback are welcome.

## License

This project is licensed under the **GNU General Public License v3.0**. See the [LICENSE](LICENSE) file for details.

## Author

Created by [yoyz](https://github.com/yoyz)

---

*Built with C++, ncurses, PortAudio, and RtMidi.*
```
