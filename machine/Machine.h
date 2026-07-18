#ifndef MACHINE_H
#define MACHINE_H

#include <string>
#include <vector>
#include <utility>
#include <cstdint>
#include <mutex>

class Machine {
public:
    static constexpr int32_t SAMPLE_RATE = 48000;

    Machine() : name_(""), selected_(false) {}
    virtual ~Machine() {}

    // Thread safety: lock on write from MIDI/UI, try_lock on read from audio
    void lock() { mtx_.lock(); }
    void unlock() { mtx_.unlock(); }
    bool tryLock() { return mtx_.try_lock(); }

    virtual void init() {}
    virtual void reset() {}
    virtual int32_t tick() { return 0; }
    virtual void noteOn() {}
    virtual void noteOff() {}

    virtual void setI(int index, int value) { (void)index; (void)value; }
    virtual void setF(int index, float value) { (void)index; (void)value; }
    virtual int getI(int index) { (void)index; return 0; }
    virtual float getF(int index) { (void)index; return 0.0f; }

    virtual int checkI(int index, int value) { return value; }
    virtual const char* getDisplayString(int index) { (void)index; return ""; }

    virtual void applyCC(int cc, float normalized, const std::string& paramName) { (void)cc; (void)normalized; (void)paramName; }

    void setName(const std::string& name) { name_ = name; }
    const std::string& getName() const { return name_; }

    void select() { selected_ = true; }
    void deselect() { selected_ = false; }
    bool isSelected() const { return selected_; }

    void setMidiDebug(bool debug) { midiDebug_ = debug; }
    bool getMidiDebug() const { return midiDebug_; }

    // Preset system
    virtual std::vector<std::pair<std::string, int>> getPresetParams() const;
    virtual bool loadPreset(const std::string& path);
    virtual bool savePreset(const std::string& path);
    static std::vector<std::string> getPresetList(const std::string& engineName);

protected:
    std::mutex mtx_;
    std::string name_;
    bool selected_;
    bool midiDebug_ = false;
};

#endif