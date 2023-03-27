#include "console.hpp"

#include <cstdio>

#include "font.hpp"

#if __INTELLISENSE__
#undef va_start(arg, va)
#define va_start(arg, va)
#undef va_end(va)
#define va_end(va)
#define __INT_MAX__ 0x7fffffff
#endif

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
            WriteAscii(drawer_, cursor_column_ * kFontHorizonPixel,
                       cursor_row_ * kFontVerticalPixel, *s, fg_color_);
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

    for (int y = 0; y < kFontVerticalPixel * kRows; y++) {
        for (int x = 0; x < kFontHorizonPixel * kColumns; x++) {
            drawer_.Draw(x, y, bg_color_);
        }
    }

    for (int row = 0; row < kRows - 1; row++) {
        memcpy(buffer_[row], buffer_[row + 1], kColumns + 1);
        WriteString(drawer_, 0, kFontVerticalPixel * row, buffer_[row],
                    fg_color_);
    }

    memset(buffer_[kRows - 1], 0, kColumns + 1);
}

namespace {
char console_mem[sizeof(Console)];
}

Console* console;

void initializeConsole() {
    ::console =
        new (console_mem) Console{*screen_drawer, {50, 200, 10}, {0, 0, 0}};
}

int printj(const char* format, ...) {
    va_list ap;
    int result;
    char s[1024];

    va_start(ap, format);
    result = vsprintf(s, format, ap);
    va_end(ap);

    console->PutString(s);
    return result;
}