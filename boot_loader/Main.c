#include <Guid/FileInfo.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DiskIo2.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h>
#include <Uefi.h>

#include "elf.hpp"
#include "frame_buffer_config.hpp"
#include "memory_map.hpp"

void Halt() {
    while (1) {
        __asm__("hlt");
    }
}

EFI_STATUS GetMemoryMap(struct MemoryMap *memory_map) {
    if (memory_map->buffer == NULL) {
        return EFI_BUFFER_TOO_SMALL;
    }

    memory_map->map_size = memory_map->buffer_size;

    return gBS->GetMemoryMap(&memory_map->map_size,
                             (EFI_MEMORY_DESCRIPTOR *)memory_map->buffer,
                             &memory_map->map_key, &memory_map->descriptor_size,
                             &memory_map->descriptor_version);
}

EFI_STATUS OpenRootDir(EFI_HANDLE image_handle, EFI_FILE_PROTOCOL **root) {
    EFI_LOADED_IMAGE_PROTOCOL *loaded_image;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs;

    gBS->OpenProtocol(image_handle, &gEfiLoadedImageProtocolGuid,
                      (VOID **)&loaded_image, image_handle, NULL,
                      EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

    gBS->OpenProtocol(loaded_image->DeviceHandle,
                      &gEfiSimpleFileSystemProtocolGuid, (VOID **)&fs,
                      image_handle, NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

    fs->OpenVolume(fs, root);

    return EFI_SUCCESS;
}

void CalcLoadAddressRange(Elf64_Ehdr *ehdr, UINT64 *first_addr,
                          UINT64 *last_addr) {
    Elf64_Phdr *phdr = (Elf64_Phdr *)((UINT64)ehdr + ehdr->e_phoff);
    *first_addr = MAX_UINT64;
    *last_addr = 0;

    for (Elf64_Half i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type != PT_LOAD) {
            continue;
        }

        *first_addr = MIN(*first_addr, phdr[i].p_vaddr);
        *last_addr = MAX(*last_addr, phdr[i].p_vaddr + phdr[i].p_memsz);
    }
}

void CopyLoadSegments(Elf64_Ehdr *ehdr) {
    Elf64_Phdr *phdr = (Elf64_Phdr *)((UINT64)ehdr + ehdr->e_phoff);
    for (Elf64_Half i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type != PT_LOAD) {
            continue;
        }

        UINT64 load_segment = (UINT64)ehdr + phdr[i].p_offset;
        CopyMem((VOID *)phdr[i].p_vaddr, (VOID *)load_segment,
                phdr[i].p_filesz);

        // bss
        UINTN remain_bytes = phdr[i].p_memsz - phdr[i].p_filesz;
        SetMem((VOID *)(phdr[i].p_vaddr + phdr[i].p_filesz), remain_bytes, 0);
    }
}

