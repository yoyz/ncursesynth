#ifndef MIDI_LEARN_H
#define MIDI_LEARN_H

#include <string>
#include <map>

struct EEntry {
    int cc;
    std::string pname;
    int minVal, maxVal, lastVal, defVal;
    std::string type;
    int status;
    bool isNRPN;
    int nrpnMSB, nrpnLSB;
    bool warn;
    std::string warnMsg;
    EEntry();
};

#endif
