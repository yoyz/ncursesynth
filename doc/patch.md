# Factory Patch Banks (FACTORYPATCH menu)

How the built-in factory patch banks work, and how to add one to a new engine.

## Overview

Some engines ship with a built-in patch/preset bank compiled into the binary:

| Engine | Bank | Count | Source |
|--------|------|-------|--------|
| Ambika | `machine/Ambika/patches.h/.cpp` | 208 | original Ambika softsynth (`patches.h/.cc`) |
| Twytch (Helm) | `machine/Twytch/twytch_factory_patches.h/.cpp` | 274 | mtytel/helm `patches/Factory Presets` (`.helm` JSON), converted by `machine/Twytch/tools/convert_helm_presets.py` |

The bank is browsed from the UI via a **FACTORYPATCH** menu item (added right
after **PRESET** in the menu bar). LEFT/RIGHT steps through the patches; the
engine loads each one and the control widgets refresh to reflect its
parameters.

## The base-class API

`machine/Machine.h` declares a generic factory-patch interface. Every engine
inherits it; the defaults are "no factory patches":

```cpp
virtual bool hasFactoryPatches() const { return false; }
virtual int getFactoryPatchCount() const { return 0; }
virtual void loadFactoryPatch(int index) { (void)index; }
virtual int getFactoryPatchIndex() const { return -1; }
virtual const char* getFactoryPatchName(int index) const { (void)index; return ""; }
```

The generic UI (`ui/machine_ui.cpp`) is engine-agnostic:

- `menuItemCount()` returns 5 when `machine->hasFactoryPatches()` is true,
  otherwise 4 (ENGINE / MIDI / MAPPING / PRESET / FACTORYPATCH).
- LEFT/RIGHT on the FACTORYPATCH menu item calls `machine->loadFactoryPatch(i)`
  then `updateControlValues()`, so the widgets reflect the loaded patch
  (`updateControlValues()` reads `getI()` for every widget).
- `clampMenuIndex()` keeps the selected menu item in range when switching
  between engines that do and do not support factory patches.

So to add factory patches to a new engine you only need to:

1. Embed the bank as C++ data.
2. Override the API in your `*Machine` class.
3. Make `loadFactoryPatch()` apply the data to the engine.
4. Make `getI()` (for the UI-exposed controls) reflect the loaded patch —
   either read live from the engine or sync your cached members after loading.

## Example 1: Ambika (patches already in engine format)

The Ambika's 208 patches are already `ambika::Patch` structs (the original
softsynth data). The engine's `PluginVoiceManager` has a `LoadPreset(int)`
that copies a patch into every voice and re-inits the envelopes.

`machine/Ambika/AmbikaMachine.cpp`:

```cpp
bool hasFactoryPatches() const override { return true; }
int  getFactoryPatchCount() const override { return m_vm->numPatches(); }
void loadFactoryPatch(int index) override { m_vm->LoadPreset(index); }
int  getFactoryPatchIndex() const override { return m_vm->current_preset_; }
const char* getFactoryPatchName(int index) const override {
  return m_vm->patchName(index);
}
```

`numPatches()` / `patchName()` were added as accessors over `kNumPatches` /
`kPatchNames` in `machine/Ambika/plugin_shared.h`.

Because Ambika's `getI()` reads the engine's live parameter state, the UI
widgets automatically reflect each loaded patch — no extra work needed.

## Example 2: Twytch (Helm, external JSON format)

The Helm factory presets are `.helm` files (JSON): a `settings` dict of
control name → native-unit value, plus a `modulations` array.

### Step 1 — convert to C++

`machine/Twytch/tools/convert_helm_presets.py` reads a directory of `.helm`
files and generates `twytch_factory_patches.h/.cpp`:

```cpp
namespace twytch {
  struct FactorySetting   { const char* name; float value; };
  struct FactoryModulation{ const char* source; const char* destination; float amount; };
  struct FactoryPatch {
    const char* name; const char* folder; const char* author;
    const FactorySetting* settings; int num_settings;
    const FactoryModulation* modulations; int num_modulations;
  };
  extern const FactoryPatch kFactoryPatches[274];
  extern const char* const kFactoryPatchNames[274];
  extern const int kNumFactoryPatches;
}
```

Run it like this (source dir = the Helm repo checkout):

```bash
python3 machine/Twytch/tools/convert_helm_presets.py \
  "/path/to/helm/patches/Factory Presets" \
  machine/Twytch
```

### Step 2 — apply settings at load time

`TwytchsynthMachine::loadFactoryPatch(index)`:

1. `clearFactoryConnections()` — disconnect + delete the previous patch's
   modulation connections (they are owned by the machine).
2. Apply every setting in native units:
   ```cpp
   auto controls = engine->getControls();
   for (i in patch.settings)
     if (controls.count(name)) controls.at(name)->set(value);
   ```
3. Rebuild the modulation routing via
   `engine->connectModulation(new ModulationConnection(src, dst))` with the
   amount set, skipping any whose source/destination the engine lacks.
4. `factoryIndex = index;` then `syncUiFromEngine()`.

### Step 3 — reflect parameters in the UI

`TwytchsynthMachine::getI()` returns cached member variables (0–127 / discrete
indices), so after loading a patch the members are re-derived from the engine
controls by `syncUiFromEngine()` (reverse conversions of the same mappings
`setI()` uses). Controls the UI does not expose are applied but not shown.

### Makefile

Add the generated `.cpp` to the engine's sources (here:
`ENGINE_TWYTCH_MACHINE`), so it builds into both the app and `test_runner`.

## Known limitations

- **Twytch modulations**: only **monophonic** modulation sources (mono LFOs,
  step sequencer) are routed. Polyphonic sources (mod/filter envelope, poly
  LFO) and mono→poly pairings crash the shared modulation `cr::Multiply` with
  a `-O2`-only UB (clean under ASan), so they are skipped. See the comment in
  `TwytchsynthMachine::loadFactoryPatch()`.
- **Twytch arpeggiator**: patches with `amp_sustain = 0` and `arp_on = 1`
  stay silent because the arpeggiator is not functional in this port.
- **Ambika**: no known limitations — all 208 patches load and the UI reflects
  them via live `getI()`.

## Checklist for a new engine

1. Embed the bank as C++ data (write a converter script if the source format
   is JSON/XML/hex — see `convert_helm_presets.py` for a template).
2. Add the generated source to the Makefile engine sources.
3. Override `hasFactoryPatches` / `getFactoryPatchCount` / `loadFactoryPatch` /
   `getFactoryPatchIndex` / `getFactoryPatchName`.
4. In `loadFactoryPatch`, apply the data to the engine, and make `getI()`
   reflect the loaded patch (live readback or member sync).
5. Ensure connection/state cleanup happens on reload, `init()`, and in the
   destructor.
6. Add a factory-patch section to AGENTS.md (see the Ambika/Twytch entries
   under "Menu System").
7. Verify: load every patch, play a note, confirm audio + no crashes; run
   `./test_ui` and `./test_runner --all-engines --all-tests`.
