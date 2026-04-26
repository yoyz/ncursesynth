#ifndef UI_COLORS_H
#define UI_COLORS_H

#include <ncurses.h>

namespace UIColor {
    const int FILTER_MOOG = 1;
    const int FILTER_KORG = 2;
    const int FILTER_OBERHEIM = 3;
    const int SELECTED = 4;
    const int HIGHLIGHT = 5;
    const int STATUS_OK = 6;
    const int STATUS_ERROR = 7;
    
    inline void initialize() {
        if (has_colors()) {
            start_color();
            init_pair(FILTER_MOOG, COLOR_RED, COLOR_BLACK);
            init_pair(FILTER_KORG, COLOR_GREEN, COLOR_BLACK);
            init_pair(FILTER_OBERHEIM, COLOR_YELLOW, COLOR_BLACK);
            init_pair(SELECTED, COLOR_WHITE, COLOR_BLUE);
            init_pair(HIGHLIGHT, COLOR_CYAN, COLOR_BLACK);
            init_pair(STATUS_OK, COLOR_GREEN, COLOR_BLACK);
            init_pair(STATUS_ERROR, COLOR_RED, COLOR_BLACK);
        }
    }
}

#endif
