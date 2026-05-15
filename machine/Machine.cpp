#include "Machine.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <cctype>
#include <sys/types.h>
#include <dirent.h>
#include <cerrno>
#include <cstring>

std::vector<std::pair<std::string, int>> Machine::getPresetParams() const {
    return {};
}

bool Machine::loadPreset(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return false;

    auto params = getPresetParams();
    std::map<std::string, int> paramMap;
    for (const auto& p : params) {
        paramMap[p.first] = p.second;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;

        std::string key = line.substr(0, eq);
        std::string valStr = line.substr(eq + 1);

        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        valStr.erase(0, valStr.find_first_not_of(" \t"));
        valStr.erase(valStr.find_last_not_of(" \t") + 1);

        auto it = paramMap.find(key);
        if (it == paramMap.end()) continue;

        int value = 0;
        try {
            size_t pos;
            value = std::stoi(valStr, &pos);
            if (pos != valStr.length()) {
                float fval = std::stof(valStr, &pos);
                if (pos == valStr.length()) {
                    value = static_cast<int>(fval);
                }
            }
        } catch (...) {
            continue;
        }

        value = std::max(0, std::min(127, value));
        setI(it->second, value);
    }

    return true;
}

bool Machine::savePreset(const std::string& path) const {
    auto params = getPresetParams();
    if (params.empty()) return false;

    std::ofstream file(path);
    if (!file.is_open()) return false;

    file << "# " << name_ << " preset\n";
    for (const auto& p : params) {
        int val = const_cast<Machine*>(this)->getI(p.second);
        file << p.first << "=" << val << "\n";
    }

    return true;
}

std::string toLower(const std::string& s) {
    std::string r = s;
    for (auto& c : r) c = std::tolower(c);
    return r;
}

std::vector<std::string> Machine::getPresetList(const std::string& engineName) {
    std::vector<std::string> presets;
    std::string dirPath = "bank/" + toLower(engineName);

    DIR* dir = opendir(dirPath.c_str());
    if (!dir) return presets;

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_REG) {
            std::string name = entry->d_name;
            if (name != "index.txt" && name[0] != '.') {
                presets.push_back(name);
            }
        }
    }
    closedir(dir);

    std::sort(presets.begin(), presets.end());
    return presets;
}
