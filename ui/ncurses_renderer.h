#ifndef NCURSES_RENDERER_H
#define NCURSES_RENDERER_H

#include "irenderer.h"
#include <chrono>

class NcursesRenderer : public IRenderer {
public:
    void init() override;
    void shutdown() override;
    void setRefreshIntervalMs(int ms) override;

    void clear() override;
    void refresh() override;

    int getKey() override;
    void getSize(int& rows, int& cols) const override;

    void write(int row, int col, const std::string& text) override;
    void setStyle(Style s) override;

    void drawBar(int row, int col, int current, int max, int width) override;

private:
    std::chrono::steady_clock::time_point lastRefresh_;
    int refreshIntervalMs_ = 30;
};

#endif
