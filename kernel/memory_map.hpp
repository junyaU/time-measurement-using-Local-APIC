#pragma once

#include <stdint.h>

#include <cstdint>

struct MemoryMap {
    uint64_t buffer_size;
    void *buffer;
    uint64_t map_size;
    uint64_t map_key;
    uint64_t descriptor_size;
    uint32_t descriptor_version;
};

struct MemoryDescriptor {
    uint32_t type;
    uintptr_t physical_start;
    uintptr_t virtual_start;
    uint64_t number_of_pages;
    uint64_t attribute;
};

enum class MemoryType {
    kEfiReservedMemoryType,
    kEfiLoaderCode,        // UEFIアプリの実行コード
    kEfiLoaderData,        // UEFIアプリのデータ
    kEfiBootServicesCode,  // ブートサービスドライバの実行コード
    kEfiBootServicesData,  // ブートサービスドライバのでーた
    kEfiRuntimeServicesCode,
    kEfiRuntimeServicesData,
    kEfiConventionalMemory,
    kEfiUnusableMemory,
    kEfiACPIReclaimMemory,
    kEfiACPIMemoryNVS,
    kEfiMemoryMappedIO,
    kEfiMemoryMappedIOPortSpace,
    kEfiPalCode,
    kEfiPersistentMemory,
    kEfiMaxMemoryType
};
