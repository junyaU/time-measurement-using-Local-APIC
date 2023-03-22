#pragma once

#include <array>
#include <cstdint>

enum class DescriptorType {
    kUpper8Bytes = 0,
    kLDT = 2,
    kTSSAvailable = 9,
    kTSSBusy = 11,
    kCallGate = 12,
    kInterruptGate = 14,
    kTrapGate = 15,
    kReadWrite = 2,
    kExecuteRead = 10,
};

union IDTEntryTypeAttribute {
    uint16_t data;

    struct {
        uint16_t ist : 3;
        uint16_t : 5;
        DescriptorType type : 4;
        uint16_t : 1;
        uint16_t descriptor_privilege_level : 2;
        uint16_t present : 1;
    } __attribute__((packed)) bits;
} __attribute__((packed));

struct IDTEntry {
    uint16_t offset_low;
    uint16_t segment_selector;
    IDTEntryTypeAttribute attr;
    uint16_t offset_mid;
    uint32_t offset_hi;
    uint32_t reserved;
} __attribute__((packed));

const int kIDTEntryNum = 256;

extern std::array<IDTEntry, kIDTEntryNum> idt;

constexpr IDTEntryTypeAttribute MakeIDTAttr(DescriptorType type,
                                            uint8_t privilege_level,
                                            bool present = true,
                                            uint8_t ist = 0) {
    IDTEntryTypeAttribute attr{};
    attr.bits.ist = ist;
    attr.bits.type = type;
    attr.bits.descriptor_privilege_level = privilege_level;
    attr.bits.present = present;

    return attr;
};

void registerIDTEntry(InterruptVector::Number vector,
                      IDTEntryTypeAttribute attr, uint64_t offset,
                      uint16_t segment_selector);

class InterruptVector {
   public:
    enum Number {
        kLocalAPICTimer = 0x40,
    };
};

struct InterruptFrame {
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
};

void notifyEOI();

void initializeInterruptConfig();

void LoadIDT();