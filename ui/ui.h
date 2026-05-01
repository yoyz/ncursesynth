#ifndef UI_H
#define UI_H

#include <atomic>
#include <thread>
#include "../synth/synth_architecture.h"
#include "ui_parameters.h"

enum class UIMode {
    NORMAL,
    PRESET_BROWSER,
    PRESET_SAVE,
    HELP
};

class UI {
private:
    SynthArchitecture* synth;
    std::atomic<bool> running;
    std::thread uiThread;
    Parameter selectedParameter;
    UIMode mode;
    int browserSelectedIndex;
    std::string savePresetName;
    
    void run();
    void handleBrowserInput(int ch);
    
public:
    UI(SynthArchitecture* synthArch);
    ~UI();
    
    void start();
    void stop();
    bool isRunning() const { return running.load(); }
};

#endif
