#include <stdbool.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/LoadedImage.h>

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

    status = gST->ConOut->OutputString(
        gST->ConOut,
        L"Hello World!\n\r"
    );
    RETURN_IF_NOT_SUCCESS(
        status,
        "Printing failed"
    );

    // status = print_input_text_handle_names(imageHandle);
    status = print_efi_system_table();
    // PrintEfiTime(),
    // print_memory_map(),

    return status;
}

EFI_STATUS
EFIAPI
UefiMain(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
) {
    EFI_STATUS status = main_entry(ImageHandle);
    RETURN_IF_NOT_SUCCESS(
        status,
        "Main failed"
    );
    
    status = gST->ConIn->Reset(
        gST->ConIn,
        FALSE
    );
    RETURN_IF_NOT_SUCCESS(
        status,
        "Failed to reset console input"
    );

    status = gST->ConOut->OutputString(
        gST->ConOut,
        L"\n\rPress a key to exit\n\r"
    );
    RETURN_IF_NOT_SUCCESS(
        status,
        "Failed to print waiting message"
    );

    UINTN index;
    status = gBS->WaitForEvent(
        1,
        &gST->ConIn->WaitForKey,
        &index
    );
    RETURN_IF_NOT_SUCCESS(
        status,
        "Failed to wait for key"
    );

    gRT->ResetSystem(
        EfiResetShutdown,
        EFI_SUCCESS,
        0,
        NULL
    );

    return EFI_SUCCESS;
}
