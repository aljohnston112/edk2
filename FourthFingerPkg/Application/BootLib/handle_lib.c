#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include "handle_lib.h"
#include "status_lib.h"

EFI_STATUS free_handle_pool(EFI_HANDLE *handles) {
    EFI_STATUS status = EFI_SUCCESS;
    if (handles != NULL) {
        status = gBS->FreePool(handles);
        if (EFI_ERROR(status)) {
            AsciiPrint("Failed to free handles\n");
        }
    }
    return status;
}

EFI_STATUS check_locate_handle_status(const EFI_STATUS status) {
    RETURN_IF_INVALID_PARAMETER_STATUS(
        status,
        "Locate handle argument error"
    );
    RETURN_IF_NOT_FOUND_STATUS(
        status,
        "Could not find handle with specified protocol"
    );
    RETURN_IF_OUT_OF_RESOURCSE_STATUS(
        status,
        "Could not allocate space for handles with specified protocol"
    );
    return status;
}

EFI_STATUS get_handles(
    EFI_HANDLE **handles,
    UINTN *number_of_handles,
    EFI_GUID *protocol
) {
    *number_of_handles = 0;
    EFI_STATUS status = gBS->LocateHandleBuffer(
        ByProtocol,
        protocol,
        NULL,
        number_of_handles,
        handles
    );
    RETURN_IF_NOT_SUCCESS(
        check_locate_handle_status(status),
        "Error getting handles with specified protocols"
    );

    status = AsciiPrint(
        "Number of handles: %u\n",
        *number_of_handles
    );

    if (EFI_ERROR(status)) {
        AsciiPrint(
             "Failed to print number of handles"
         );
    }
    return EFI_SUCCESS;
}