#include <cstddef>
#include <cstdint>
#include <cstdio>

#include "console.hpp"
#include "drawing.hpp"
#include "font.hpp"
#include "frame_buffer_config.hpp"
#include "memory_map.hpp"

#if __INTELLISENSE__
#undef va_start(arg, va)
#define va_start(arg, va)
#undef va_end(va)
#define va_end(va)
#define __INT_MAX__ 0x7fffffff
#endif

// placement new
void* operator new(size_t size, void* buf) { return buf; }
void operator delete(void* obj) noexcept {}

char screen_drawer_buf[sizeof(RGB8BitScreenDrawer)];
ScreenDrawer* screen_drawer;

char console_buf[sizeof(Console)];
Console* console;

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
            screen_drawer->Draw(x, y, {0, 0, 0});
        }
    }

    console =
        new (console_buf) Console{*screen_drawer, {50, 200, 10}, {0, 0, 0}};

    printj("hello");

    while (1) {
        __asm__("hlt");
    };
}