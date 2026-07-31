#include "machine_ui.h"
#include "ui_layout.h"
#include "../machine/ParamID.h"
#include "../midi/midi_input.h"
#include "../midi/midi_mapping.h"
#include "../audio/audio_level.h"
#include <cmath>
#include <cstring>
#include <cctype>
#include <chrono>

// AZERTY → chromatic note mapping (one octave, C4 = MIDI 60)
static const char PC_NOTE_KEYS[] = {
    'a', 'z', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '^', '$'
};
static const int PC_NOTE_COUNT = 12;

MachineUI::MachineUI(Machine* mach, MachineManager* mgr)
    : selectedControl(0), machine(mach), machineManager(mgr),
      midiInput(nullptr), renderer(nullptr), screenRows(0), screenCols(0),
      lastMidiNote(-1), lastMidiVel(0), midiActivity(false),
      pcKeyboardMode(false), pcOctave(4),
      menuSelection(0), menuIndex(0), midiDeviceIndex(-1), mappingIndex(0),
      presetIndex(0), factoryIndex(0), presetInputMode(false), statusTimer(0),
      masterEffects(nullptr), fxView(false), fxSelection(0), fxAddType(0) {
    columnTitles[0] = "OSCILLATORS";
    columnTitles[1] = "FILTER";
    columnTitles[2] = "ENVELOPE";
    scanPresets();
}

MachineUI::~MachineUI() {
}

void MachineUI::init() {
    initControls();
    if (renderer) {
        renderer->getSize(screenRows, screenCols);
    }

    if (machine) {
        machine->lock();
        for (const auto& w : widgets) {
            machine->setI(w.paramId, w.value);
        }
        machine->unlock();
        updateControlValues();
    }
}

void MachineUI::drawColumnHeader(int col, const char* title) {
    if (!renderer) return;
    renderer->setStyle(Style::BOLD);
    renderer->write(HEADER_ROW, col, title);
    renderer->setStyle(Style::NORMAL);
}

void MachineUI::updateControlValues() {
    if (!machine) return;

    machine->lock();
    for (auto& w : widgets) {
        int raw = machine->getI(w.paramId);
        if (raw >= 0 && raw <= 128) {
            w.value = (raw > 127) ? 127 : raw;
        }
    }
    machine->unlock();

    midiActivity = false;
}

void MachineUI::draw() {
    if (!renderer) return;
    renderer->clear();
    renderer->setStyle(Style::NORMAL);

    renderer->getSize(screenRows, screenCols);

    drawMenuBar();

    std::string engineName = machine ? machine->getName() : "SYNTH";
    renderer->setStyle(Style::BOLD);
    renderer->write(4, 2, engineName + " ENGINE");
    renderer->setStyle(Style::NORMAL);

    if (fxView) {
        drawFxView();
    } else {
        drawColumnHeader(2, columnTitles[0].c_str());
        drawColumnHeader(40, columnTitles[1].c_str());
        drawColumnHeader(78, columnTitles[2].c_str());

        for (size_t i = 0; i < widgets.size(); i++) {
            widgets[i].draw(*renderer, (int)i == selectedControl, machine);
        }
    }

    if (statusTimer > 0) {
        statusTimer--;
        renderer->setStyle(Style::BOLD);
        renderer->write(screenRows - 1, 2, statusMessage);
        renderer->setStyle(Style::NORMAL);
    }

    renderer->setStyle(Style::DIM);
    if (presetInputMode) {
        renderer->write(screenRows - 2, 2, "Enter preset name: " + presetInputBuffer + "_");
        renderer->write(screenRows - 3, 2, "Press ENTER to save, ESC to cancel");
    } else if (fxView) {
        renderer->write(screenRows - 3, 2,
            "F: Params | UP/DOWN: Select | L/R or PGUP/DN: Adjust | A: Add | X: Remove | [/]: Move");
    } else if (menuSelection == 0) {
        if (pcKeyboardMode) {
            char buf[80];
            snprintf(buf, sizeof(buf),
                "a-z=keys | Z/X:Octave(%d) | ARROWS:Params | 1-9:Set | S:Save | Q:Quit", pcOctave);
            renderer->write(screenRows - 3, 2, buf);
        } else {
            renderer->write(screenRows - 3, 2,
                "TAB: Menu | ARROWS: Navigate | 1-9: Set | PGUP/DN: Adjust | S:Save | C:Create | Q:Quit");
        }
    } else if (menuSelection == 1) {
        renderer->write(screenRows - 3, 2,
            "LEFT/RIGHT: Switch | TAB: Params | Q: Quit");
    }
    renderer->setStyle(Style::NORMAL);

    drawMidiMonitor();
    drawLevelMeter();

    renderer->refresh();
}

