#include <cstddef>
#include <cstdint>

#include "drawing.hpp"
#include "frame_buffer_config.hpp"
#include "memory_map.hpp"

void* operator new(size_t size, void* buf) { return buf; }

void operator delete(void* obj) noexcept {}

char screen_drawer_buf[sizeof(RGB8BitScreenDrawer)];
ScreenDrawer* screen_drawer;

extern "C" void main(const FrameBufferConfig& frame_buffer_config,
                     const MemoryMap& memory_map) {
    switch (frame_buffer_config.pixel_format) {
        case kPixelBGR8BitPerColor:
            screen_drawer = new (screen_drawer_buf)
                BGR8BitScreenDrawer{frame_buffer_config};
            break;
        case kPixelRGB8BitPerColor:
            screen_drawer = new (screen_drawer_buf)
                RGB8BitScreenDrawer{frame_buffer_config};
            break;
    }

    for (int x = 0; x < frame_buffer_config.horizon_resolution; x++) {
        for (int y = 0; y < frame_buffer_config.vertical_resolution; y++) {
            screen_drawer->Draw(x, y, {50, 200, 10});
        }
    }

    while (1) {
        __asm__("hlt");
    };
}