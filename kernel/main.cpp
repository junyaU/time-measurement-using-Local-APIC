#include <cstddef>
#include <cstdint>

#include "frame_buffer_config.hpp"
#include "memory_map.hpp"

void* operator new(size_t size, void* buf) { return buf; }

void operator delete(void* obj) noexcept {}

extern "C" void main(const FrameBufferConfig& frame_buffer_config,
                     const MemoryMap& memory_map) {
        while (1) {
        __asm__("hlt");
    };
}