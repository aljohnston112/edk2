#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include "efi_memory_type.h"
#include "memory_lib.h"

#include <console_lib.h>

#include "text_input_lib.h"

#include "status_lib.h"

static const EFI_MEMORY_TYPE letter_to_memory_type[] = {
    [0] = EfiReservedMemoryType, // 'a'
    [1] = EfiLoaderCode, // 'b'
    [2] = EfiLoaderData, // 'c'
    [3] = EfiBootServicesCode, // 'd'
    [4] = EfiBootServicesData, // 'e'
    [5] = EfiRuntimeServicesCode, // 'f'
    [6] = EfiRuntimeServicesData, // 'g'
    [7] = EfiConventionalMemory, // 'h'
    [8] = EfiUnusableMemory, // 'i'
    [9] = EfiACPIReclaimMemory, // 'j'
    [10] = EfiACPIMemoryNVS, // 'k'
    [11] = EfiMemoryMappedIO, // 'l'
    [12] = EfiMemoryMappedIOPortSpace, // 'm'
    [13] = EfiPalCode, // 'n'
    [14] = EfiPersistentMemory, // 'o'
    [15] = EfiUnacceptedMemoryType // 'p'
};

static const char *efi_memory_type_strings[EfiMaxMemoryType] = {
    [EfiReservedMemoryType] = "a. Reserved Memory",
    [EfiLoaderCode] = "b. Loader Code",
    [EfiLoaderData] = "c. Loader Data",
    [EfiBootServicesCode] = "d. Boot Services Code",
    [EfiBootServicesData] = "e. Boot Services Data",
    [EfiRuntimeServicesCode] = "f. Runtime Services Code",
    [EfiRuntimeServicesData] = "g. Runtime Services Data",
    [EfiConventionalMemory] = "h. Conventional Memory",
    [EfiUnusableMemory] = "i. Unusable Memory",
    [EfiACPIReclaimMemory] = "j. ACPI Reclaim Memory",
    [EfiACPIMemoryNVS] = "k. ACPI NVS Memory",
    [EfiMemoryMappedIO] = "l. Memory Mapped IO",
    [EfiMemoryMappedIOPortSpace] = "m. Memory Mapped IO Port Space",
    [EfiPalCode] = "n. PAL Code",
    [EfiPersistentMemory] = "o. Persistent Memory",
    [EfiUnacceptedMemoryType] = "p. Unaccepted Memory"
};

void add_memory_descriptor(
    efi_memory_descriptor_list *memory_descriptor_list,
    EFI_MEMORY_DESCRIPTOR *memory_descriptor
);

