#ifndef AMBIKA_MACHINE_H
#define AMBIKA_MACHINE_H

#include "../Machine.h"
#include <cstdint>

class PluginVoiceManager;

class AmbikaMachine : public Machine {
public:
    AmbikaMachine();
    ~AmbikaMachine();

    void init() override;
    void reset() override;
    int32_t tick() override;
    void noteOn() override;
    void noteOff() override;
    void setI(int index, int value) override;
    void setF(int index, float value) override;
    int getI(int index) override;
    float getF(int index) override;
    void applyCC(int cc, float normalized, const std::string& paramName) override;
    const char* getDisplayString(int index) override;
    std::vector<std::pair<std::string, int>> getPresetParams() const override;

    int getKeyOn() const { return m_hasActiveNotes; }
    int getLastNote() const { return m_lastNote; }

private:
    PluginVoiceManager* m_vm;
    int m_lastNote;
    bool m_hasActiveNotes;

    float m_resampleBuf[40];
    float m_resamplePos;
    float m_resampleStep;

    // Local cache for discrete params to avoid lossy roundtrips
    int m_osc1Shape;
    int m_osc2Shape;
    int m_filterMode;
    int m_filterType;
    int m_mixOp;
    int m_lfoShape;
    int m_fltEnvDepth;

    static int mapParam(int id);
    void processBlock();
};

#endif
