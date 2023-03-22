#pragma once

#include <cstddef>
#include <cstdint>

// Advanced Configuration and Power Interface
namespace acpi {

// Root System Description Pointer
struct RSDP {
    char signature[8];
    uint8_t checksum;
    char oem_id[6];
    uint8_t revision;
    uint32_t rsdt_addr;

    // RSDP version 2.0
    uint32_t length;
    uint64_t xsdt_addr;
    uint8_t extended_checksum;
    char reserved[3];
} __attribute__((packed));
}  // namespace acpi