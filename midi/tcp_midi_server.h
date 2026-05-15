#ifndef TCP_MIDI_SERVER_H
#define TCP_MIDI_SERVER_H

#include <atomic>
#include <thread>
#include <string>

class Machine;
class MachineManager;

class TcpMidiServer {
public:
    TcpMidiServer(int port, MachineManager* mgr);
    ~TcpMidiServer();

    bool start();
    void stop();

private:
    int port;
    MachineManager* manager;
    std::atomic<bool> running;
    std::thread serverThread;

    void serverLoop();
    void handleClient(int clientFd);
    void handleMessage(const std::string& json);
};

#endif
