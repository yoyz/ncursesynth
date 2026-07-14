#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <cstdio>
#include <ncurses.h>
#include "../ui/machine_ui.h"
#include "../ui/ncursesynth_ui.h"
#include "../ui/pbsynth_ui.h"
#include "../ui/cursynth_ui.h"
#include "../ui/twytch_ui.h"
#include "../machine/Ncursesynth/NcursesynthMachine.h"
#include "../machine/PBSynth/PBSynthMachine.h"
#include "../machine/Cursynth/CursynthMachine.h"
#include "../machine/Twytch/TwytchsynthMachine.h"
#include "../machine/MachineManager.h"

#define COL_WIDTH 36
#define NUM_COLUMNS 3
#define MIN_ROWS 24
#define MAX_CONTROL_NAME_LEN 13

static int g_passed = 0;
static int g_failed = 0;

static void printResult(const std::string& name, bool passed, const std::string& msg = "") {
    if (passed) {
        std::cout << "  [PASS] " << name;
        g_passed++;
    } else {
        std::cout << "  [FAIL] " << name;
        g_failed++;
    }
    if (!msg.empty()) std::cout << " " << msg;
    std::cout << std::endl;
}

struct EngineTest {
    std::string name;
    Machine* machine;
    MachineUI* ui;
};

static int controlColumn(int col) {
    if (col < 20) return 0;
    if (col < 60) return 1;
    return 2;
}

static bool testEngine(EngineTest& et) {
    bool allOk = true;
    const auto& widgets = et.ui->getWidgets();

    printResult(et.name + " has controls", !widgets.empty(),
                std::to_string(widgets.size()) + " controls");

    std::set<std::string> seenNames;
    std::set<std::pair<int,int>> seenPositions;

    for (size_t i = 0; i < widgets.size(); i++) {
        const auto& w = widgets[i];
        std::string prefix = et.name + "[" + std::to_string(i) + "]";

        bool hasName = !w.name.empty();
        if (!hasName) allOk = false;
        printResult(prefix + " name", hasName,
                    hasName ? "" : "EMPTY NAME");

        bool nameFits = w.name.length() < MAX_CONTROL_NAME_LEN;
        if (!nameFits) allOk = false;
        printResult(prefix + " \"" + w.name + "\" length",
                    nameFits, std::to_string(w.name.length()) + " chars");

        int drawRow = w.row + 7;
        bool rowOk = drawRow >= 0 && drawRow < MIN_ROWS + 20;
        if (!rowOk) allOk = false;
        printResult(prefix + " row", rowOk,
                    "row=" + std::to_string(drawRow));

        bool colOk = w.col >= 0 && w.col < 160;
        if (!colOk) allOk = false;
        printResult(prefix + " col", colOk,
                    "col=" + std::to_string(w.col));

        int colIdx = controlColumn(w.col);
        static const int colStarts[NUM_COLUMNS] = {2, 40, 78};
        int colStart = colStarts[colIdx];
        bool fitsInCol = (w.col - colStart + (int)w.name.length()) < COL_WIDTH;
        if (!fitsInCol) allOk = false;
        printResult(prefix + " fits column", fitsInCol,
                    "col=" + std::to_string(w.col) + " start=" + std::to_string(colStart));

        bool nameUnique = seenNames.find(w.name) == seenNames.end();
        if (!nameUnique) allOk = false;
        printResult(prefix + " unique name", nameUnique);
        seenNames.insert(w.name);

        auto pos = std::make_pair(w.row, w.col);
        bool posUnique = seenPositions.find(pos) == seenPositions.end();
        if (!posUnique) allOk = false;
        printResult(prefix + " unique position", posUnique);
        seenPositions.insert(pos);

        bool valOk = w.value >= 0 && w.value <= 127;
        if (!valOk) allOk = false;
        printResult(prefix + " value range", valOk,
                    "val=" + std::to_string(w.value));
    }

    return allOk;
}

int main() {
    FILE* nullOut = fopen("/dev/null", "w");
    if (!nullOut) {
        std::cerr << "ERROR: could not open /dev/null" << std::endl;
        return 1;
    }
    SCREEN* screen = newterm("xterm", nullOut, stdin);
    if (!screen) {
        std::cerr << "ERROR: could not init ncurses" << std::endl;
        fclose(nullOut);
        return 1;
    }
    set_term(screen);
    noecho();
    keypad(stdscr, TRUE);

    std::cout << "=== UI Layout Validation ===" << std::endl;

    MachineManager mgr;

    NcursesynthMachine ncMach;
    ncMach.init();
    NcursesynthUI ncUI(&ncMach, &mgr);

    PBSynthMachine pbMach;
    pbMach.init();
    PBSynthUI pbUI(&pbMach, &mgr);

    CursynthMachine curMach(8);
    curMach.init();
    CursynthUI curUI(&curMach, &mgr);

    TwytchsynthMachine twMach;
    twMach.init();
    TwytchUI twUI(&twMach, &mgr);

    std::vector<EngineTest> engines = {
        {"Ncursesynth", &ncMach, &ncUI},
        {"PBSynth", &pbMach, &pbUI},
        {"Cursynth", &curMach, &curUI},
        {"Twytch", &twMach, &twUI}
    };

    for (auto& et : engines) {
        std::cout << "\n=== " << et.name << " ===" << std::endl;
        testEngine(et);

        bool drawOk = true;
        try {
            et.ui->draw();
        } catch (...) {
            drawOk = false;
        }
        printResult(et.name + " draw()", drawOk);
    }

    fclose(nullOut);

    int total = g_passed + g_failed;
    double rate = total > 0 ? (100.0 * g_passed / total) : 0.0;

    std::cout << "\n=== Summary ===" << std::endl;
    std::cout << "Passed: " << g_passed << std::endl;
    std::cout << "Failed: " << g_failed << std::endl;
    std::cout << "Rate: " << rate << "%" << std::endl;

    return g_failed > 0 ? 1 : 0;
}
