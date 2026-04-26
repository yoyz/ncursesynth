#ifndef UI_H
#define UI_H

#include <atomic>
#include <thread>
#include "../synth/synth_architecture.h"
#include "ui_parameters.h"  // This now contains the Parameter enum

class UI {
private:
    SynthArchitecture* synth;
    std::atomic<bool> running;
    std::thread uiThread;
    Parameter selectedParameter;
    
    void run();
    
public:
    UI(SynthArchitecture* synthArch);
    ~UI();
    
    void start();
    void stop();
    bool isRunning() const { return running.load(); }
};

#endif
