# Remote Control & Audio Capture

Two TCP-based features for remote control and headless audio capture, designed for debugging audio quality issues (saturation, noise floor) and automated testing.

## Usage

```bash
./virtual_synth --tcp-midi-port 9000 --tcp-capture-audio 9001
```

| Flag | Description |
|------|-------------|
| `--tcp-midi-port N` | Enable TCP MIDI server on port N |
| `--tcp-capture-audio N` | Enable TCP audio capture on port N (replaces PortAudio) |
| `--capture-audio-plus-fft-rms FILE` | Write raw float32 audio + per-second FFT/RMS analysis on exit |
| `--capture-midi-plus-analysis FILE` | Write MIDI event log (note_on/off/cc) with timestamps |
| `--synthengine NAME` | Run headless with named engine (ncursesynth/pbsynth/cursynth/twytch) |

`--tcp-midi-port` and `--tcp-capture-audio` are independent — use either or both.

---

## TCP MIDI Server

Receives MIDI messages over TCP and forwards them to the currently active engine.

### Protocol

JSON messages, newline-delimited, on the specified TCP port.

**Note On:**
```json
{"type":"note_on","note":60,"velocity":100}
```

**Note Off:**
```json
{"type":"note_off","note":60}
```

**CC Message:**
```json
{"type":"cc","cc":29,"value":100}
```

**Reset Engine:**
```json
{"type":"reset"}
```

**Raw setI:**
```json
{"type":"setI","index":70,"value":60}
```

### Example

```bash
# Send a note
echo '{"type":"note_on","note":60,"velocity":100}' | nc 127.0.0.1 9000

# Send a CC (filter cutoff)
echo '{"type":"cc","cc":29,"value":80}' | nc 127.0.0.1 9000

# Turn note off
echo '{"type":"note_off","note":60}' | nc 127.0.0.1 9000
```

---

## Audio Capture Driver

Replaces PortAudio with a real-time capture loop that writes audio samples to a ring buffer. The buffer holds 30 seconds of float32 mono audio at 48kHz. Served over TCP for remote retrieval.

### Protocol

Text commands, newline-delimited. Responses vary by command type.

**snapshot** — Retrieve current buffer contents.

Returns `[4-byte uint32 sample count (big-endian)] + [N × float32 samples]`.

```bash
echo -e "snapshot" | nc 127.0.0.1 9001 | dd bs=4 skip=1 of=capture.raw
```

The first 4 bytes are the sample count; remaining bytes are raw float32 samples.
Import in Audacity as: **File → Import → Raw Data**, 48000 Hz, Mono, 32-bit float.

**save <path>** — Write current buffer to a binary file on the server.

```bash
echo -e "save tmp2/my_capture.raw" | nc 127.0.0.1 9001
```

Response: `OK <count> samples written` or `ERR cannot open file`.

**status** — Get capture statistics.

```bash
echo -e "status" | nc 127.0.0.1 9001
```

Response: `samples=<total> ring=<ring_buffer_fill_count>`

### File Format

Raw float32 (32-bit IEEE 754), mono, 48000 Hz sample rate. No header — import as raw.

---

## Workflow Example

### Debugging Audio Saturation

```bash
# Terminal 1: start synth in capture mode
./virtual_synth --tcp-midi-port 9000 --capture-audio 9001

# Terminal 2: trigger a note, wait for steady state, capture
echo '{"type":"note_on","note":60,"velocity":100}' | nc 127.0.0.1 9000
sleep 2
echo -e "save tmp2/note60.raw" | nc 127.0.0.1 9001

# Analyze
python3 -c "
import struct
with open('tmp2/note60.raw','rb') as f:
    samples = struct.unpack('{}f'.format((len(f.read()))//4), f.read())
rms = (sum(s*s for s in samples)/len(samples))**0.5
peak = max(abs(s) for s in samples)
print(f'Samples: {len(samples)}, RMS: {rms:.4f}, Peak: {peak:.4f}')
print(f'Clip ratio: {sum(1 for s in samples if abs(s)>=0.949)/len(samples)*100:.1f}%')
"
```

### Headless Preset Testing

```bash
# Start with a specific preset loaded via the UI, then:
echo '{"type":"note_on","note":48,"velocity":100}' | nc 127.0.0.1 9000
sleep 1
echo -e "snapshot" | nc 127.0.0.1 9001 > tmp2/bass_note.raw
echo '{"type":"note_off","note":48}' | nc 127.0.0.1 9000
```

---

## Implementation

| Component | File | Description |
|-----------|------|-------------|
| TcpMidiServer | `midi/tcp_midi_server.h/cpp` | TCP server accepting JSON MIDI messages |
| AudioCaptureDriver | `audio/audio_capture.h/cpp` | Real-time capture loop, ring buffer, TCP server |
| CaptureAnalyzer | `audio/capture_analysis.h/cpp` | Raw audio dump + per-second FFT/RMS analysis |
| MidiCapture | `midi/midi_capture.h/cpp` | MIDI event logger (note_on/off/cc) |
| AudioLevel | `audio/audio_level.h/cpp` | Global peak level meter for UI |
| FFTAnalyzer (reused) | `test/fft_analyzer.h/cpp` | FFT computation and frequency detection |
| Integration | `main.cpp` | CLI flags `--tcp-midi-port` and `--capture-audio` |

