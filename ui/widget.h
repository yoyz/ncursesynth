#ifndef WIDGET_H
#define WIDGET_H

#include <string>
#include "irenderer.h"

class Machine;

enum class WidgetType {
    CONTINUOUS,  // 0-127, percentage bar, step by 1
    DISCRETE,    // N selections, name display, step by 1 index
    BIPOLAR      // 0-127 centered at 64, centered bar
};

class Widget {
public:
    std::string name;
    int paramId;
    int row;
    int col;
    int value;
    WidgetType type;
    int discreteCount;

    Widget(const std::string& name, int paramId, int row, int col,
           WidgetType type = WidgetType::CONTINUOUS, float defaultNorm = 0.5f,
           int discreteCount = 0);

    void draw(IRenderer& r, bool selected, Machine* machine) const;
    bool onKey(int key, Machine* machine);
};

#endif
