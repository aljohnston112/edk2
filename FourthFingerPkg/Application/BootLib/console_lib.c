#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include "console_lib.h"
#include "status_lib.h"

EFI_STATUS clear_console(){
    EFI_STATUS status = gST->ConOut->Reset(
    gST->ConOut,
    TRUE
    );
    RETURN_IF_NOT_SUCCESS(
        status,
        "The text output device is not functioning correctly and could not be reset."
    );
    return EFI_SUCCESS;
}