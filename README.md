# ncursesynth

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

A polyphonic virtual analog synthesizer with MIDI support and ncurses terminal UI. 
Play it using a MIDI controller or your computer keyboard, all from within your terminal.

## Status: Alpha

## Features

* **Multi-Engine Architecture:** Dynamically switch between distinct synthesizer engines on the fly without interrupting the audio stream.
* **Terminal User Interface (TUI):** keyboard-driven interface built with `ncurses` for headless setups, minimal environments, distraction-free sound design.
* **Robust MIDI Integration:** support for external MIDI hardware, including device discovery, port selection
* **Visual Feedback:** Real-time visual updates on the TUI for parameter changes and incoming MIDI notes.
* **Preset Management:** Save and load patches per-engine with S/C keys.

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


## Command-Line Usage

You can launch `ncursesynth` with several flags to configure MIDI and debugging options before the UI loads.

| Option | Description |
| :--- | :--- |
| `--help`, `-h` | Show the help message and exit. |
| `--list-midi` | List all available MIDI devices and their hardware ports. |
| `--midi-port <hw:X,Y,Z>`| Connect to a specific MIDI port on startup (e.g., `hw:1,0,0`). |
| `--midi-debug` | Enable verbose MIDI debugging output to the console. |
| `--tcp-midi-port N` | TCP port for remote MIDI input. |
| `--tcp-capture-audio N` | TCP port for audio capture (replaces PortAudio). |
| `--capture-audio-plus-fft-rms FILE` | Write raw audio + per-second FFT/RMS analysis. |
| `--capture-midi-plus-analysis FILE` | Write MIDI event log (note_on/off/cc) with timestamps. |
| `--synthengine NAME` | Run headless with named engine (ncursesynth/pbsynth/cursynth/twytch). |


## Keyboard Controls

The application is heavily optimized for keyboard-centric workflows. 

| Key | Action |
| :--- | :--- |
| **TAB** | Switch between parameter mode and menu mode (Engine/MIDI/Mapping/Preset). |
| **UP/DOWN** | Navigate parameters or menu items. |
| **LEFT/RIGHT** | Adjust parameter value, or switch engine/device/mapping in menu mode. |
| **PGUP/PGDN** | Increase/decrease parameter by 10%. |
| **1-9, 0** | Set parameter slider to 10%-100%. |
| **S** | Save current preset. |
| **C** | Create new preset (enter name, ENTER to confirm). |
| **Q** | Quit the application. |

## Running

```bash
./ncursesynth
```

**Note**: The synth requires an active audio device (or use `--tcp-capture-audio` for headless TCP capture).

## MIDI Support

### DeepMind 12 CC Mapping

The synth includes a preset mapping for Behringer DeepMind 12:
- Set DeepMind 12 to USB-CTRL CC mode
- Select MIDI device in the UI
- Select "deepmind12" mapping

### MIDI CC Mappings

Custom mapping files can be placed in the `mapping/` directory.
Parameter names: CUTOFF, RESONANCE, FILTER_ENV_AMOUNT, HPF_FREQ, FILTER_ATTACK,
FILTER_DECAY, FILTER_SUSTAIN, FILTER_RELEASE, AMP_ATTACK, AMP_DECAY, AMP_SUSTAIN,
AMP_RELEASE, VOLUME.

## Presets

Presets are stored per-engine in `bank/<enginename>/` directories:
- Default presets included: epiano, bass, pad, lead (ncursesynth engine)
- Save current preset with **S**, create new with **C** (enter name, ENTER to confirm)

## License

This project is licensed under the **GNU General Public License v3.0**. See the [LICENSE](LICENSE) file for details.

## Author

Created by [yoyz](https://github.com/yoyz)

---

*Built with C++, ncurses, PortAudio, and RtMidi.*
