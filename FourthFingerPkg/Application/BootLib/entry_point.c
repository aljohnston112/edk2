#include <handle_lib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include "console_lib.h"
#include "memory_lib/memory_lib.h"
#include "status_lib.h"
#include "system_table_lib.h"
#include "text_input_lib.h"
#include "time_lib.h"

EFI_STATUS main_entry(
    const EFI_HANDLE imageHandle
) {
    BOOLEAN user_done = FALSE;
    while (user_done == FALSE) {
        EFI_STATUS status = clear_console();
        RETURN_IF_NOT_SUCCESS(
            status,
            "Failed to clear console"
        );

        AsciiPrint("Hello, World!\n\n");

        AsciiPrint("a: System Table\n");
        AsciiPrint("b: Time\n");
        AsciiPrint("c: Memory Map\n");
        AsciiPrint("d: Handles\n");
        Print(L"\n\rPress enter to exit\n\r");

        EFI_INPUT_KEY key;
        status = wait_for_key_and_get_it(&key);
        RETURN_IF_NOT_SUCCESS(status, "Failed to get key");
        status = clear_console();
        RETURN_IF_NOT_SUCCESS(status , "Failed to clear console");
        switch (key.UnicodeChar) {
        case 'a':
            status = print_efi_system_table();
            RETURN_IF_NOT_SUCCESS(status, "System table explorer failed");
            break;
        case 'b':
            status = PrintEfiTime();
            RETURN_IF_NOT_SUCCESS(status, "Failed to print time");
            break;
        case 'c':
            status = print_memory_map();
            RETURN_IF_NOT_SUCCESS(status, "Memory map explorer failed");
            break;
        case 'd':
            status = get_all_handles(imageHandle);
            RETURN_IF_NOT_SUCCESS(status, "Handle explorer failed");
            break;
        case UNICODE_ENTER:
            user_done = TRUE;
            break;
        default:
            break;
        }
    }

    return EFI_SUCCESS;
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

    gRT->ResetSystem(
        EfiResetShutdown,
        EFI_SUCCESS,
        0,
        NULL
    );

    return EFI_SUCCESS;
}
