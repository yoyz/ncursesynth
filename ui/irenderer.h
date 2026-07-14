#ifndef IRENDERER_H
#define IRENDERER_H

#include <string>

enum class Style { NORMAL, BOLD, REVERSE, DIM };

struct Rect {
    int row, col, width, height;
};

namespace Key {
    constexpr int NONE = -1;
    constexpr int UP = 259;
    constexpr int DOWN = 258;
    constexpr int LEFT = 260;
    constexpr int RIGHT = 261;
    constexpr int PAGE_UP = 339;
    constexpr int PAGE_DOWN = 338;
    constexpr int TAB = '\t';
    constexpr int ENTER = '\n';
    constexpr int ESC = 27;
    constexpr int BACKSPACE = 127;
}

class IRenderer {
public:
    virtual ~IRenderer() = default;

    virtual void init() = 0;
    virtual void shutdown() = 0;
    virtual void setRefreshIntervalMs(int ms) = 0;

    virtual void clear() = 0;
    virtual void refresh() = 0;

    virtual int getKey() = 0;
    virtual void getSize(int& rows, int& cols) const = 0;

    virtual void write(int row, int col, const std::string& text) = 0;
    virtual void setStyle(Style s) = 0;

    virtual void drawBar(int row, int col, int current, int max, int width) = 0;

    void drawBracketedBar(int row, int col, int current, int max, int barWidth) {
        write(row, col, "[");
        drawBar(row, col + 1, current, max, barWidth);
        write(row, col + 1 + barWidth, "]");
    }
};

#endif
