#include <Library/UefiBootServicesTableLib.h>

#include "status_lib.h"
#include "text_input_lib.h"

#include <Library/UefiRuntimeServicesTableLib.h>

EFI_STATUS check_locate_handle_status(const EFI_STATUS status) {
    RETURN_IF_INVALID_PARAMETER_STATUS(
        status,
        "Check handle argument error for text input"
    );
    if (status == EFI_NOT_FOUND) {
        RETURN_IF_NOT_SUCCESS(
            status,
            "Could not find handle for text input"
        );
    }
    return EFI_SUCCESS;
}

EFI_STATUS get_handles(
    EFI_HANDLE** handles,
    UINTN* buffer_size
) {
    *buffer_size = 0;
    EFI_STATUS status = gBS->LocateHandle(
        ByProtocol,
        &gEfiSimpleTextInputExProtocolGuid,
        NULL,
        buffer_size,
        *handles
    );

    RETURN_IF_INVALID_PARAMETER_STATUS(
        status,
        "Error locating handle"
    );

    while (status == EFI_BUFFER_TOO_SMALL) {
        RETURN_IF_NOT_SUCCESS(
            check_allocate_pool_status(
                gBS->AllocatePool(
                    EfiLoaderData,
                    *buffer_size,
                    (VOID**)handles
                )
            ),
            "Error allocating pool for handles"
        );

        status = gBS->LocateHandle(
            ByProtocol,
            &gEfiSimpleTextInputExProtocolGuid,
            NULL,
            buffer_size,
            *handles
        );

        if (status == EFI_INVALID_PARAMETER) {
            gBS->FreePool(handles);

            RETURN_IF_NOT_SUCCESS(
                status,
                "Failed to get handle"
            );
        }
    }
    return EFI_SUCCESS;
}

EFI_STATUS get_text_input_handles(
    const EFI_HANDLE imageHandle,
    EFI_HANDLE** handles,
    EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL** text_input_protocols,
    UINTN* text_input_buffer_size
) {
    UINTN buffer_size = 0;
    RETURN_IF_NOT_SUCCESS(
        get_handles(
            handles,
            &buffer_size
        ),
        "Failed to get text input handles"
    );

    *text_input_buffer_size = 0;
    if (handles != NULL) {
        const UINTN number_of_handles = buffer_size / sizeof(EFI_HANDLE*);

        RETURN_IF_NOT_SUCCESS(
            AsciiPrint(
                "Number of handles: %u\n",
                number_of_handles
            ),
            "Failed to print number of handles"
        );

        RETURN_IF_NOT_SUCCESS(
            check_allocate_pool_status(
                gBS->AllocatePool(
                    EfiLoaderData,
                    number_of_handles * sizeof(EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL*),
                    (VOID**)text_input_protocols
                )
            ),
            "Error allocating pool for text input handles"
        );

        for (UINTN i = 0; i < number_of_handles; i++) {
            const EFI_STATUS status = gBS->OpenProtocol(
                (*handles)[i],
                &gEfiSimpleTextInputExProtocolGuid,
                (VOID**)&(*text_input_protocols)[i],
                imageHandle,
                NULL,
                EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL
            );
            *text_input_buffer_size += sizeof(EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL*);

            if (EFI_ERROR(status)) {
                for (UINTN j = 0; j < i; j++) {
                    gBS->CloseProtocol(
                        (*handles)[j],
                        &gEfiSimpleTextInputExProtocolGuid,
                        imageHandle,
                        NULL
                    );
                }

                gBS->FreePool(*handles);
                gBS->FreePool(*text_input_protocols);
                *text_input_buffer_size = 0;
                AsciiPrint("Error opening text input protocol\n");
                break;
            }

        }
    }
    else {
        RETURN_IF_NOT_SUCCESS(
            EFI_ABORTED,
            "Text input handles were null"
        );
    }

    return EFI_SUCCESS;
}

EFI_STATUS print_input_text_handle_names(
    const EFI_HANDLE imageHandle
) {
    EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL* protocols = NULL;
    EFI_HANDLE* handles = NULL;
    UINTN buffer_size = 0;

    RETURN_IF_NOT_SUCCESS(
        get_text_input_handles(
            imageHandle,
            &handles,
            &protocols,
            &buffer_size
        ),
        "Failed to get text input handles"
    );

    if (protocols != NULL) {
        const UINTN number_of_handles = buffer_size / sizeof(EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL*);

        RETURN_IF_NOT_SUCCESS(
            AsciiPrint(
                "Number of text input handles: %u\n",
                number_of_handles
            ),
            "Failed to print number of text input handles"
        );

        for (UINTN i = 0; i < number_of_handles; i++) {
            gBS->CloseProtocol(
                handles[i],
                &gEfiSimpleTextInputExProtocolGuid,
                imageHandle,
                NULL
            );
        }
        if(number_of_handles > 0) {
            gBS->FreePool(handles);
            gBS->FreePool(protocols);
        }
    }
    else {
        RETURN_IF_NOT_SUCCESS(
            EFI_ABORTED,
            "Text input handles were null"
        );
    }

    return EFI_SUCCESS;
}
