#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include  "console_lib.h"
#include "system_table_lib.h"
#include "memory_lib/memory_lib.h"
#include "status_lib.h"
#include "text_input_lib.h"
#include "time_lib.h"

EFI_STATUS main_entry(
    EFI_HANDLE imageHandle
) {
    EFI_STATUS status = clear_console();
    RETURN_IF_NOT_SUCCESS(
        status,
        "Failed to clear console"
    );

    Print(L"Hello World!\n\r");

    // status = print_input_text_handle_names(imageHandle);
    // status = print_efi_system_table();
    // PrintEfiTime(),
    status = print_memory_map();

    return status;
}

EFI_STATUS
EFIAPI
UefiMain(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE* SystemTable
) {
    EFI_STATUS status = main_entry(ImageHandle);
    RETURN_IF_NOT_SUCCESS(
        status,
        "Main failed"
    );

    clear_console();

    Print(L"\n\rPress enter to exit\n\r");

    status = wait_for_unicode(UNICODE_ENTER);
    RETURN_IF_NOT_SUCCESS(
        status,
        "Failed to wait for enter"
    );

    gRT->ResetSystem(
        EfiResetShutdown,
        EFI_SUCCESS,
        0,
        NULL
    );

    return EFI_SUCCESS;
}
