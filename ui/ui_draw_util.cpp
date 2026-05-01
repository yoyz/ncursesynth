#include "ui_draw_util.h"
#include "ui_colors.h"
#include <cmath>
#include <sstream>
#include <iomanip>

void UIDrawUtil::drawSlider(int row, int col, float value, bool selected, bool bipolar) {
    if (selected) {
        attron(A_REVERSE);
        attron(COLOR_PAIR(UIColor::SELECTED));
    }

    int fillWidth = static_cast<int>(value * SLIDER_WIDTH);
    if (bipolar) {
        int center = SLIDER_WIDTH / 2;
        mvaddch(row, col, '[');
        for (int i = 0; i < fillWidth; i++) {
            if (i < center - 1) {
                addch('=');
            } else if (i == center - 1) {
                addch('|');
            } else {
                addch('=');
            }
        }
        for (int i = fillWidth; i < SLIDER_WIDTH; i++) {
            addch(' ');
        }
        mvaddch(row, col + SLIDER_WIDTH + 1, ']');
    } else {
        mvaddch(row, col, '[');
        for (int i = 0; i < fillWidth; i++) {
            addch('=');
        }
        for (int i = fillWidth; i < SLIDER_WIDTH; i++) {
            addch(' ');
        }
        mvaddch(row, col + SLIDER_WIDTH + 1, ']');
    }

    if (selected) {
        attroff(COLOR_PAIR(UIColor::SELECTED));
        attroff(A_REVERSE);
    }
}

void UIDrawUtil::drawLabel(int row, int col, const char* label) {
    attron(COLOR_PAIR(UIColor::LABEL));
    mvprintw(row, col, "%s", label);
    attroff(COLOR_PAIR(UIColor::LABEL));
}

void UIDrawUtil::drawValue(int row, int col, const char* value) {
    attron(COLOR_PAIR(UIColor::VALUE));
    mvprintw(row, col, "%s", value);
    attroff(COLOR_PAIR(UIColor::VALUE));
}

void UIDrawUtil::drawSectionHeader(int row, int col, const char* title) {
    attron(A_BOLD);
    attron(COLOR_PAIR(UIColor::SECTION_HEADER));
    mvprintw(row, col, "=== %s ===", title);
    attroff(COLOR_PAIR(UIColor::SECTION_HEADER));
    attroff(A_BOLD);
}

void UIDrawUtil::drawVerticalMarkers(int row, int col, int width) {
    mvaddch(row, col, '|');
    mvaddch(row, col + width + 1, '|');
}

std::string UIDrawUtil::floatToString(float value, int decimals) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(decimals) << value;
    return oss.str();
}

std::string UIDrawUtil::intToString(int value) {
    return std::to_string(value);
}