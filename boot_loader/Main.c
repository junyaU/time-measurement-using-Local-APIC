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

#include "memory_map.hpp"

void Halt()
{
    while (1)
    {
        __asm__("hlt");
    }
}

EFI_STATUS GetMemoryMap(struct MemoryMap *memory_map)
{
    if (memory_map->buffer == NULL)
    {
        return EFI_BUFFER_TOO_SMALL;
    }

    memory_map->map_size = memory_map->buffer_size;

    return gBS->GetMemoryMap(
        &memory_map->map_size,
        (EFI_MEMORY_DESCRIPTOR *)memory_map->buffer,
        &memory_map->map_key,
        &memory_map->descriptor_size,
        &memory_map->descriptor_version);
}

EFI_STATUS OpenRootDir(EFI_HANDLE image_handle, EFI_FILE_PROTOCOL **root)
{
    EFI_LOADED_IMAGE_PROTOCOL *loaded_image;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs;

    gBS->OpenProtocol(
        image_handle,
        &gEfiLoadedImageProtocolGuid,
        (VOID **)&loaded_image,
        image_handle,
        NULL,
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

    gBS->OpenProtocol(
        loaded_image->DeviceHandle,
        &gEfiSimpleFileSystemProtocolGuid,
        (VOID **)&fs,
        image_handle,
        NULL,
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

    fs->OpenVolume(fs, root);

    return EFI_SUCCESS;
}

// Accessing Graphics Board Functions
EFI_STATUS OpenGOP(EFI_HANDLE image_handle, EFI_GRAPHICS_OUTPUT_PROTOCOL **gop)
{
    EFI_STATUS Status;
    UINTN num_gop_handles = 0;
    EFI_HANDLE *gop_handles = NULL;

    Status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiGraphicsOutputProtocolGuid,
        NULL,
        &num_gop_handles,
        &gop_handles);
    if (EFI_ERROR(Status))
    {
        return Status;
    }

    Status = gBS->OpenProtocol(
        gop_handles[0],
        &gEfiGraphicsOutputProtocolGuid,
        (VOID **)gop,
        image_handle,
        NULL,
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
    if (EFI_ERROR(Status))
    {
        return Status;
    }

    FreePool(gop_handles);

    return EFI_SUCCESS;
}

EFI_STATUS EFIAPI UefiMain(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table)
{
    Print(L"hello!");

    CHAR8 memory_map_buffer[4096 * 4];
    struct MemoryMap memory_map = {
        sizeof(memory_map_buffer), memory_map_buffer, 0, 0, 0, 0};
    if (EFI_ERROR(GetMemoryMap(&memory_map)))
    {
        Print(L"failed to get memory map\n");
        Halt();
    }

    EFI_FILE_PROTOCOL *root_dir;
    if (EFI_ERROR(OpenRootDir(image_handle, &root_dir)))
    {
        Print(L"failed to open root directory\n");
        Halt();
    }

    EFI_GRAPHICS_OUTPUT_PROTOCOL *
        gop;
    if (EFI_ERROR(OpenGOP(image_handle, &gop)))
    {
        Print(L"failed to open GOP\n");
        Halt();
    }

    Print(L"nice");

    while (1)
        ;

    return EFI_SUCCESS;
}
