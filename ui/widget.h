#ifndef WIDGET_H
#define WIDGET_H

#include <string>
#include "irenderer.h"

class Machine;

class Widget {
public:
    std::string name;
    int paramId;
    int row;
    int col;
    int value; // raw 0-127

    Widget(const std::string& name, int paramId, int row, int col, float defaultNorm = 0.5f);

    void draw(IRenderer& r, bool selected, Machine* machine) const;
    bool onKey(int key, Machine* machine); // returns true if value changed
};

#endif
