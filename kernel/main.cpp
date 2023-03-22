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

const acpi::FADT* fadt;

extern "C" void main(const FrameBufferConfig& frame_buffer_config,
                     const MemoryMap& memory_map, const acpi::RSDP& rsdp) {
    InitializeScreenDrawer(frame_buffer_config);

    DrawConsoleScreen();

    InitializeConsole();

    if (!rsdp.isValid()) {
        printj("fail");
    }

    const acpi::XSDT& xsdt =
        *reinterpret_cast<const acpi::XSDT*>(rsdp.xsdt_addr);

    if (!xsdt.header.isValid("XSDT")) {
        printj("fail2");
    }

    fadt = nullptr;
    for (int i = 0; i < xsdt.countSDTEntries(); i++) {
        const auto& sdt_entry = xsdt[i];

        if (sdt_entry.isValid("FACP")) {
            fadt = reinterpret_cast<const acpi::FADT*>(&sdt_entry);
            break;
        }
    }

    if (fadt == nullptr) {
        printj("miss\n");
    }

    printj("ok");

    while (1) {
        __asm__("hlt");
    };
}