int MachineUI::menuItemCount() const {
    return (machine && machine->hasFactoryPatches()) ? 5 : 4;
}

void MachineUI::clampMenuIndex() {
    int maxIdx = menuItemCount() - 1;
    if (menuIndex < 0) menuIndex = 0;
    if (menuIndex > maxIdx) menuIndex = maxIdx;
}

void MachineUI::drawMenuBar() {
    if (!renderer) return;
    clampMenuIndex();
    int row = 2;
    int col = 2;

    renderer->setStyle(menuSelection == 1 && menuIndex == 0 ? Style::REVERSE : Style::NORMAL);
    renderer->write(row, col, "[ENGINE: " + (machine ? machine->getName() : "None") + "]");
    renderer->setStyle(Style::NORMAL);

    std::string midiName = "None";
    if (midiInput && midiDeviceIndex >= 0) {
        int dc = midiInput->getDeviceCount();
        if (midiDeviceIndex < dc) {
            std::string n = midiInput->getDeviceName(midiDeviceIndex);
            if (!n.empty()) {
                midiName = n;
                if (midiName.length() > 16) midiName = midiName.substr(0, 13) + "...";
            }
        }
    }
    renderer->setStyle(menuSelection == 1 && menuIndex == 1 ? Style::REVERSE : Style::NORMAL);
    renderer->write(row, col + 18, "[MIDI: " + midiName + "]");
    renderer->setStyle(Style::NORMAL);

    std::string mapName = "None";
    if (midiInput) {
        auto* mm = midiInput->getMappingManager();
        if (mm && mm->getMappingCount() > 0) {
            mapName = mm->getMappingName(mm->getCurrentMappingIndex());
            if (mapName.length() > 16) mapName = mapName.substr(0, 13) + "...";
        }
    }
    renderer->setStyle(menuSelection == 1 && menuIndex == 2 ? Style::REVERSE : Style::NORMAL);
    renderer->write(row, col + 38, "[MAPPING: " + mapName + "]");
    renderer->setStyle(Style::NORMAL);

    std::string presetName = "Init";
    if (!presets.empty() && presetIndex >= 0 && presetIndex < (int)presets.size())
        presetName = presets[presetIndex].name;
    if (presetName.length() > 16) presetName = presetName.substr(0, 13) + "...";
    renderer->setStyle(menuSelection == 1 && menuIndex == 3 ? Style::REVERSE : Style::NORMAL);
    renderer->write(row, col + 60, "[PRESET: " + presetName + "]");
    renderer->setStyle(Style::NORMAL);

    bool hasFactory = machine && machine->hasFactoryPatches();
    if (hasFactory) {
        std::string factoryName = "Init";
        if (machine->getFactoryPatchIndex() >= 0)
            factoryName = machine->getFactoryPatchName(machine->getFactoryPatchIndex());
        if (factoryName.length() > 13) factoryName = factoryName.substr(0, 10) + "...";
        renderer->setStyle(menuSelection == 1 && menuIndex == 4 ? Style::REVERSE : Style::NORMAL);
        renderer->write(row, col + 80, "[FACTORYPATCH: " + factoryName + "]");
        renderer->setStyle(Style::NORMAL);
    }

    int hintCol = hasFactory ? col + 104 : col + 82;
    if (!hasFactory || screenCols >= hintCol + 25) {
        renderer->setStyle(Style::DIM);
        if (menuSelection == 0)
            renderer->write(row, hintCol, "(TAB: Menu | ARROWS: Navigate | S: Save)");
        else
            renderer->write(row, hintCol, "(LEFT/RIGHT: Switch | TAB: Params)");
        renderer->setStyle(Style::NORMAL);
    }
}

