#include "widget.h"
#include "../machine/Machine.h"
#include <cstdio>
#include <cstring>

static constexpr int CONTROL_BAR_LEN = 16;
static constexpr int NAME_WIDTH = 11;

Widget::Widget(const std::string& name, int paramId, int row, int col, float defaultNorm)
    : name(name), paramId(paramId), row(row), col(col) {
    value = (int)(defaultNorm * 127.0f);
    if (value < 0) value = 0;
    if (value > 127) value = 127;
}

void Widget::draw(IRenderer& r, bool selected, Machine* machine) const {
    int drawRow = row + 7;

    char nameBuf[16];
    snprintf(nameBuf, sizeof(nameBuf), "%-*s", NAME_WIDTH, name.c_str());

    if (selected) r.setStyle(Style::REVERSE);

    const char* displayStr = machine ? machine->getDisplayString(paramId) : nullptr;
    if (displayStr && displayStr[0] != '\0') {
        r.write(drawRow, col, nameBuf);
        r.write(drawRow, col + NAME_WIDTH + 1, "[");
        r.write(drawRow, col + NAME_WIDTH + 2, displayStr);
        r.write(drawRow, col + NAME_WIDTH + 2 + strlen(displayStr), "]");
    } else {
        r.write(drawRow, col, nameBuf);
        r.write(drawRow, col + NAME_WIDTH + 1, "[");
        r.drawBar(drawRow, col + NAME_WIDTH + 2, value, 127, CONTROL_BAR_LEN);
        r.write(drawRow, col + NAME_WIDTH + 2 + CONTROL_BAR_LEN, "]");

        int pct = value * 100 / 127;
        char pctStr[16];
        snprintf(pctStr, sizeof(pctStr), "%3d%%", pct);
        r.write(drawRow, col + NAME_WIDTH + 2 + CONTROL_BAR_LEN + 2, pctStr);
    }

    if (selected) r.setStyle(Style::NORMAL);
}

bool Widget::onKey(int key, Machine* machine) {
    if (!machine) return false;
    if (key != Key::LEFT && key != Key::RIGHT) return false;

    int oldValue = value;

    if (key == Key::LEFT && value > 0) {
        value--;
    } else if (key == Key::RIGHT && value < 127) {
        value++;
    }

    if (value != oldValue) {
        machine->setI(paramId, value);
        return true;
    }
    return false;
}
