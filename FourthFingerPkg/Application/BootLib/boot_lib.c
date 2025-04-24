#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include "boot_lib.h"
#include "memory_lib/efi_memory_type.h"
#include "memory_lib/memory_lib.h"

void start_os(
    EFI_HANDLE image_handle
) {
    efi_memory_descriptor_list memory_descriptor_lists[EfiMaxMemoryType];
    UINTN map_key;
    EFI_STATUS status = get_memory_map(
        memory_descriptor_lists,
        &map_key
    );
    if (EFI_ERROR(status)) {
        status = free_memory_descriptor_lists(memory_descriptor_lists);
        if (EFI_ERROR(status)) {
            AsciiPrint("Failed to free all memory descriptor lists");
        }
    }
    gBS->ExitBootServices(
        image_handle,
        map_key
    );

}
