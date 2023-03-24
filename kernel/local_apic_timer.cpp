#include "local_apic_timer.hpp"

#include <cstdlib>
#include <cstring>

#include "acpi.hpp"
#include "console.hpp"
#include "interrupt.hpp"

namespace {
// register for setting operating mode and interrupts
volatile uint32_t& lvt_timer = *reinterpret_cast<uint32_t*>(0xfee00320);

// register to set the initial value of the counter
volatile uint32_t& initial_count = *reinterpret_cast<uint32_t*>(0xfee00380);

// register in which the current value of the counter is stored
volatile uint32_t& current_count = *reinterpret_cast<uint32_t*>(0xfee00390);

// register for divider ratio setting
volatile uint32_t& divide_conf = *reinterpret_cast<uint32_t*>(0xfee003e0);

const uint32_t kCountMax = 0xffffffffu;
}  // namespace

void initializeLocalAPICTimer() {
    // 1:1
    divide_conf = 0b1011;
    lvt_timer = (0b001 << 16);

    startTimer();
    acpi::waitMilliSec(100);
    const auto elapsed = kCountMax - current_count;
    stopTimer();

    unsigned long local_apic_timer_frequency =
        static_cast<unsigned long>(elapsed * 10);

    divide_conf = 0b1011;
    lvt_timer = (0b010 << 16) | InterruptVector::kLocalAPICTimer;
    initial_count = local_apic_timer_frequency;

    printj("aaaaaaa");
}

void startTimer() { initial_count = kCountMax; }

void stopTimer() { initial_count = 0; }
