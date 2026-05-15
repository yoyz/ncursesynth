#!/usr/bin/env python3
"""
Headless note test: plays C2 C3 C4 C5 on all synth engines via TCP MIDI,
captures audio + FFT/RMS analysis, and verifies each note's frequency.
"""

import subprocess
import socket
import time
import os
import sys
import math
import signal

BASE_PORT = 9500
CAPTURE_DIR = "tmp2"

ENGINES = ["ncursesynth", "pbsynth", "cursynth", "twytch"]

NOTES = [
    (36, "C2", 65.41),
    (48, "C3", 130.81),
    (60, "C4", 261.63),
    (72, "C5", 523.25),
]

NOTE_ON_DELAY = 2.0
NOTE_OFF_DELAY = 1.0
STARTUP_DELAY = 3.0
SEMITONE_TOLERANCE = 1.0

total_passed = 0
total_failed = 0

def midi_note_to_freq(note):
    return 440.0 * (2.0 ** ((note - 69) / 12.0))

def freq_to_midi(freq):
    if freq <= 0:
        return -1
    return 69 + 12 * math.log2(freq / 440.0)

def semitone_error(detected_hz, expected_hz):
    if detected_hz <= 0 or expected_hz <= 0:
        return 999
    return abs(12 * math.log2(detected_hz / expected_hz))

def send_tcp(host, port, msg):
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(3.0)
        s.connect((host, port))
        s.sendall((msg + "\n").encode())
        s.close()
        return True
    except Exception as e:
        print(f"      TCP send failed: {e}")
        return False

def run_test():
    global total_passed, total_failed
    os.makedirs(CAPTURE_DIR, exist_ok=True)

    for engine in ENGINES:
        print(f"\n{'='*60}")
        print(f"  Engine: {engine}")
        print(f"{'='*60}")

        port = BASE_PORT + ENGINES.index(engine)
        capture_path = os.path.join(CAPTURE_DIR, f"test_notes_{engine}.raw")
        analysis_path = capture_path + ".analysis.txt"

        if os.path.exists(capture_path):
            os.remove(capture_path)
        if os.path.exists(analysis_path):
            os.remove(analysis_path)

        proc = subprocess.Popen(
            ["./virtual_synth",
             "--synthengine", engine,
             "--tcp-midi-port", str(port),
             "--capture-audio-plus-fft-rms", capture_path],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL
        )

        time.sleep(STARTUP_DELAY)

        engine_ok = True
        note_results = []

        for note_num, note_name, expected_freq in NOTES:
            print(f"    {note_name} (MIDI {note_num}, exp={expected_freq:.1f}Hz)...")

            if not send_tcp("127.0.0.1", port,
                            f'{{"type":"note_on","note":{note_num},"velocity":100}}'):
                note_results.append((note_name, 0, 0, False, "TCP send failed"))
                engine_ok = False
                break

            time.sleep(NOTE_ON_DELAY)

            send_tcp("127.0.0.1", port,
                     f'{{"type":"note_off","note":{note_num}}}')
            time.sleep(NOTE_OFF_DELAY)

        proc.terminate()
        try:
            proc.wait(timeout=5)
        except:
            proc.kill()
            proc.wait()

        if not os.path.exists(analysis_path):
            print(f"    [FAIL] No analysis file generated")
            total_failed += 1
            continue

        with open(analysis_path) as f:
            lines = f.readlines()

        results = []
        for line in lines:
            line = line.strip()
            if not line:
                continue
            try:
                parts = line.split()
                t = int(parts[0].replace("t=", "").replace("s", ""))
                fft_str = parts[1].replace("fft=", "").replace("Hz", "")
                rms = float(parts[3].replace("rms=", ""))
                freq = float(fft_str) if fft_str != "--" else 0.0
                midi_val = float(parts[2].replace("midi=", "")) if parts[2].replace("midi=", "") != "--" else -1.0
                results.append((t, freq, midi_val, rms))
            except (IndexError, ValueError):
                continue

        if not results:
            print(f"    [FAIL] Could not parse analysis")
            total_failed += 1
            continue

        for i, (note_num, note_name, expected_freq) in enumerate(NOTES):
            center_second = int(STARTUP_DELAY + i * (NOTE_ON_DELAY + NOTE_OFF_DELAY) + NOTE_ON_DELAY * 0.5)

            best = None
            for t, freq, midi_val, rms in results:
                if abs(t - center_second) <= 1 and rms > 0.01:
                    error = semitone_error(freq, expected_freq)
                    if best is None or error < best[0]:
                        best = (error, freq, rms)

            if best is None:
                print(f"    [{note_name:4s}] [FAIL] No valid audio for this note")
                total_failed += 1
                engine_ok = False
            else:
                error, freq, rms = best
                if error <= SEMITONE_TOLERANCE and rms > 0.01:
                    print(f"    [{note_name:4s}] [PASS] freq={freq:.1f}Hz err={error:.2f}st rms={rms:.4f}")
                    total_passed += 1
                else:
                    status = "tone" if rms > 0.01 else "silence"
                    print(f"    [{note_name:4s}] [FAIL] freq={freq:.1f}Hz err={error:.2f}st rms={rms:.4f} ({status})")
                    total_failed += 1
                    engine_ok = False

        if os.path.exists(capture_path):
            os.remove(capture_path)
        if os.path.exists(analysis_path):
            os.remove(analysis_path)

def main():
    print("ncursesynth Headless Note Test")
    print("Plays C2 C3 C4 C5 on each engine via TCP MIDI + FFT/RMS verification\n")

    try:
        run_test()
    except KeyboardInterrupt:
        print("\nInterrupted")

    print(f"\n{'='*60}")
    print(f"  Total: {total_passed} passed, {total_failed} failed")
    rate = (total_passed / (total_passed + total_failed) * 100) if (total_passed + total_failed) > 0 else 0
    print(f"  Rate: {rate:.1f}%")
    print(f"{'='*60}")

    return 0 if total_failed == 0 else 1

if __name__ == "__main__":
    sys.exit(main())
