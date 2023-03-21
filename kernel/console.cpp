#include "console.hpp"

#include <cstring>
#include <new>

#include "font.hpp"

Console::Console(ScreenDrawer& drawer, const PixelColor& fg_color,
                 const PixelColor& bg_color)
    : drawer_{drawer},
      fg_color_{fg_color},
      bg_color_{bg_color},
      buffer_{},
      cursor_row_{0},
      cursor_column_{0} {};

void Console::PutString(const char* s) {
    while (*s != '\0') {
        if (*s == '\n') {
            NewLine();
        } else if (cursor_column_ < kColumns - 1) {
            WriteAscii(drawer_, cursor_column_ * 8, cursor_row_ * 16, *s,
                       fg_color_);
            buffer_[cursor_row_][cursor_column_] = *s;
            cursor_column_++;
        }

        s++;
    }
}

void Console::NewLine() {
    if (cursor_row_ < kRows - 1) {
        cursor_row_++;
        cursor_column_ = 0;
        return;
    }

    cursor_column_ = 0;

    for (int y = 0; y < 16 * kRows; y++) {
        for (int x = 0; x < 8 * kColumns; x++) {
            drawer_.Draw(x, y, bg_color_);
        }
    }

    for (int row = 0; row < kRows - 1; row++) {
        memcpy(buffer_[row], buffer_[row + 1], kColumns + 1);
        WriteString(drawer_, 0, 16 * row, buffer_[row], fg_color_);
    }

    memset(buffer_[kRows - 1], 0, kColumns + 1);
}

namespace {
char console_mem[sizeof(Console)];
}

Console* console;

void InitializeConsole() {
    ::console =
        new (console_mem) Console{*screen_drawer, {50, 200, 10}, {0, 0, 0}};
}