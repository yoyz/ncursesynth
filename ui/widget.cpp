#include "widget.h"
#include "../machine/Machine.h"
#include <cstdio>
#include <cstring>

static constexpr int BAR_LEN = 16;
static constexpr int NAME_WIDTH = 11;

Widget::Widget(const std::string& name, int paramId, int row, int col,
               WidgetType type, float defaultNorm, int discreteCount)
    : name(name), paramId(paramId), row(row), col(col), type(type), value(0),
      discreteCount(discreteCount) {
    if (type == WidgetType::DISCRETE && discreteCount > 1) {
        int maxIdx = discreteCount - 1;
        value = (int)(defaultNorm * maxIdx + 0.5f);
        if (value < 0) value = 0;
        if (value > maxIdx) value = maxIdx;
    } else {
        value = (int)(defaultNorm * 127.0f + 0.5f);
        if (value < 0) value = 0;
        if (value > 127) value = 127;
    }
}

void Widget::draw(IRenderer& r, bool selected, Machine* machine) const {
    int drawRow = row + 7;

    char nameBuf[16];
    snprintf(nameBuf, sizeof(nameBuf), "%-*s", NAME_WIDTH, name.c_str());

    if (selected) r.setStyle(Style::REVERSE);

    if (type == WidgetType::DISCRETE) {
        const char* displayStr = machine ? machine->getDisplayString(paramId) : nullptr;
        if (displayStr && displayStr[0] != '\0') {
            r.write(drawRow, col, nameBuf);
            r.write(drawRow, col + NAME_WIDTH + 1, "[");
            r.write(drawRow, col + NAME_WIDTH + 2, displayStr);
            r.write(drawRow, col + NAME_WIDTH + 2 + strlen(displayStr), "]");
        } else {
            r.write(drawRow, col, nameBuf);
            r.write(drawRow, col + NAME_WIDTH + 1, "[");
            r.write(drawRow, col + NAME_WIDTH + 2, "---");
            r.write(drawRow, col + NAME_WIDTH + 5, "]");
        }
    } else if (type == WidgetType::BIPOLAR) {
        r.write(drawRow, col, nameBuf);
        r.write(drawRow, col + NAME_WIDTH + 1, "[");
        r.drawBar(drawRow, col + NAME_WIDTH + 2, value, 127, BAR_LEN, true);
        r.write(drawRow, col + NAME_WIDTH + 2 + BAR_LEN, "]");
        int pct;
        if (value <= 64)
            pct = (value - 64) * 100 / 64;
        else
            pct = (value - 64) * 100 / 63;
        char pctStr[16];
        snprintf(pctStr, sizeof(pctStr), "%+4d%%", pct);
        r.write(drawRow, col + NAME_WIDTH + 2 + BAR_LEN + 2, pctStr);
    } else {
        r.write(drawRow, col, nameBuf);
        r.write(drawRow, col + NAME_WIDTH + 1, "[");
        r.drawBar(drawRow, col + NAME_WIDTH + 2, value, 127, BAR_LEN);
        r.write(drawRow, col + NAME_WIDTH + 2 + BAR_LEN, "]");
        int pct = (value * 100 + 63) / 127;
        char pctStr[16];
        snprintf(pctStr, sizeof(pctStr), "%3d%%", pct);
        r.write(drawRow, col + NAME_WIDTH + 2 + BAR_LEN + 2, pctStr);
    }

    if (selected) r.setStyle(Style::NORMAL);
}

bool Widget::onKey(int key, Machine* machine) {
    if (!machine) return false;
    if (key != Key::LEFT && key != Key::RIGHT) return false;

    if (type == WidgetType::DISCRETE) {
        int oldValue = value;
        if (key == Key::LEFT && value > 0)
            value--;
        else if (key == Key::RIGHT && value < discreteCount - 1)
            value++;
        if (value != oldValue) {
            machine->lock();
            machine->setI(paramId, value);
            machine->unlock();
            return true;
        }
        return false;
    }

    int oldValue = value;
    if (key == Key::LEFT && value > 0)
        value--;
    else if (key == Key::RIGHT && value < 127)
        value++;
    if (value != oldValue) {
        machine->lock();
        machine->setI(paramId, value);
        machine->unlock();
        return true;
    }
    return false;
}