void MachineUI::drawMidiMonitor() {
    if (!renderer) return;
    int row = screenRows - 5;

    bool hasCC = false;
    int lastCC = -1;
    int lastCCValue = 0;
    int elapsed = 0;

    if (midiInput) {
        auto now = std::chrono::steady_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - midiInput->getLastActivity()).count();
        lastCC = midiInput->getLastCC();
        lastCCValue = midiInput->getLastCCValue();
        hasCC = (elapsed < 2000 && lastCC >= 0 && lastCC < 128);
    }

    if (hasCC) {
        renderer->setStyle(Style::BOLD);

        char buf[64];
        snprintf(buf, sizeof(buf), "MIDI: CC%3d = %3d   ", lastCC, lastCCValue);
        renderer->write(row, 2, buf);
        renderer->setStyle(Style::NORMAL);

        if (screenCols > 20) {
            int clearWidth = screenCols - 20;
            char clear[128];
            int w = clearWidth < 128 ? clearWidth : 127;
            memset(clear, ' ', w);
            clear[w] = '\0';
            renderer->write(row, 20, clear);
        }
    } else if (midiActivity) {
        renderer->setStyle(Style::BOLD);

        char buf[64];
        snprintf(buf, sizeof(buf), "MIDI: Note=%d Vel=%d  ", lastMidiNote, lastMidiVel);
        renderer->write(row, 2, buf);
        renderer->setStyle(Style::NORMAL);

        if (screenCols > 25) {
            int clearWidth = screenCols - 25;
            char clear[128];
            int w = clearWidth < 128 ? clearWidth : 127;
            memset(clear, ' ', w);
            clear[w] = '\0';
            renderer->write(row, 25, clear);
        }
    } else {
        renderer->setStyle(Style::DIM);
        renderer->write(row, 2, "MIDI: Waiting for input...");
        renderer->setStyle(Style::NORMAL);
    }
}

void MachineUI::drawLevelMeter() {
    if (!renderer) return;
    int lvlRow = screenRows - 4;
    float peak = AudioLevel::getPeak();
    int barLen = (int)(peak * 16.0f);
    if (barLen < 0) barLen = 0;
    if (barLen > 16) barLen = 16;

    renderer->setStyle((peak > 0.85f) ? Style::REVERSE : Style::DIM);
    renderer->write(lvlRow, 2, "LVL [");
    renderer->drawBar(lvlRow, 7, barLen, 16, 16);

    char pctStr[32];
    snprintf(pctStr, sizeof(pctStr), "] %3d%%  peak", (int)(peak * 100));
    renderer->write(lvlRow, 23, pctStr);
    renderer->setStyle(Style::NORMAL);
}

int MachineUI::fxRowCount() const {
    if (!masterEffects) return 0;
    int n = 0;
    for (int e = 0; e < masterEffects->getEffectCount(); e++) {
        n += 1 + masterEffects->getEffectParamCount(e);
    }
    return n;
}

bool MachineUI::fxRowAt(int flat, int& effectIdx, int& paramIdx) const {
    if (!masterEffects) return false;
    int idx = flat;
    for (int e = 0; e < masterEffects->getEffectCount(); e++) {
        int pcount = masterEffects->getEffectParamCount(e);
        if (idx == 0) { effectIdx = e; paramIdx = -1; return true; }
        if (idx <= pcount) { effectIdx = e; paramIdx = idx - 1; return true; }
        idx -= pcount + 1;
    }
    return false;
}

