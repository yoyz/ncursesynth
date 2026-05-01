#ifndef UI_DRAW_UTIL_H
#define UI_DRAW_UTIL_H

#include <ncurses.h>
#include <string>

namespace UIDrawUtil {
    const int SLIDER_WIDTH = 20;

    void drawSlider(int row, int col, float value, bool selected, bool bipolar = false);
    void drawLabel(int row, int col, const char* label);
    void drawValue(int row, int col, const char* value);
    void drawSectionHeader(int row, int col, const char* title);
    void drawVerticalMarkers(int row, int col, int width);
    std::string floatToString(float value, int decimals = 2);
    std::string intToString(int value);
}

#endif