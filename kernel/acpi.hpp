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

    bool isValid() const;
} __attribute__((packed));

// System Description Table Header
struct SDTHeader {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_version;

    bool isValid(const char* signature) const;
} __attribute__((packed));

// eXtended System Description Table
struct XSDT {
    SDTHeader header;

    const SDTHeader& operator[](size_t i) const;
    size_t countSDTEntries() const;
} __attribute__((packed));

struct FADT {
    SDTHeader header;
    char reserved1[76 - sizeof(header)];
    uint32_t pm_tmr_blk;
    char reserved2[112 - 80];
    uint32_t flags;
    char reserved3[276 - 116];
} __attribute__((packed));

extern const FADT* fadt;

void Initialize(RSDP& rsdp);
}  // namespace acpi