#include "interrupt.hpp"

#include "console.hpp"

std::array<IDTEntry, kIDTEntryNum> idt;

void registerIDTEntry(int vector, IDTEntryTypeAttribute attr, uint64_t offset,
                      uint16_t segment_selector) {
    idt[vector].attr = attr;
    idt[vector].offset_low = offset & 0xffffu;
    idt[vector].offset_mid = (offset >> 16) & 0xffffu;
    idt[vector].offset_hi = offset >> 32;
    idt[vector].segment_selector = segment_selector;
}

void notifyEOI() {
    volatile auto eoi = reinterpret_cast<uint32_t*>(0xfee000b0);
    *eoi = 0;
}

void loadIDT() {
    uint16_t size = sizeof(idt) - 1;
    uint64_t address = reinterpret_cast<uintptr_t>(&idt[0]);

    struct {
        uint16_t limit;
        uint64_t base;
    } __attribute__((packed)) idtr = {size, address};

    __asm__("lidt %0" : : "m"(idtr));
}

namespace {
__attribute__((interrupt)) void InterruptHandlerLocalAPICTimer(
    InterruptFrame* frame) {
    printj("local_APIC_timer timed out\n");
    notifyEOI();
}
}  // namespace

void initializeInterruptConfig() {
    uint16_t current_cs;
    __asm__("movw %%cs, %0" : "=r"(current_cs));

    registerIDTEntry(InterruptVector::kLocalAPICTimer,
                     MakeIDTAttr(DescriptorType::kInterruptGate, 0),
                     reinterpret_cast<uint64_t>(InterruptHandlerLocalAPICTimer),
                     current_cs);

    loadIDT();
    __asm__("sti");
}