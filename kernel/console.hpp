#pragma once

#include <cstring>
#include <new>

#include "drawing.hpp"

class Console {
   public:
    static const int kRows = 25, kColumns = 80;

    Console(ScreenDrawer& drawer, const PixelColor& fg_color,
            const PixelColor& bg_color);
    void PutString(const char* s);

   private:
    void NewLine();

    ScreenDrawer& drawer_;
    const PixelColor fg_color_, bg_color_;
    char buffer_[kRows][kColumns + 1];
    int cursor_row_, cursor_column_;
};

extern Console* console;

void InitializeConsole();

int printj(const char* format, ...);
