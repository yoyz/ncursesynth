#include "ncurses_renderer.h"
#include <ncurses.h>

void NcursesRenderer::init() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    start_color();
    nodelay(stdscr, TRUE);
    timeout(50);
    lastRefresh_ = std::chrono::steady_clock::now();
}

void NcursesRenderer::shutdown() {
    endwin();
}

void NcursesRenderer::setRefreshIntervalMs(int ms) {
    refreshIntervalMs_ = ms > 0 ? ms : 30;
}

void NcursesRenderer::clear() {
    erase();
}

void NcursesRenderer::refresh() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - lastRefresh_).count();
    if (elapsed < refreshIntervalMs_)
        return;
    lastRefresh_ = now;
    ::refresh();
}

int NcursesRenderer::getKey() {
    return getch();
}

void NcursesRenderer::getSize(int& rows, int& cols) const {
    getmaxyx(stdscr, rows, cols);
}

void NcursesRenderer::write(int row, int col, const std::string& text) {
    mvprintw(row, col, "%s", text.c_str());
}

void NcursesRenderer::setStyle(Style s) {
    switch (s) {
        case Style::NORMAL:  attrset(A_NORMAL); break;
        case Style::BOLD:    attron(A_BOLD); break;
        case Style::REVERSE: attron(A_REVERSE); break;
        case Style::DIM:     attron(A_DIM); break;
    }
}

void NcursesRenderer::drawBar(int row, int col, int current, int max, int width, bool bipolar) {
    if (bipolar) {
        int half = width / 2;
        int center = max / 2;
        int offset = current - center;
        int rightFilled = 0, leftFilled = 0;
        if (offset > 0)
            rightFilled = offset * half / center;
        else
            leftFilled = (-offset) * half / center;
        if (rightFilled > half) rightFilled = half;
        if (leftFilled > half) leftFilled = half;

        mvaddch(row, col + half, '|');
        for (int i = 0; i < half; i++)
            mvaddch(row, col + i, (half - 1 - i) < leftFilled ? '#' : '-');
        for (int i = 0; i < half; i++)
            mvaddch(row, col + half + 1 + i, i < rightFilled ? '#' : '-');
    } else {
        int filled = current * width / max;
        if (filled < 0) filled = 0;
        if (filled > width) filled = width;

        for (int i = 0; i < width; i++) {
            mvaddch(row, col + i, i < filled ? '#' : '-');
        }
    }
}
