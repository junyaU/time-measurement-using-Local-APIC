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

char console_mem[sizeof(Console)];
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
    InitializeScreenDrawer(frame_buffer_config);

    DrawConsoleScreen();

    console =
        new (console_mem) Console{*screen_drawer, {50, 200, 10}, {0, 0, 0}};

    printj("hello");

    while (1) {
        __asm__("hlt");
    };
}