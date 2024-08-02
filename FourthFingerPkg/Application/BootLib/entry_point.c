#include <stdbool.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include "system_table_lib.h"
#include "memory_lib/memory_lib.h"
#include "memory_lib/efi_memory_type.h"
#include "status_lib.h"
#include "text_input_lib.h"
#include "time_lib.h"

EFI_STATUS main_entry(
    EFI_HANDLE imageHandle
) {
    EFI_STATUS status = gST->ConOut->Reset(
            gST->ConOut,
            true
        );
    RETURN_IF_NOT_SUCCESS(
        status,
        "The text output device is not functioning correctly and could not be reset."
    );

    status = gST->ConOut->OutputString (
            gST->ConOut,
            L"Hello World!\n\r"
        );
    RETURN_IF_NOT_SUCCESS(
        status,
        "Printing failed"
    );

    status = print_input_text_handle_names(imageHandle);
    // PrintEfiSystemTable(),
    // PrintEfiTime(),
    // print_memory_map(),
    RETURN_IF_NOT_SUCCESS(
        status,
        "Main failed"
    );

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
UefiMain(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE* SystemTable
) {
    EFI_STATUS status = main_entry(ImageHandle);
    while (true) {}
    return status;
}
