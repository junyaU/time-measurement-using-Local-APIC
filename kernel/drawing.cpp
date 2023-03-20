#include "drawing.hpp"

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