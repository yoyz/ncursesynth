#ifndef DIGITS_MACHINE_H
#define DIGITS_MACHINE_H

#include "../Machine.h"
#include "DigitsEngine.h"

class DigitsMachine : public Machine {
public:
    DigitsMachine();
    ~DigitsMachine();

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

    int getKeyOn() const { return m_engine.getKeyOn(); }
    int getLastNote() const { return m_engine.getLastNote(); }

private:
    DigitsEngine m_engine;
    int m_bufferIndex;
};

#endif