int MachineUI::fxRowOf(int effectIdx, int paramIdx) const {
    int flat = 0;
    for (int e = 0; e < effectIdx; e++) {
        flat += 1 + masterEffects->getEffectParamCount(e);
    }
    return flat + (paramIdx < 0 ? 0 : paramIdx + 1);
}

void MachineUI::drawFxView() {
    if (!renderer || !masterEffects) return;

    renderer->setStyle(Style::BOLD);
    renderer->write(4, 18, "MASTER FX  (shared, post-mix)");
    renderer->setStyle(Style::NORMAL);

    constexpr int NAME_WIDTH = 11;
    int row = CONTROL_ROW_OFFSET;
    int col = 2;
    int lastRow = screenRows - 6;

    for (int e = 0; e < masterEffects->getEffectCount(); e++) {
        if (row > lastRow) break;
        int headerFlat = fxRowOf(e, -1);
        bool selected = (headerFlat == fxSelection);

        char buf[48];
        snprintf(buf, sizeof(buf), "[%d] %-*s", e + 1, NAME_WIDTH - 3,
                 masterEffects->getEffectName(e));
        if (selected) renderer->setStyle(Style::REVERSE);
        renderer->write(row, col, buf);
        renderer->setStyle(selected ? Style::NORMAL : Style::DIM);
        renderer->write(row, col + 11, masterEffects->isEffectEnabled(e) ? "[ON]" : "[OFF]");
        renderer->setStyle(Style::NORMAL);

        row++;

        int pcount = masterEffects->getEffectParamCount(e);
        for (int p = 0; p < pcount; p++) {
            if (row > lastRow) break;
            int flat = fxRowOf(e, p);
            bool psel = (flat == fxSelection);

            char nameBuf[16];
            snprintf(nameBuf, sizeof(nameBuf), "%-*s", NAME_WIDTH,
                     masterEffects->getEffectParamName(e, p));
            float norm = masterEffects->getEffectParam(e, p);
            int raw = (int)(norm * 127.0f + 0.5f);
            if (raw > 127) raw = 127;
            if (raw < 0) raw = 0;

            if (psel) renderer->setStyle(Style::REVERSE);
            renderer->write(row, col + 3, nameBuf);
            renderer->write(row, col + 3 + NAME_WIDTH + 1, "[");
            renderer->drawBar(row, col + 3 + NAME_WIDTH + 2, raw, 127, CONTROL_BAR_LEN);
            renderer->write(row, col + 3 + NAME_WIDTH + 2 + CONTROL_BAR_LEN, "]");
            int pct = (raw * 100 + 63) / 127;
            char pctStr[16];
            snprintf(pctStr, sizeof(pctStr), "%3d%%", pct);
            renderer->write(row, col + 3 + NAME_WIDTH + 2 + CONTROL_BAR_LEN + 2, pctStr);
            renderer->setStyle(psel ? Style::NORMAL : Style::NORMAL);

            row++;
        }
    }

    if (masterEffects->getEffectCount() == 0) {
        renderer->setStyle(Style::DIM);
        renderer->write(row, col, "Chain empty - press A to add an effect");
        renderer->setStyle(Style::NORMAL);
    }
}

