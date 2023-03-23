#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>

#include "acpi.hpp"
#include "console.hpp"
#include "drawing.hpp"
#include "font.hpp"
#include "frame_buffer_config.hpp"
#include "interrupt.hpp"
#include "memory_map.hpp"

#if __INTELLISENSE__
#undef va_start(arg, va)
#define va_start(arg, va)
#undef va_end(va)
#define va_end(va)
#define __INT_MAX__ 0x7fffffff
#endif

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
                     const MemoryMap& memory_map, const acpi::RSDP& rsdp) {
    InitializeScreenDrawer(frame_buffer_config);

    DrawConsoleScreen();

    InitializeConsole();

    printj("success initialization of screen drawing\n");
    printj("success initialization of console\n");

    acpi::Initialize(rsdp);

    printj("ok\n");
    acpi::waitMilliSec(1000);
    printj("aaa\n");
    acpi::waitMilliSec(1000);
    printj("bbb\n");

    while (1) {
        __asm__("hlt");
    };
}