// Accessing Graphics Board Functions
EFI_STATUS OpenGOP(EFI_HANDLE image_handle,
                   EFI_GRAPHICS_OUTPUT_PROTOCOL **gop) {
    EFI_STATUS Status;
    UINTN num_gop_handles = 0;
    EFI_HANDLE *gop_handles = NULL;

    Status =
        gBS->LocateHandleBuffer(ByProtocol, &gEfiGraphicsOutputProtocolGuid,
                                NULL, &num_gop_handles, &gop_handles);
    if (EFI_ERROR(Status)) {
        return Status;
    }

    Status = gBS->OpenProtocol(gop_handles[0], &gEfiGraphicsOutputProtocolGuid,
                               (VOID **)gop, image_handle, NULL,
                               EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
    if (EFI_ERROR(Status)) {
        return Status;
    }

    FreePool(gop_handles);

    return EFI_SUCCESS;
}

EFI_STATUS GetFrameBufferConfig(EFI_HANDLE image_handle,
                                struct FrameBufferConfig *config) {
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    EFI_STATUS status = OpenGOP(image_handle, &gop);
    if (EFI_ERROR(status)) {
        Print(L"failed to open GOP\n");
        return status;
    }

    config->frame_buffer = (UINT8 *)gop->Mode->FrameBufferBase;
    config->pixels_per_scan_line = gop->Mode->Info->PixelsPerScanLine;
    config->horizon_resolution = gop->Mode->Info->HorizontalResolution;
    config->vertical_resolution = gop->Mode->Info->VerticalResolution;

    switch (gop->Mode->Info->PixelFormat) {
        case PixelRedGreenBlueReserved8BitPerColor:
            config->pixel_format = kPixelRGB8BitPerColor;
            break;
        case PixelBlueGreenRedReserved8BitPerColor:
            config->pixel_format = kPixelBGR8BitPerColor;
            break;
        default:
            Print(L"Unimplemented pixel format: %d\n",
                  gop->Mode->Info->PixelFormat);
            return EFI_UNSUPPORTED;
    }

    return EFI_SUCCESS;
}

EFI_STATUS LoadKernel(EFI_HANDLE image_handle, UINT64 *kernel_buffer) {
    EFI_FILE_PROTOCOL *root_dir;
    EFI_STATUS status;

    status = OpenRootDir(image_handle, &root_dir);
    if (EFI_ERROR(status)) {
        Print(L"failed to open root directory\n");
        return status;
    }

    EFI_FILE_PROTOCOL *kernel;
    status = root_dir->Open(root_dir, &kernel, L"\\kernel.elf",
                            EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(status)) {
        Print(L"failed to open kernel\n");
        return status;
    }

    UINTN kernel_info_size = sizeof(EFI_FILE_INFO) + sizeof(CHAR16) * 12;
    UINT8 kernel_info_buffer[kernel_info_size];
    status = kernel->GetInfo(kernel, &gEfiFileInfoGuid, &kernel_info_size,
                             kernel_info_buffer);
    if (EFI_ERROR(status)) {
        Print(L"failed to get kernel info\n");
        return status;
    }

    EFI_FILE_INFO *kernel_info = (EFI_FILE_INFO *)kernel_info_buffer;
    UINTN kernel_file_size = kernel_info->FileSize;
    VOID *kernel_buf;
    status = gBS->AllocatePool(EfiLoaderData, kernel_file_size, &kernel_buf);
    if (EFI_ERROR(status)) {
        Print(L"failed to allocate pool\n");
        return status;
    }

    status = kernel->Read(kernel, &kernel_file_size, kernel_buf);
    if (EFI_ERROR(status)) {
        Print(L"error\n");
        return status;
    }

    Elf64_Ehdr *kernel_ehdr = (Elf64_Ehdr *)kernel_buf;
    UINT64 kernel_end_addr;
    CalcLoadAddressRange(kernel_ehdr, kernel_buffer, &kernel_end_addr);

    UINTN num_pages = (kernel_end_addr - *kernel_buffer + 0xfff) / 0x1000;
    status = gBS->AllocatePages(AllocateAddress, EfiLoaderData, num_pages,
                                kernel_buffer);
    if (EFI_ERROR(status)) {
        Print(L"failed to allocate pages\n");
        return status;
    }

    CopyLoadSegments(kernel_ehdr);

    status = gBS->FreePool(kernel_buf);
    if (EFI_ERROR(status)) {
        Print(L"failed to free pool\n");
        return status;
    }

    return EFI_SUCCESS;
}

EFI_STATUS GetRSDP(EFI_SYSTEM_TABLE *system_table, VOID **rsdp) {
    for (UINTN i = 0; i < system_table->NumberOfTableEntries; ++i) {
        if (CompareGuid(&gEfiAcpiTableGuid,
                        &system_table->ConfigurationTable[i].VendorGuid)) {
            *rsdp = system_table->ConfigurationTable[i].VendorTable;
            break;
        }
    }

    if (*rsdp == NULL) {
        Print(L"could not retrieve RSDP\n");
        return EFI_UNSUPPORTED;
    }

    return EFI_SUCCESS;
}

EFI_STATUS EFIAPI uefi_main(EFI_HANDLE image_handle,
                            EFI_SYSTEM_TABLE *system_table) {
    UINT64 kernel_buffer;
    if (EFI_ERROR(LoadKernel(image_handle, &kernel_buffer))) {
        Halt();
    }

    struct FrameBufferConfig frame_buffer_config = {NULL, 0, 0, 0, 0};
    if (EFI_ERROR(GetFrameBufferConfig(image_handle, &frame_buffer_config))) {
        Halt();
    }

    CHAR8 memory_map_buffer[4096 * 4];
    struct MemoryMap memory_map = {
        sizeof(memory_map_buffer), memory_map_buffer, 0, 0, 0, 0};
    if (EFI_ERROR(GetMemoryMap(&memory_map))) {
        Print(L"failed to get memory map\n");
        Halt();
    }

    if (EFI_ERROR(gBS->ExitBootServices(image_handle, memory_map.map_key))) {
        Print(L"could not exit boot service\n");
        Halt();
    }

    VOID *rsdp = NULL;
    if (EFI_ERROR(GetRSDP(system_table, &rsdp))) {
        Halt();
    }

    EFI_PHYSICAL_ADDRESS kernel_entry_offset = 24;
    UINT64 kernel_entry_addr = *(UINT64 *)(kernel_buffer + kernel_entry_offset);

    typedef void __attribute__((sysv_abi))
    KernelEntry(const struct FrameBufferConfig *, const struct MemoryMap *,
                const VOID *);
    ((KernelEntry *)kernel_entry_addr)(&frame_buffer_config, &memory_map, rsdp);

    while (1)
        ;

    return EFI_SUCCESS;
}
