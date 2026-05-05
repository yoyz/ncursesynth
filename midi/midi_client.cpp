/**
 * MIDI Client - SYNTH SIMULATOR
 * Connects to midi_learn TCP server (port 8765) and simulates MIDI messages
 * from hardware synthesizers using CSV mapping files.
 *
 * Usage:
 *   ./midi_client <synth> [MODE] [OPTIONS]
 *   ./midi_client --help
 *
 * Synth profiles (loaded from mapping/midiclient/):
 *   prologue      Korg Prologue
 *   summit         Novation Summit/Peak
 *   polybrute      Arturia PolyBrute
 *   deepmind       Behringer DeepMind 12
 *
 * Modes:
 *   --loop   Send all CC/NRPN values in sequence (min, mid, max)
 *   --demo   Play demo sequence (presets in musical order)
 *   --sweep N  Sweep parameter N from min to max
 *   --send CC VAL  Send single CC message
 *   --nrpn MSB LSB VAL  Send NRPN message
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>
#include <cmath>
#include <iomanip>

static volatile bool g_running = true;
static int g_sockfd = -1;
static const char* g_serverIP = "127.0.0.1";
static int g_serverPort = 8765;

struct SynthParam {
    std::string name;
    std::string section;
    std::string description;
    int cc;
    int ccMin, ccMax, ccDefault;
    bool hasNRPN;
    int nrpnMSB, nrpnLSB;
    int nrpnMin, nrpnMax, nrpnDefault;
    int orientation; // 0=0-based, 1=centered
};

struct SynthProfile {
    std::string name;
    std::string displayName;
    std::vector<SynthParam> params;
};

static int safeStoi(const std::string& s, int defaultVal) {
    if (s.empty()) return defaultVal;
    try {
        size_t pos;
        int val = std::stoi(s, &pos);
        if (pos == s.size()) return val;
    } catch (...) {}
    return defaultVal;
}

static std::map<std::string, SynthProfile> g_synths;

static int loadCSV(const std::string& filename, SynthProfile& profile) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: cannot open " << filename << std::endl;
        return -1;
    }

    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        std::vector<std::string> fields;
        std::string field;
        while (std::getline(iss, field, ',')) {
            fields.push_back(field);
        }

        if (lineNum == 1 && fields[0] == "manufacturer") continue;

        if (fields.size() < 15) continue;

if (fields.size() >= 14) {
            SynthParam p;
            p.name = fields[3];
            p.section = fields[2];
            p.description = (fields.size() > 4) ? fields[4] : "";

            int cc_msb = safeStoi(fields[5], -1);
            int cc_lsb = (fields.size() > 6) ? safeStoi(fields[6], -1) : -1;
            p.cc = (cc_msb >= 0) ? cc_msb : cc_lsb;
            p.ccMin = (fields.size() > 7) ? safeStoi(fields[7], 0) : 0;
            p.ccMax = (fields.size() > 8) ? safeStoi(fields[8], 127) : 127;
            p.ccDefault = (fields.size() > 9) ? safeStoi(fields[9], 64) : 64;

            if (p.cc >= 0) {
                profile.params.push_back(p);
            }
        }
    }
    return 0;
}

static void loadAllSynths() {
    g_synths.clear();

    std::vector<std::pair<std::string, std::string>> files = {
        {"prologue", "mapping/midiclient/korg_prologue.csv"},
        {"summit", "mapping/midiclient/novation_summit.csv"},
        {"polybrute", "mapping/midiclient/arturia_polybrute.csv"},
        {"deepmind", "mapping/midiclient/berhringer_deepmind_12.csv"},
    };

    for (const auto& f : files) {
        SynthProfile profile;
        profile.name = f.first;
        int ret = loadCSV(f.second, profile);
        if (ret == 0 && !profile.params.empty()) {
            if (f.first == "prologue") profile.displayName = "Korg Prologue";
            else if (f.first == "summit") profile.displayName = "Novation Summit";
            else if (f.first == "polybrute") profile.displayName = "Arturia PolyBrute";
            else if (f.first == "deepmind") profile.displayName = "Behringer DeepMind 12";
            else profile.displayName = f.first;
            g_synths[f.first] = profile;
        }
    }
}

static void printUsage() {
    std::cout << "MIDI Client - Synthesizer Simulator" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: midi_client <synth> [MODE] [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Synth profiles:" << std::endl;
    for (const auto& s : g_synths) {
        std::cout << "  " << s.first << " (" << s.second.displayName << ", "
                  << s.second.params.size() << " params)" << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Modes:" << std::endl;
    std::cout << "  --loop       Send all CC values in sequence (min, mid, max)" << std::endl;
    std::cout << "  --demo       Play demo sequence (musical order)" << std::endl;
    std::cout << "  --sweep N    Sweep parameter N from min to max" << std::endl;
    std::cout << "  --send CC VAL   Send single CC message" << std::endl;
    std::cout << "  --nrpn MSB LSB VAL  Send NRPN message" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --host IP    Server IP (default: 127.0.0.1)" << std::endl;
    std::cout << "  --port NUM   Server port (default: 8765)" << std::endl;
    std::cout << "  --help       Show this help" << std::endl;
}

static int connectToServer(const char* host, int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "error: cannot create socket" << std::endl;
        return -1;
    }
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, host, &addr.sin_addr);
    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "error: cannot connect to " << host << ":" << port << std::endl;
        close(sockfd);
        return -1;
    }
    return sockfd;
}

static bool sendMessage(int sockfd, const std::string& msg) {
    uint32_t len = msg.size();
    unsigned char lenBytes[4];
    lenBytes[0] = (len >> 24) & 0xFF;
    lenBytes[1] = (len >> 16) & 0xFF;
    lenBytes[2] = (len >> 8) & 0xFF;
    lenBytes[3] = len & 0xFF;
    write(sockfd, lenBytes, 4);
    write(sockfd, msg.c_str(), msg.size());
    return true;
}

static void sendCC(int sockfd, int cc, int value) {
    char buf[64];
    snprintf(buf, sizeof(buf), "{\"type\":\"cc\",\"cc\":%d,\"value\":%d}", cc, value);
    sendMessage(sockfd, buf);
}

static void sendNRPN(int sockfd, int msb, int lsb, int value) {
    char buf[64];
    snprintf(buf, sizeof(buf), "{\"type\":\"nrpn\",\"msb\":%d,\"lsb\":%d,\"value\":%d}", msb, lsb, value);
    sendMessage(sockfd, buf);
}

static void sendAllCCs(int sockfd, const std::vector<SynthParam>& params, const std::string& name) {
    std::cout << "Sending all CCs for " << name << " (" << params.size() << " params)..." << std::endl;
    for (size_t i = 0; i < params.size() && g_running; i++) {
        const SynthParam& p = params[i];
        sendCC(sockfd, p.cc, p.ccMin);
        usleep(50000);
        sendCC(sockfd, p.cc, (p.ccMin + p.ccMax) / 2);
        usleep(50000);
        sendCC(sockfd, p.cc, p.ccMax);
        usleep(50000);
    }
}

static void sendDemoSequence(int sockfd, const std::vector<SynthParam>& params, const std::string& name) {
    std::cout << "Playing demo for " << name << "..." << std::endl;
    std::vector<int> indices;
    for (size_t i = 0; i < params.size(); i++) indices.push_back(i);

    srand(time(nullptr));
    for (size_t shuffle = 0; shuffle < 3 && g_running; shuffle++) {
        for (size_t i = indices.size() - 1; i > 0; i--) {
            size_t j = rand() % (i + 1);
            std::swap(indices[i], indices[j]);
        }
    }

    for (size_t idx = 0; idx < indices.size() && g_running; idx++) {
        const SynthParam& p = params[indices[idx]];
        int mid = (p.ccMin + p.ccMax) / 2;
        sendCC(sockfd, p.cc, p.ccMin);
        usleep(80000);
        sendCC(sockfd, p.cc, mid);
        usleep(80000);
        sendCC(sockfd, p.cc, p.ccMax);
        usleep(80000);
    }
}

static void sendSweep(int sockfd, const std::vector<SynthParam>& params, int idx) {
    if (idx < 0 || idx >= (int)params.size()) {
        std::cerr << "Invalid parameter index: " << idx << std::endl;
        return;
    }
    const SynthParam& p = params[idx];
    std::cout << "Sweeping: " << p.name << " (CC " << p.cc << ")" << std::endl;

    for (int v = p.ccMin; v <= p.ccMax && g_running; v += 4) {
        sendCC(sockfd, p.cc, v);
        usleep(20000);
    }
    sendCC(sockfd, p.cc, p.ccDefault);
}

static void signal_handler(int) { g_running = false; }

int main(int argc, char* argv[]) {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    loadAllSynths();

    if (g_synths.empty()) {
        std::cerr << "Error: no synth profiles loaded" << std::endl;
        return 1;
    }

    std::string synthName;
    std::string mode = "loop";
    int ccNum = -1;
    int sendVal = 64;
    int nrpnMsb = 0, nrpnLsb = 0, nrpnVal = 0;
    bool useNRPN = false;
    int sweepCount = 0;
    int sweepIndices[10];

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (g_synths.find(arg) != g_synths.end()) {
            synthName = arg;
        } else if (arg == "--loop") {
            mode = "loop";
        } else if (arg == "--demo") {
            mode = "demo";
        } else if (arg == "--sweep") {
            if (i + 1 < argc) sweepIndices[sweepCount++] = std::stoi(argv[++i]);
            mode = "sweep";
        } else if (arg == "--send") {
            if (i + 2 < argc) {
                ccNum = std::stoi(argv[++i]);
                sendVal = std::stoi(argv[++i]);
                mode = "send";
            }
        } else if (arg == "--nrpn") {
            if (i + 3 < argc) {
                nrpnMsb = std::stoi(argv[++i]);
                nrpnLsb = std::stoi(argv[++i]);
                nrpnVal = std::stoi(argv[++i]);
                useNRPN = true;
                mode = "nrpn";
            }
        } else if (arg == "--host") {
            if (i + 1 < argc) g_serverIP = argv[++i];
        } else if (arg == "--port") {
            if (i + 1 < argc) g_serverPort = std::stoi(argv[++i]);
        } else if (arg == "--help") {
            printUsage();
            return 0;
        }
    }

    if (synthName.empty()) {
        std::cerr << "Error: no synth specified" << std::endl;
        printUsage();
        return 1;
    }

    if (g_synths.find(synthName) == g_synths.end()) {
        std::cerr << "Error: unknown synth: " << synthName << std::endl;
        return 1;
    }

    const SynthProfile& synth = g_synths[synthName];

    g_sockfd = connectToServer(g_serverIP, g_serverPort);
    if (g_sockfd < 0) return 1;

    std::cout << "Connected to " << g_serverIP << ":" << g_serverPort << std::endl;
    std::cout << "Synth: " << synth.displayName << " (" << synth.params.size() << " params)" << std::endl;

    if (mode == "loop") {
        sendAllCCs(g_sockfd, synth.params, synth.displayName);
    } else if (mode == "demo") {
        sendDemoSequence(g_sockfd, synth.params, synth.displayName);
    } else if (mode == "sweep") {
        for (int i = 0; i < sweepCount && i < 10; i++) {
            if (sweepIndices[i] < (int)synth.params.size()) {
                sendSweep(g_sockfd, synth.params, sweepIndices[i]);
            }
        }
    } else if (mode == "send") {
        sendCC(g_sockfd, ccNum, sendVal);
        std::cout << "Sent CC " << ccNum << " = " << sendVal << std::endl;
    } else if (mode == "nrpn") {
        sendNRPN(g_sockfd, nrpnMsb, nrpnLsb, nrpnVal);
        std::cout << "Sent NRPN " << nrpnMsb << ":" << nrpnLsb << " = " << nrpnVal << std::endl;
    }

    close(g_sockfd);
    return 0;
}