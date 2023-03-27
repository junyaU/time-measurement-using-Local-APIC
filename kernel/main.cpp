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
#include "local_apic_timer.hpp"
#include "memory_map.hpp"

extern "C" void main(const FrameBufferConfig& frame_buffer_config,
                     const MemoryMap& memory_map, const acpi::RSDP& rsdp) {
    initializeScreenDrawer(frame_buffer_config);

    drawConsoleScreen();

    initializeConsole();

    printj("Booting the operating system...\n");

    initializeInterruptConfig();

    acpi::initialize(rsdp);

    initializeLocalAPICTimer();

    while (1) {
        __asm__("hlt");
    };
}