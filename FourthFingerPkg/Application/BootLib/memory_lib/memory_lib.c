#include <Library/UefiBootServicesTableLib.h>

#include "memory_lib.h"
#include "status_lib.h"

void print_all_memory_types(const EFI_MEMORY_TYPE memoryType) {
    switch (memoryType) {
        case EfiReservedMemoryType:
            AsciiPrint("Memory type: EfiReservedMemoryType\n");
        break;
        case EfiLoaderCode:
            AsciiPrint("Memory type: EfiLoaderCode\n");
        break;
        case EfiLoaderData:
            AsciiPrint("Memory type: EfiLoaderData\n");
        break;
        case EfiBootServicesCode:
            AsciiPrint("Memory type: EfiBootServicesCode\n");
        break;
        case EfiBootServicesData:
            AsciiPrint("Memory type: EfiBootServicesData\n");
        break;
        case EfiRuntimeServicesCode:
            AsciiPrint("Memory type: EfiRuntimeServicesCode\n");
        break;
        case EfiRuntimeServicesData:
            AsciiPrint("Memory type: EfiRuntimeServicesData\n");
        break;
        case EfiConventionalMemory:
            AsciiPrint("Memory type: EfiConventionalMemory\n");
        break;
        case EfiUnusableMemory:
            AsciiPrint("Memory type: EfiUnusableMemory\n");
        break;
        case EfiACPIReclaimMemory:
            AsciiPrint("Memory type: EfiACPIReclaimMemory\n");
        break;
        case EfiACPIMemoryNVS:
            AsciiPrint("Memory type: EfiACPIMemoryNVS\n");
        break;
        case EfiMemoryMappedIO:
            AsciiPrint("Memory type: EfiMemoryMappedIO\n");
        break;
        case EfiMemoryMappedIOPortSpace:
            AsciiPrint("Memory type: EfiMemoryMappedIOPortSpace\n");
        break;
        case EfiPalCode:
            AsciiPrint("Memory type: EfiPalCode\n");
        break;
        case EfiPersistentMemory:
            AsciiPrint("Memory type: EfiPersistentMemory\n");
        break;
        default:
            AsciiPrint("Unknown memory type\n");
        break;
    }
}

void check_memory_attributes(const UINT64 memoryAttributes) {
    if (memoryAttributes & EFI_MEMORY_UC) {
        AsciiPrint("Memory attribute: EFI_MEMORY_UC\n");
    }
    if (memoryAttributes & EFI_MEMORY_WC) {
        AsciiPrint("Memory attribute: EFI_MEMORY_WC\n");
    }
    if (memoryAttributes & EFI_MEMORY_WT) {
        AsciiPrint("Memory attribute: EFI_MEMORY_WT\n");
    }
    if (memoryAttributes & EFI_MEMORY_WB) {
        AsciiPrint("Memory attribute: EFI_MEMORY_WB\n");
    }
    if (memoryAttributes & EFI_MEMORY_UCE) {
        AsciiPrint("Memory attribute: EFI_MEMORY_UCE\n");
    }
    if (memoryAttributes & EFI_MEMORY_WP) {
        AsciiPrint("Memory attribute: EFI_MEMORY_WP\n");
    }
    if (memoryAttributes & EFI_MEMORY_RP) {
        AsciiPrint("Memory attribute: EFI_MEMORY_RP\n");
    }
    if (memoryAttributes & EFI_MEMORY_XP) {
        AsciiPrint("Memory attribute: EFI_MEMORY_XP\n");
    }
    if (memoryAttributes & EFI_MEMORY_NV) {
        AsciiPrint("Memory attribute: EFI_MEMORY_NV\n");
    }
    if (memoryAttributes & EFI_MEMORY_MORE_RELIABLE) {
        AsciiPrint("Memory attribute: EFI_MEMORY_MORE_RELIABLE\n");
    }
    if (memoryAttributes & EFI_MEMORY_RO) {
        AsciiPrint("Memory attribute: EFI_MEMORY_RO\n");
    }
    if (memoryAttributes & EFI_MEMORY_RUNTIME) {
        AsciiPrint("Memory attribute: EFI_MEMORY_RUNTIME\n");
    }
}

EFI_STATUS print_memory_map() {
    UINTN memory_map_size = 0;
    EFI_MEMORY_DESCRIPTOR* memory_map = NULL;
    UINTN map_key;
    UINTN descriptor_size;
    UINT32 descriptor_version;

    // Allocate and load the memory map
    EFI_STATUS status = gBS->GetMemoryMap(
        &memory_map_size,
        memory_map,
        &map_key,
        &descriptor_size,
        &descriptor_version
    );
    RETURN_IF_INVALID_PARAMETER_STATUS(
        status,
        "Memory map error"
    );

    while (status == EFI_BUFFER_TOO_SMALL) {
        RETURN_IF_NOT_SUCCESS(
            check_allocate_pool_status(
                gBS->AllocatePool(
                    EfiLoaderData,
                    memory_map_size,
                    (VOID**)&memory_map
                )
            ),
            "Unknown error allocating pool for memory map"
        );

        status = gBS->GetMemoryMap(
            &memory_map_size,
            memory_map,
            &map_key,
            &descriptor_size,
            &descriptor_version
        );
        if (status == EFI_INVALID_PARAMETER) {
            gBS->FreePool(memory_map);
            RETURN_IF_NOT_SUCCESS(
                status,
                "Memory map error"
            );
        }
    }

    if (memory_map != NULL) {
        const UINTN memory_map_entry_count = memory_map_size / descriptor_size;
        for (UINTN index = 0; index < memory_map_entry_count; index++) {
            const EFI_MEMORY_DESCRIPTOR current_descriptor = memory_map[index];
            const EFI_MEMORY_TYPE type = current_descriptor.Type;
            print_all_memory_types(type);
        }
    }
    else {
        RETURN_IF_NOT_SUCCESS(
            EFI_ABORTED,
            "Memory map was null after allocation"
        );
    }

    return EFI_SUCCESS;
}
