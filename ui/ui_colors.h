#ifndef UI_COLORS_H
#define UI_COLORS_H

#include <ncurses.h>

namespace UIColor {
    const int BG = 1;
    const int SLIDER_FG = 2;
    const int SLIDER_BG = 3;
    const int LOGO = 4;
    const int PATCH_LOAD = 5;
    const int CONTROL_TEXT = 6;
    const int SECTION_HEADER = 7;
    const int VALUE = 8;
    const int LABEL = 9;
    const int KEYBOARD = 10;
    const int MOD_MATRIX = 11;

    const int FILTER_MOOG = 1;
    const int FILTER_KORG = 2;
    const int FILTER_OBERHEIM = 3;
    const int SELECTED = 12;
    const int HIGHLIGHT = 13;
    const int STATUS_OK = 14;
    const int STATUS_ERROR = 15;

    inline void initialize() {
        if (has_colors()) {
            start_color();
            init_pair(BG, COLOR_BLACK, COLOR_BLACK);
            init_pair(SLIDER_FG, COLOR_WHITE, COLOR_YELLOW);
            init_pair(SLIDER_BG, COLOR_YELLOW, COLOR_WHITE);
            init_pair(LOGO, COLOR_RED, COLOR_BLACK);
            init_pair(PATCH_LOAD, COLOR_BLACK, COLOR_CYAN);
            init_pair(CONTROL_TEXT, COLOR_BLACK, COLOR_WHITE);
            init_pair(SECTION_HEADER, COLOR_WHITE, COLOR_BLACK);
            init_pair(VALUE, COLOR_WHITE, COLOR_BLACK);
            init_pair(LABEL, COLOR_WHITE, COLOR_BLACK);
            init_pair(KEYBOARD, COLOR_CYAN, COLOR_BLACK);
            init_pair(MOD_MATRIX, COLOR_MAGENTA, COLOR_BLACK);

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