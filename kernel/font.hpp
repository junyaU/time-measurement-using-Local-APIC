#pragma once

#include <cstdint>

#include "drawing.hpp"

void WriteAscii(ScreenDrawer& drawer, int x, int y, char c,
                const PixelColor& color);
void WriteString(ScreenDrawer& drawer, int x, int y, const char* s,
                 const PixelColor& color);

constexpr int kFontHorizonPixel = 8;
constexpr int kFontVerticalPixel = 16;
