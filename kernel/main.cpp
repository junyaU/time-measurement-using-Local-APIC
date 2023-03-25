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
    InitializeScreenDrawer(frame_buffer_config);

    DrawConsoleScreen();

    InitializeConsole();

    printj("success initialization of screen drawing\n");
    printj("success initialization of console\n");

    initializeInterruptConfig();
    printj("success initialization of intterupt config\n");

    acpi::Initialize(rsdp);
    printj("success initialization of acpi pm timer\n");

    initializeLocalAPICTimer();
    printj("success initialization of local apic timer\n");

    while (1) {
        __asm__("hlt");
    };
}