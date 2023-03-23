#include "acpi.hpp"

#include <cstdlib>
#include <cstring>

namespace {
template <typename T>
uint8_t SumBytes(const T* data, size_t bytes) {
    return SumBytes(reinterpret_cast<const uint8_t*>(data), bytes);
}

template <>
uint8_t SumBytes<uint8_t>(const uint8_t* data, size_t bytes) {
    uint8_t sum = 0;
    for (size_t i = 0; i < bytes; ++i) {
        sum += data[i];
    }

    return sum;
}
}  // namespace

namespace acpi {
bool RSDP::isValid() const {
    const auto kRSDPSignaturePrefix = "RSD PTR ";
    if (strncmp(this->signature, kRSDPSignaturePrefix, 8) != 0) {
        return false;
    }

    if (this->revision != 2) {
        return false;
    }

    const auto kRevision1Bytes = 20;
    const auto kRevision2Bytes = 36;

    if (auto sum = SumBytes(this, kRevision1Bytes); sum != 0) {
        return false;
    }

    if (auto sum = SumBytes(this, kRevision2Bytes); sum != 0) {
        return false;
    }

    return true;
}

bool SDTHeader::isValid(const char* signature) const {
    if (strncmp(this->signature, signature, 4) != 0) {
        return false;
    }

    if (auto sum = SumBytes(this, this->length); sum != 0) {
        return false;
    }

    return true;
}

const SDTHeader& XSDT::operator[](size_t i) const {
    auto sdt_entries = reinterpret_cast<const uint64_t*>(&this->header + 1);
    return *reinterpret_cast<const SDTHeader*>(sdt_entries[i]);
}

size_t XSDT::countSDTEntries() const {
    return (this->header.length - sizeof(SDTHeader)) / sizeof(uint64_t);
}

const FADT* fadt;

void Initialize(const RSDP& rsdp) {
    if (!rsdp.isValid()) {
        exit(1);
    }

    const acpi::XSDT& xsdt =
        *reinterpret_cast<const acpi::XSDT*>(rsdp.xsdt_addr);

    if (!xsdt.header.isValid("XSDT")) {
        exit(1);
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
        exit(1);
    }
}

void waitMilliSec(unsigned long msec) {}
}  // namespace acpi
