/**
 * MIDI Learn Protocol - TCP communication format
 * 
 * Server listens on configurable port (default 8765).
 * Clients send plain-text commands with length-prefixed encoding.
 * 
 * Wire format: 4 bytes big-endian length, then payload.
 * 
 * Client -> Server:
 *   "cc <cc_num> <value>\n"          - standard CC message (0-127, 0-127)
 *   "nrpn <msb> <lsb> <value>\n"     - NRPN message
 *   "quit\n"                          - close connection
 * 
 * Server -> Client:
 *   "ok\n"                            - acknowledgment
 */

#include <cstdint>
#include <string>
#include <cstring>
#include <sstream>
#include <iostream>

// Parse "cc N V" or "nrpn N N V" from string
static bool parseMidMessage(const std::string& msg, int& outType, int& outCC,
                            int& outMsb, int& outLsb, int& outVal) {
    std::istringstream iss(msg);
    std::string cmd;
    iss >> cmd;
    
    if (cmd == "quit") {
        outType = 2;
        return true;
    }
    if (cmd == "cc") {
        if (iss >> outCC >> outVal) {
            if (outCC >= 0 && outCC <= 127 && outVal >= 0 && outVal <= 127) {
                outType = 0;
                outMsb = 0;
                outLsb = 0;
                return true;
            }
        }
    }
    if (cmd == "nrpn") {
        if (iss >> outMsb >> outLsb >> outVal) {
            if (outMsb >= 0 && outMsb <= 127 && outLsb >= 0 && outLsb <= 127 && outVal >= 0 && outVal <= 127) {
                outType = 1;
                outCC = 0;
                return true;
            }
        }
    }
    return false;
}

// Format response
static std::string makeResponse(const std::string& cmd, int arg = 0) {
    if (arg != 0) return cmd + " " + std::to_string(arg);
    return cmd;
}

// Read length-prefixed message from socket descriptor
static std::string readLenMsg(int sockfd) {
    char lenBytes[4];
    ssize_t n = read(sockfd, lenBytes, 4);
    if (n != 4) return ""; // connection closed or error
    uint32_t len = (lenBytes[0] << 24) | (lenBytes[1] << 16) | (lenBytes[2] << 8) | lenBytes[3];
    if (len > 4096) return "";
    std::string msg(len, '\0');
    if (read(sockfd, &msg[0], len) < (ssize_t)len) return "";
    return msg;
}

// Write length-prefixed message to socket descriptor
static bool writeLenMsg(int sockfd, const std::string& msg) {
    uint32_t len = msg.size();
    char lenBytes[4] = {
        static_cast<char>((len >> 24) & 0xFF),
        static_cast<char>((len >> 16) & 0xFF),
        static_cast<char>((len >> 8) & 0xFF),
        static_cast<char>(len & 0xFF)
    };
    write(sockfd, lenBytes, 4);
    write(sockfd, msg.c_str(), len);
    return true;
}
