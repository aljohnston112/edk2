#include <stdbool.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include "system_table_lib.h"
#include "memory_lib/memory_lib.h"
#include "memory_lib/efi_memory_type.h"
#include "status_lib.h"
#include "time_lib.h"

EFI_STATUS main_entry() {
    RETURN_IF_NOT_SUCCESS(
        gST->ConOut->Reset(
            gST->ConOut,
            true
        ),
        "The text output device is not functioning correctly and could not be reset."
    );

    RETURN_IF_NOT_SUCCESS(
        gST->ConOut->OutputString (
            gST->ConOut,
            L"Hello World!\n\r"
        ),
        "Printing failed"
    );

    RETURN_IF_NOT_SUCCESS(
        PrintEfiSystemTable(),
        // PrintEfiTime(),
        // print_memory_map(),
        "Printing time failed"
    );
    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
UefiMain(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE* SystemTable
) {
    EFI_STATUS status = main_entry();
    while (true) {}
    return status;
}
