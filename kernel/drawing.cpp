#include "drawing.hpp"

#include <new>

void RGB8BitScreenDrawer::Draw(int x, int y, const PixelColor& c) {
    auto target_pixel_addr = pixelAt(x, y);
    target_pixel_addr[0] = c.r;
    target_pixel_addr[1] = c.g;
    target_pixel_addr[2] = c.b;
}

void BGR8BitScreenDrawer::Draw(int x, int y, const PixelColor& c) {
    auto target_pixel_addr = pixelAt(x, y);
    target_pixel_addr[0] = c.b;
    target_pixel_addr[1] = c.g;
    target_pixel_addr[2] = c.r;
}

namespace {
char screen_drawer_mem[sizeof(RGB8BitScreenDrawer)];
}

ScreenDrawer* screen_drawer;
FrameBufferConfig config;

void InitializeScreenDrawer(const FrameBufferConfig config) {
    ::config = config;

    switch (config.pixel_format) {
        case kPixelBGR8BitPerColor:
            ::screen_drawer =
                new (screen_drawer_mem) BGR8BitScreenDrawer{config};
            break;
        case kPixelRGB8BitPerColor:
            ::screen_drawer =
                new (screen_drawer_mem) RGB8BitScreenDrawer{config};
            break;
    }
}

void DrawConsoleScreen() {
    for (int x = 0; x < config.horizon_resolution; x++) {
        for (int y = 0; y < config.vertical_resolution; y++) {
            screen_drawer->Draw(x, y, {0, 0, 0});
        }
    }
};