void print_memory_type(const EFI_MEMORY_TYPE memoryType) {
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

void print_memory_attributes(const UINT64 memoryAttributes) {
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

EFI_STATUS page_through_memory_map(efi_memory_descriptor_list memory_descriptor_list) {
    __label__ error;

    EFI_STATUS status = EFI_SUCCESS;
    char *error_string = "Success";

    void on_error(char *on_error) {
        error_string = on_error;
        if (EFI_ERROR(status)) {
            goto error;
        }
    }

    clear_console();

    UINTN number_of_pages = memory_descriptor_list.size;
    if (number_of_pages == 0) {
        AsciiPrint("No Pages; Press Enter to Return");
        status = wait_for_unicode(UNICODE_ENTER);
        on_error(
            "Failed to wait for key"
        );
    } else {
        UINTN current_page = 0;
        BOOLEAN user_done = FALSE;
        EFI_INPUT_KEY key;
        while (user_done == FALSE) {
            BOOLEAN valid_key = FALSE;
            EFI_MEMORY_DESCRIPTOR* memory_descriptor = memory_descriptor_list.memory_descriptors[current_page];
            clear_console();
            AsciiPrint("%u out of %u\n", current_page + 1, number_of_pages);
            print_memory_type(memory_descriptor->Type);
            AsciiPrint("Physical Start Address: 0x%X\n", memory_descriptor->PhysicalStart);
            AsciiPrint("Virtual Start Address: 0x%X\n", memory_descriptor->VirtualStart);
            AsciiPrint("Number of Pages: %u\n", memory_descriptor->NumberOfPages);
            print_memory_attributes(memory_descriptor->Attribute);
            AsciiPrint("\nPress Enter to Go Back\n");

            while (valid_key == FALSE) {
                status = wait_for_key_and_get_it(&key);
                on_error(
                    "Failed to wait for key"
                );

                if (key.ScanCode == UEFI_KEY_LEFT ||
                    key.ScanCode == UEFI_KEY_RIGHT ||
                    key.UnicodeChar == UNICODE_ENTER
                ) {
                    valid_key = TRUE;
                }
            }

            if (key.ScanCode == UEFI_KEY_LEFT) {
                if (current_page == 0) {
                    current_page = number_of_pages - 1;
                } else {
                    current_page--;
                }
            } else if (key.ScanCode == UEFI_KEY_RIGHT) {
                if (current_page == number_of_pages - 1) {
                    current_page = 0;
                } else {
                    current_page++;
                }
            } else if (key.UnicodeChar == UNICODE_ENTER) {
                user_done = TRUE;
            } else {
                on_error("Bug in page_through_memory_map");
            }
        }
    }

error:
    RETURN_IF_NOT_SUCCESS(
        status,
        error_string
    );
    return status;
}

EFI_STATUS print_memory_map() {
    __label__ error;

    EFI_STATUS status = EFI_SUCCESS;
    char *error_string = "Success";

    EFI_MEMORY_DESCRIPTOR *memory_map = NULL;
    efi_memory_descriptor_list memory_descriptor_lists[EfiMaxMemoryType];

    void free() {
        const EFI_STATUS original_status = status;
        if (memory_map != NULL) {
            status = gBS->FreePool(memory_map);
            if (EFI_ERROR(status)) {
                AsciiPrint("Failed to free memory_map");
            }
        }
        for (UINTN i = 0; i < EfiMaxMemoryType; ++i) {
            if (memory_descriptor_lists[i].memory_descriptors != NULL) {
                status = gBS->FreePool(memory_descriptor_lists[i].memory_descriptors);
                if (EFI_ERROR(status)) {
                    AsciiPrint("Failed to free memory descriptor");
                }
            }
        }
        status = original_status;
        goto error;
    }

    void free_if_error(char *on_error) {
        if (EFI_ERROR(status)) {
            error_string = on_error;
            free();
        }
    }

    // Allocate for and load the memory map
    UINTN memory_map_size = 0;
    UINTN map_key;
    UINTN descriptor_size;
    UINT32 descriptor_version;

    status = gBS->GetMemoryMap(
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
        status = check_allocate_pool_status(
            gBS->AllocatePool(
                EfiLoaderData,
                memory_map_size,
                (VOID **) &memory_map
            )
        );
        RETURN_IF_NOT_SUCCESS(
            status,
            "Unknown error allocating pool for memory map"
        );

        status = gBS->GetMemoryMap(
            &memory_map_size,
            memory_map,
            &map_key,
            &descriptor_size,
            &descriptor_version
        );
        // Out of resources is expected here
        if(status == EFI_INVALID_PARAMETER) {
            free_if_error("Memory map error");
        }
    }

    if (memory_map != NULL) {
        // Count the number of each memory type
        UINT64 memory_counts[EfiMaxMemoryType] = {0};
        const UINTN memory_map_entry_count = memory_map_size / descriptor_size;
        EFI_MEMORY_DESCRIPTOR *current_descriptor = memory_map;
        for (UINTN index = 0; index < memory_map_entry_count; index++) {
            const EFI_MEMORY_TYPE type = current_descriptor->Type;
            memory_counts[type]++;
            current_descriptor = NEXT_MEMORY_DESCRIPTOR(current_descriptor, descriptor_size);
        }

        // Allocate room for the descriptors in efi_memory_descriptors
        for (UINTN i = 0; i < EfiMaxMemoryType; ++i) {
            status = gBS->AllocatePool(
                EfiLoaderData,
                memory_counts[i] * sizeof(EFI_MEMORY_DESCRIPTOR *),
                (VOID **) &memory_descriptor_lists[i].memory_descriptors
            );
            free_if_error("Failed to allocate for memory descriptor");
            memory_descriptor_lists[i].size = 0;
        }

        // Put the memory descriptors efi_memory_descriptors
        current_descriptor = memory_map;
        for (UINTN index = 0; index < memory_map_entry_count; index++) {
            EFI_MEMORY_TYPE type = current_descriptor->Type;
            add_memory_descriptor(
                &memory_descriptor_lists[type],
                current_descriptor
            );
            current_descriptor = NEXT_MEMORY_DESCRIPTOR(current_descriptor, descriptor_size);
        }
    } else {
        status = EFI_ABORTED;
        free_if_error("Memory map was null after allocation");
    }

    BOOLEAN user_done = FALSE;
    while (user_done == FALSE) {
        clear_console();

        // Display memory type options
        for (UINTN i = 0; i < EfiMaxMemoryType; ++i) {
            AsciiPrint(efi_memory_type_strings[i]);
            AsciiPrint("\n");
        }
        AsciiPrint("\nPress Enter to Go Back\n");

        BOOLEAN valid_key = FALSE;
        EFI_INPUT_KEY key;
        while (valid_key == FALSE) {
            status = wait_for_key_and_get_it(&key);
            free_if_error("Failed to wait for key");

            if (
                (key.UnicodeChar >= 'a' &&
                 key.UnicodeChar <= 'p') ||
                key.UnicodeChar == UNICODE_ENTER
            ) {
                valid_key = TRUE;
            }
        }

        if (key.UnicodeChar != UNICODE_ENTER) {
            status = page_through_memory_map(
                memory_descriptor_lists[
                    letter_to_memory_type[key.UnicodeChar - 'a']
                ]
            );
            free_if_error("Problem paging through memory map");
        } else {
            user_done = TRUE;
        }

    }

    status = EFI_SUCCESS;
    free();
error:
    RETURN_IF_NOT_SUCCESS(status, error_string);
    return status;
}