void MachineUI::handleFxInput(int ch) {
    if (ch == 'f' || ch == 'F') {
        fxView = false;
        return;
    }
    if (!masterEffects) return;

    int total = fxRowCount();
    if (total == 0) {
        if (ch == 'a' || ch == 'A') {
            masterEffects->addEffect((MasterEffects::EffectType)fxAddType);
            fxAddType = (fxAddType + 1) % 4;
            fxSelection = 0;
        }
        return;
    }

    if (ch == Key::UP) { fxSelection = (fxSelection - 1 + total) % total; return; }
    if (ch == Key::DOWN) { fxSelection = (fxSelection + 1) % total; return; }

    int effectIdx = 0, paramIdx = 0;
    if (!fxRowAt(fxSelection, effectIdx, paramIdx)) return;

    if (ch == 'a' || ch == 'A') {
        if (masterEffects->insertEffect(effectIdx, (MasterEffects::EffectType)fxAddType)) {
            fxAddType = (fxAddType + 1) % 4;
            fxSelection = fxRowOf(effectIdx + 1, -1);
        }
        return;
    }

    if (paramIdx < 0) {
        // Effect header row
        if (ch == Key::LEFT || ch == Key::RIGHT || ch == Key::PAGE_UP || ch == Key::PAGE_DOWN) {
            masterEffects->setEffectEnabled(effectIdx, !masterEffects->isEffectEnabled(effectIdx));
        } else if (ch == 'x' || ch == 'X') {
            masterEffects->removeEffect(effectIdx);
            if (fxSelection >= fxRowCount()) fxSelection = fxRowCount() - 1;
        } else if (ch == '[') {
            masterEffects->moveEffect(effectIdx, effectIdx - 1);
            fxSelection = fxRowOf(effectIdx - 1, -1);
        } else if (ch == ']') {
            masterEffects->moveEffect(effectIdx, effectIdx + 1);
            fxSelection = fxRowOf(effectIdx + 1, -1);
        }
        return;
    }

    // Parameter row
    int raw = (int)(masterEffects->getEffectParam(effectIdx, paramIdx) * 127.0f + 0.5f);
    if (ch == Key::LEFT || ch == Key::PAGE_DOWN) raw--;
    else if (ch == Key::RIGHT || ch == Key::PAGE_UP) raw++;
    else if (ch >= '0' && ch <= '9') raw = (int)((ch - '0') / 10.0f * 127.0f + 0.5f);
    else return;
    if (raw < 0) raw = 0;
    if (raw > 127) raw = 127;
    masterEffects->setEffectParam(effectIdx, paramIdx, raw / 127.0f);
}

