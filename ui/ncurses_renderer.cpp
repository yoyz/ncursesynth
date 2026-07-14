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

void NcursesRenderer::drawBar(int row, int col, int current, int max, int width) {
    int filled = current * width / max;
    if (filled < 0) filled = 0;
    if (filled > width) filled = width;

    for (int i = 0; i < width; i++) {
        mvaddch(row, col + i, i < filled ? '#' : '-');
    }
}
