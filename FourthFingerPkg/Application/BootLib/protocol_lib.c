#include <stdbool.h>

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include "handle_lib.h"
#include "protocol_lib.h"

#include <text_input_lib.h>

#include "status_lib.h"

EFI_STATUS close_protocols_and_free_pool(
    EFI_HANDLE* handles,
    UINTN number_of_protocols,
    EFI_GUID* protocolGuid,
    EFI_HANDLE imageHandle,
    VOID** protocols
) {
    EFI_STATUS status = EFI_SUCCESS;
    for (UINTN i = 0; i < number_of_protocols; i++) {
        status = gBS->CloseProtocol(
            handles[i],
            protocolGuid,
            imageHandle,
            NULL
        );
        if (EFI_ERROR(status)) {
            AsciiPrint("Failed to close protocol\n");
        }
    }
    if (*protocols != NULL) {
        status = gBS->FreePool(*protocols);
        if (EFI_ERROR(status)) {
            AsciiPrint("Failed to free protocol pool\n");
        }
    }
    return status;
}

EFI_STATUS get_protocol_from_handles(
    const EFI_HANDLE imageHandle,
    EFI_HANDLE** handles,
    VOID** protocols,
    UINTN* number_of_handles,
    UINTN* number_of_protocols,
    EFI_GUID* protocolGuid
) {
    bool userSuppliedHandles = true;

#define FREE(status, string_on_error) \
    EFI_STATUS original_status = status; \
    do { \
        status = close_protocols_and_free_pool( \
            *handles, \
            *number_of_protocols, \
            protocolGuid, \
            imageHandle, \
            protocols \
        ); \
        *number_of_protocols = 0; \
        if(!userSuppliedHandles){ \
            status = free_handle_pool(*handles); \
            *number_of_handles = 0; \
        } \
        RETURN_IF_NOT_SUCCESS( \
            original_status, \
            string_on_error \
        ); \
        return EFI_SUCCESS; \
    } while(0)

    if (*handles == NULL) {
        userSuppliedHandles = false;
        *number_of_handles = 0;
        EFI_STATUS status = get_handles(
            handles,
            number_of_handles,
            protocolGuid
        );
        RETURN_IF_NOT_SUCCESS(
            status,
            "Failed to get handles"
        );
        if (*handles == NULL) {
            RETURN_IF_NOT_SUCCESS(
                EFI_ABORTED,
                "Handles were null"
            );
        }
    }

    EFI_STATUS status = gBS->AllocatePool(
        EfiLoaderData,
        *number_of_handles * sizeof(*protocols),
        protocols
    );
    if (EFI_ERROR(status)) {
        status = check_allocate_pool_status(status);
        FREE(
            status,
            "Error allocating pool for text input handles"
        );
    } else {
        for (UINTN i = 0; i < *number_of_handles; i++) {
            status = gBS->OpenProtocol(
                (*handles)[i],
                protocolGuid,
                (VOID**)((UINT8*)(*protocols) + (sizeof(*protocols) * i)),
                imageHandle,
                NULL,
                EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL
            );

            ++(*number_of_protocols);

            if (status == EFI_UNSUPPORTED) {
                FREE(
                    status,
                    "Handles did not support protocol"
                );
            }

            if (status == EFI_ACCESS_DENIED) {
                FREE(
                    status,
                    "Handle protocl access denied"
                );
            }

            if (EFI_ERROR(status)) {
                FREE(
                    status,
                    "Error opening protocol"
                );
            }
        }
    }

    return EFI_SUCCESS;
}