void MachineUI::handleInput(int ch) {
    if (presetInputMode) {
        if (ch == '\n' || ch == Key::ENTER) {
            if (!presetInputBuffer.empty()) {
                savePreset(presetInputBuffer);
            }
            presetInputMode = false;
            presetInputBuffer.clear();
        } else if (ch == 27 || ch == '\t') {
            presetInputMode = false;
            presetInputBuffer.clear();
        } else if (ch == Key::BACKSPACE || ch == 127 || ch == 8) {
            if (!presetInputBuffer.empty())
                presetInputBuffer.pop_back();
        } else if (ch >= 32 && ch <= 126) {
            if (presetInputBuffer.length() < 40)
                presetInputBuffer += (char)ch;
        }
        return;
    }

    // Master-FX view (shared across all engines)
    if (fxView) {
        handleFxInput(ch);
        return;
    }

    // PC keyboard note playing (only in parameter mode)
    if (pcKeyboardMode && menuSelection == 0 && machine) {
        if (ch == 'Z') { pcOctave = (pcOctave > 0) ? pcOctave - 1 : 0; return; }
        if (ch == 'X') { pcOctave = (pcOctave < 9) ? pcOctave + 1 : 9; return; }
        for (int i = 0; i < PC_NOTE_COUNT; i++) {
            if (ch == PC_NOTE_KEYS[i]) {
                int midiNote = (pcOctave * 12) + i;
                machine->lock();
                machine->setI(ParamID::note, midiNote);
                if (pcActiveKeys[ch]) {
                    pcActiveKeys[ch] = false;
                    machine->setI(ParamID::note_on, 0);
                } else {
                    pcActiveKeys[ch] = true;
                    machine->setI(ParamID::velocity, 100);
                    machine->setI(ParamID::note_on, 1);
                    setMidiNote(midiNote, 100);
                }
                machine->unlock();
                return;
            }
        }
    }

    if (ch == '\t') {
        menuSelection = (menuSelection == 0) ? 1 : 0;
        return;
    }

    if (menuSelection == 1) {
        if (ch == Key::UP) {
            int mc = menuItemCount();
            menuIndex = (menuIndex - 1 + mc) % mc;
            return;
        } else if (ch == Key::DOWN) {
            int mc = menuItemCount();
            menuIndex = (menuIndex + 1) % mc;
            return;
        }

        if (ch == Key::LEFT || ch == Key::RIGHT) {
            if (menuIndex == 0 && machineManager) {
                int mc = machineManager->getMachineCount();
                int curr = machineManager->getCurrentMachineIndex();
                if (mc > 0) {
                    if (ch == Key::LEFT) {
                        machineManager->setCurrentMachine((curr - 1 + mc) % mc);
                    } else {
                        machineManager->setCurrentMachine((curr + 1) % mc);
                    }
                }
            } else if (menuIndex == 1 && midiInput) {
                int dc = midiInput->getDeviceCount();
                if (dc > 0) {
                    if (ch == Key::LEFT) {
                        midiDeviceIndex = (midiDeviceIndex - 1 + dc) % dc;
                    } else {
                        midiDeviceIndex = (midiDeviceIndex + 1) % dc;
                    }
                    midiInput->selectDevice(midiDeviceIndex);
                    midiInput->setMappingMachine(machine);
                    if (!midiInput->isRunning()) {
                        midiInput->start();
                    }
                }
            } else if (menuIndex == 2 && midiInput) {
                auto* mm = midiInput->getMappingManager();
                if (mm && mm->getMappingCount() > 0) {
                    int mc = mm->getMappingCount();
                    if (ch == Key::LEFT) {
                        mm->setCurrentMapping((mm->getCurrentMappingIndex() - 1 + mc) % mc);
                    } else {
                        mm->setCurrentMapping((mm->getCurrentMappingIndex() + 1) % mc);
                    }
                }
            } else if (menuIndex == 3) {
                int pc = (int)presets.size();
                if (pc > 0) {
                    if (ch == Key::LEFT) {
                        int newIdx = (presetIndex - 1 + pc) % pc;
                        loadPreset(newIdx);
                    } else {
                        int newIdx = (presetIndex + 1) % pc;
                        loadPreset(newIdx);
                    }
                }
            } else if (menuIndex == 4 && machine && machine->hasFactoryPatches()) {
                int fc = machine->getFactoryPatchCount();
                if (fc > 0) {
                    if (ch == Key::LEFT) {
                        factoryIndex = (factoryIndex - 1 + fc) % fc;
                    } else {
                        factoryIndex = (factoryIndex + 1) % fc;
                    }
                    if (factoryIndex < 0) factoryIndex = 0;
                    machine->lock();
                    machine->loadFactoryPatch(factoryIndex);
                    machine->unlock();
                    updateControlValues();
                }
            }
        }
        return;
    }

    if (menuSelection == 0) {
        if (ch == 'f' || ch == 'F') {
            fxView = true;
            fxSelection = 0;
            return;
        }
        if (ch == 's' || ch == 'S') {
            std::string name = getCurrentPresetName();
            if (!name.empty() && name != "Init") {
                savePreset(name);
            }
            return;
        }
        if (ch == 'c' || ch == 'C') {
            presetInputMode = true;
            presetInputBuffer.clear();
            return;
        }
        handleNavigation(ch);
        if (ch >= '0' && ch <= '9') {
            float val = (ch - '0') / 10.0f;
            if (selectedControl >= 0 && selectedControl < (int)widgets.size()) {
                auto& w = widgets[selectedControl];
                if (w.type == WidgetType::DISCRETE) {
                    w.value = (int)(val * (w.discreteCount - 1) + 0.5f);
                } else {
                    w.value = (int)(val * 127 + 0.5f);
                }
                machine->lock();
                machine->setI(w.paramId, w.value);
                machine->unlock();
            }
        }
    }
}

