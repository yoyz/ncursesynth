#include "tcp_midi_server.h"
#include "../machine/Machine.h"
#include "../machine/MachineManager.h"
#include "../machine/ParamID.h"
#include "midi_capture.h"
#include <iostream>
#include <cstring>
#include <cmath>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>

TcpMidiServer::TcpMidiServer(int port, MachineManager* mgr)
    : port(port), manager(mgr), running(false) {}

TcpMidiServer::~TcpMidiServer() {
    stop();
}

bool TcpMidiServer::start() {
    if (running.load()) return true;
    running.store(true);
    serverThread = std::thread(&TcpMidiServer::serverLoop, this);
    return true;
}

void TcpMidiServer::stop() {
    running.store(false);
    if (serverThread.joinable()) serverThread.join();
}

void TcpMidiServer::serverLoop() {
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0) {
        std::cerr << "[tcp-midi] Failed to create socket" << std::endl;
        return;
    }

    int opt = 1;
    setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(serverFd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "[tcp-midi] Failed to bind on port " << port << std::endl;
        close(serverFd);
        return;
    }

    if (listen(serverFd, 5) < 0) {
        std::cerr << "[tcp-midi] Failed to listen" << std::endl;
        close(serverFd);
        return;
    }

    std::cout << "[tcp-midi] MIDI server on port " << port << std::endl;

    while (running.load()) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int clientFd = accept(serverFd, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientFd < 0) {
            if (!running.load()) break;
            continue;
        }
        handleClient(clientFd);
        close(clientFd);
    }

    close(serverFd);
}

void TcpMidiServer::handleClient(int clientFd) {
    char buf[4096];
    ssize_t n = read(clientFd, buf, sizeof(buf) - 1);
    if (n <= 0) return;
    buf[n] = '\0';

    std::string data(buf);
    size_t pos = 0;
    while (pos < data.size()) {
        size_t nl = data.find('\n', pos);
        if (nl == std::string::npos) nl = data.size();
        std::string line = data.substr(pos, nl - pos);
        if (!line.empty()) {
            handleMessage(line);
        }
        pos = nl + 1;
    }
}

void TcpMidiServer::handleMessage(const std::string& json) {
    Machine* mach = manager ? manager->getCurrentMachine() : nullptr;
    if (!mach) return;

    auto findStr = [&](const std::string& key) -> std::string {
        size_t k = json.find("\"" + key + "\"");
        if (k == std::string::npos) return "";
        size_t v = json.find(':', k);
        if (v == std::string::npos) return "";
        v = json.find_first_of("\"0123456789-", v + 1);
        if (v == std::string::npos) return "";
        if (json[v] == '\"') {
            size_t end = json.find('\"', v + 1);
            if (end == std::string::npos) return "";
            return json.substr(v + 1, end - v - 1);
        }
        size_t end = json.find_first_of(",\n}", v + 1);
        if (end == std::string::npos) end = json.size();
        return json.substr(v, end - v);
    };

    auto type = findStr("type");

    if (type == "note_on") {
        int note = 60, vel = 100;
        try { note = std::stoi(findStr("note")); } catch (...) {}
        try { vel = std::stoi(findStr("velocity")); } catch (...) {}
        MidiCapture::logNoteOn(note, vel);
        if (vel > 0) {
            mach->setI(ParamID::note, note);
            mach->setI(ParamID::note_on, 1);
        } else {
            mach->setI(ParamID::note_on, 0);
        }
    } else if (type == "note_off") {
        int note = 60;
        try { note = std::stoi(findStr("note")); } catch (...) {}
        (void)note;
        MidiCapture::logNoteOff(note);
        mach->setI(ParamID::note_on, 0);
    } else if (type == "cc") {
        int cc = 0, value = 64;
        try { cc = std::stoi(findStr("cc")); } catch (...) {}
        try { value = std::stoi(findStr("value")); } catch (...) {}
        MidiCapture::logCC(cc, value);
        mach->applyCC(cc, value / 127.0f, "");
    } else if (type == "reset") {
        mach->reset();
    } else if (type == "setI") {
        int index = 0, value = 0;
        try { index = std::stoi(findStr("index")); } catch (...) {}
        try { value = std::stoi(findStr("value")); } catch (...) {}
        mach->setI(index, value);
    }
}
