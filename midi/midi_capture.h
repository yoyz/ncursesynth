#ifndef MIDI_CAPTURE_H
#define MIDI_CAPTURE_H

#include <string>
#include <vector>
#include <cstdio>
#include <cstdint>

class MidiCapture {
public:
    MidiCapture(const std::string& path);
    ~MidiCapture();

    void noteOn(int note, int vel);
    void noteOff(int note);
    void cc(int cc, int value);
    void finalize();

    bool failed() const { return writeFailed; }
    const std::string& getPath() const { return logPath; }
    int getEventCount() const { return eventCount; }

    static MidiCapture* instance;
    static void setInstance(MidiCapture* m) { instance = m; }
    static MidiCapture* getInstance() { return instance; }
    static void logNoteOn(int n, int v) { if (instance) instance->noteOn(n, v); }
    static void logNoteOff(int n) { if (instance) instance->noteOff(n); }
    static void logCC(int c, int v) { if (instance) instance->cc(c, v); }

private:
    struct Event {
        double time;
        char type; // 'N'=note_on, 'F'=note_off, 'C'=cc
        int p1, p2;
    };

    std::vector<Event> events;
    std::string logPath;
    std::string rawPath;
    bool writeFailed;
    int eventCount;
    double startTime;
};

#endif