void MachineUI::handleNavigation(int ch) {
    if (ch == Key::UP) {
        selectedControl = (selectedControl - 1 + (int)widgets.size()) % (int)widgets.size();
    } else if (ch == Key::DOWN) {
        selectedControl = (selectedControl + 1) % (int)widgets.size();
    } else if (ch == Key::LEFT || ch == Key::RIGHT) {
        if (selectedControl >= 0 && selectedControl < (int)widgets.size()) {
            widgets[selectedControl].onKey(ch, machine);
        }
    } else if (ch == Key::PAGE_UP) {
        if (selectedControl >= 0 && selectedControl < (int)widgets.size()) {
            auto& w = widgets[selectedControl];
            int& val = w.value;
            val += 13;
            int maxVal = (w.type == WidgetType::DISCRETE) ? w.discreteCount - 1 : 127;
            if (val > maxVal) val = maxVal;
            machine->lock();
            machine->setI(w.paramId, val);
            machine->unlock();
        }
    } else if (ch == Key::PAGE_DOWN) {
        if (selectedControl >= 0 && selectedControl < (int)widgets.size()) {
            auto& w = widgets[selectedControl];
            int& val = w.value;
            val -= 13;
            if (val < 0) val = 0;
            machine->lock();
            machine->setI(w.paramId, val);
            machine->unlock();
        }
    }
}

void MachineUI::updateValues() {
    updateControlValues();
}

void MachineUI::setControlValue(int paramId, float value) {
    for (auto& w : widgets) {
        if (w.paramId == paramId) {
            w.value = (int)(value * 127.0f);
            break;
        }
    }
}

MappingManager* MachineUI::getMappingManager() {
    return midiInput ? midiInput->getMappingManager() : nullptr;
}

static std::string engineDir(const std::string& name) {
    std::string r = "bank/";
    for (char c : name) r += std::tolower(c);
    return r;
}

void MachineUI::scanPresets() {
    presets.clear();
    if (!machine) return;

    PresetInfo initPreset;
    initPreset.name = "Init";
    initPreset.path = "";
    presets.push_back(initPreset);

    std::vector<std::string> files = Machine::getPresetList(machine->getName());
    for (const auto& f : files) {
        PresetInfo pi;
        pi.name = f;
        pi.path = engineDir(machine->getName()) + "/" + f;
        presets.push_back(pi);
    }

    if (presetIndex < 0) presetIndex = 0;
    if (presetIndex >= (int)presets.size())
        presetIndex = 0;
}

bool MachineUI::loadPreset(int index) {
    if (!machine) return false;
    if (index < 0 || index >= (int)presets.size()) return false;

    bool ok = true;
    if (index == 0 && presets[index].path.empty()) {
        machine->lock();
        machine->init();
        machine->unlock();
        updateControlValues();
        presetIndex = 0;
    } else {
        machine->lock();
        ok = machine->loadPreset(presets[index].path);
        machine->unlock();
        if (ok) {
            presetIndex = index;
            updateControlValues();
        }
    }
    return ok;
}

bool MachineUI::savePreset(const std::string& name) {
    if (!machine) return false;

    std::string path = engineDir(machine->getName()) + "/" + name;
    bool ok = machine->savePreset(path);
    if (ok) {
        statusMessage = "Preset '" + name + "' saved";
        statusTimer = 60;
        scanPresets();
        for (size_t i = 0; i < presets.size(); i++) {
            if (presets[i].name == name) {
                presetIndex = (int)i;
                break;
            }
        }
    } else {
        statusMessage = "Save failed - check permissions or disk space";
        statusTimer = 120;
    }
    return ok;
}

const std::string& MachineUI::getCurrentPresetName() const {
    if (presets.empty()) {
        static std::string init = "Init";
        return init;
    }
    if (presetIndex >= 0 && presetIndex < (int)presets.size()) {
        return presets[presetIndex].name;
    }
    static std::string init = "Init";
    return init;
}

void MachineUI::stop() {
    if (renderer) {
        renderer->shutdown();
    }
}