### AudioCaptureDriver internals

- **Ring buffer**: 1,440,000 float32 samples (30 seconds at 48kHz)
- **Tick timing**: `sleep_until` loop at 20.8µs per sample
- **Clipping**: Same hard clip at ±0.95 as PortAudio path
- **Thread safety**: `std::atomic<Machine*>` for machine pointer, atomic index for ring buffer
- **No PortAudio dependency**: When `--capture-audio` is active, `Pa_Initialize()` is never called

### TcpMidiServer internals

- Simple `accept()` loop, one client at a time
- JSON parsed via lightweight string search (no external parser dependency)
- Forwards to `MachineManager::getCurrentMachine()` for engine-safe access

---

## Headless Mode

Runs a synth engine without ncurses UI. Useful for automated testing and analysis via TCP MIDI + capture.

```bash
./virtual_synth --synthengine ncursesynth --tcp-midi-port 9000 --capture-audio-plus-fft-rms tmp2/test.raw
```

| Flag | Description |
|------|-------------|
| `--synthengine NAME` | Engine name: `ncursesynth`, `pbsynth`, `cursynth`, `twytch` |

When `--synthengine` is specified, the program skips ncurses UI initialization
and runs a simple sleep loop waiting for SIGINT. Combine with `--tcp-midi-port`
for remote control and `--capture-audio-plus-fft-rms` for audio analysis.

---

## Automated Note Test (`test/test_notes.py`)

Python script that tests all four engines headlessly:

```bash
python3 test/test_notes.py
```

For each engine, the script:
1. Launches `virtual_synth` with `--synthengine`, `--tcp-midi-port`, and `--capture-audio-plus-fft-rms`
2. Sends MIDI note_on for C2 (36), C3 (48), C4 (60), C5 (72) in sequence via TCP
3. Captures audio analysis (per-second FFT + RMS)
4. Verifies each detected frequency is within ±1 semitone of expected
5. Reports pass/fail per note per engine

Expected frequencies:
```
C2 = 65.41 Hz   (MIDI 36)
C3 = 130.81 Hz  (MIDI 48)
C4 = 261.63 Hz  (MIDI 60)
C5 = 523.25 Hz  (MIDI 72)
```

Output:
```
============================================================
  Engine: ncursesynth
============================================================
    C2   [PASS] freq=65.4Hz err=0.01st rms=0.3823
    C3   [PASS] freq=130.8Hz err=0.00st rms=0.5321
    C4   [PASS] freq=261.6Hz err=0.00st rms=0.6102
    C5   [PASS] freq=523.3Hz err=0.00st rms=0.4224
...
============================================================
  Total: 16 passed, 0 failed
  Rate: 100.0%
============================================================
```

---

## Capture Audio + FFT/RMS Analysis

Records every audio sample to a raw float32 file and, on exit, writes a per-second analysis of RMS and dominant frequency (via FFT). Reuses `test/fft_analyzer.h/cpp` — no code duplication.

### Usage

```bash
./virtual_synth --capture-audio-plus-fft-rms tmp2/debug.raw
```

### Output

Two files:

- `tmp2/debug.raw` — raw float32 mono, 48kHz (importable in Audacity as raw 32-bit float)
- `tmp2/debug.raw.analysis.txt` — one line per second:

```
t=0s  fft=--  midi=--  rms=0.000000
t=1s  fft=--  midi=--  rms=0.279822
t=2s  fft=785.16Hz  midi=79.0  rms=0.268776
```

Fields: `t` = second index, `fft` = detected fundamental frequency, `midi` = corresponding MIDI note number, `rms` = root-mean-square amplitude.

### Implementation

`CaptureAnalyzer` is a singleton set via `--capture-audio-plus-fft-rms`. Both `AudioEngine` (PortAudio callback) and `AudioCaptureDriver` (TCP capture loop) call `CaptureAnalyzer::writeSample()` after generating each sample. On exit, `finalize()` flushes any remaining buffer and writes the analysis file.

---

## MIDI Capture

Logs MIDI note on/off and CC events with timestamps. Intercepts from both TCP MIDI (`TcpMidiServer`) and hardware MIDI (`MidiInput`).

```bash
./virtual_synth --capture-midi-plus-analysis tmp2/midi
```

On exit, writes `<file>.midi.txt` and displays on terminal:

```
Shutting down...
MIDI log written to tmp2/midi.midi.txt (3 events)
  t=0.023s  note_on  60 100
  t=1.015s  note_off 60
  t=1.523s  cc       29 80
```

---

## LVL Meter (UI only)

When running in UI mode (without `--synthengine`), a real-time level meter is displayed below the MIDI monitor:

```
MIDI: Waiting for input...
LVL [########--------]  50%  peak
```

The bar updates every frame (50ms). Peak responds instantly and decays slowly (exponential moving average at 0.9995 per sample). The line uses `A_REVERSE` when level exceeds 85%, indicating the soft knee limiter is active.

The level is measured after the soft knee limiter and `/8192` scaling, so it reflects the actual signal being sent to the audio output.
