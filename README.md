# ncursesynth

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

A polyphonic virtual analog synthesizer with MIDI support and ncurses terminal UI. Play it using a MIDI controller or your computer keyboard, all from within your terminal.

## Status: Alpha

## Features

* **Multi-Engine Architecture:** Dynamically switch between distinct synthesizer engines on the fly without interrupting the audio stream.
* **Terminal User Interface (TUI):** keyboard-driven interface built with `ncurses` for headless setups, minimal environments, distraction-free sound design.
* **Robust MIDI Integration:** support for external MIDI hardware, including device discovery, port selection
* **Visual Feedback:** Real-time visual updates on the TUI for parameter changes and incoming MIDI notes.
* **Preset Management:** TODO Save and load patches seamlessly.

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

## Dependencies

To compile and run `ncursesynth`, you will need a C++ compiler supporting standard threading (`<thread>`, `<atomic>`) and the following libraries:
* `libncurses-dev` (for the TUI)
* Standard audio/MIDI development headers (e.g., ALSA, depending on your backend)


## Command-Line Usage

You can launch `ncursesynth` with several flags to configure MIDI and debugging options before the UI loads.

| Option | Description |
| :--- | :--- |
| `--help`, `-h` | Show the help message and exit. |
| `--list-midi` | List all available MIDI devices and their hardware ports. |
| `--midi-port <hw:X,Y,Z>`| Connect to a specific MIDI port on startup (e.g., `hw:1,0,0`). |
| `--midi-debug` | Enable verbose MIDI debugging output to the console. |


## Keyboard Controls

The application is heavily optimized for keyboard-centric workflows. 

| Key | Action |
| :--- | :--- |
| **TAB** | Switch between UI menus (Engine selection, MIDI setup, Parameters). |
| **Arrow Keys** | Navigate through parameters or menu items. |
| **1-9, 0** | Instantly set the selected parameter slider from 10% to 100%. |
| **Q** or **ESC** | Safely shutdown the audio/MIDI engines and quit the application. |

## Running

```bash
./ncursesynth
```

**Note**: The synth currently requires an active audio device

## MIDI Support

### DeepMind 12 CC Mapping

The synth includes a preset mapping for Behringer DeepMind 12:
- Set DeepMind 12 to USB-CTRL CC mode
- Select MIDI device in the UI
- Select "deepmind12" mapping

### MIDI CC Mappings

Custom mapping files can be placed in the `mapping/` directory.
TODO work on other midi keyboard

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
