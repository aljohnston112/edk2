#include <stdlib.h>

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include "handle_lib.h"
#include "protocol_lib.h"
#include "status_lib.h"
#include "text_input_lib.h"


RETURN_STATUS wait_for_key() {
    UINTN index = 0;
    EFI_STATUS status = gBS->WaitForEvent(
        1,
        &gST->ConIn->WaitForKey,
        &index
    );
    RETURN_IF_NOT_SUCCESS(
        status,
        "Failed to wait for key"
    );

    return EFI_SUCCESS;
}

EFI_STATUS wait_for_key_and_get_it(
    EFI_INPUT_KEY* key
) {
    EFI_STATUS status = wait_for_key();
    RETURN_IF_NOT_SUCCESS(
        status,
        "Failed to wait for key"
    );
    while ((status = gST->ConIn->ReadKeyStroke(
            gST->ConIn,
            key
        )) == EFI_NOT_READY
    ) {}

    RETURN_IF_NOT_SUCCESS(
        status,
        "Failed to read key"
    );
    return EFI_SUCCESS;
}

EFI_STATUS wait_for_unicode(
    const UINT16 unicode_char
) {
    EFI_STATUS status = EFI_SUCCESS;
    EFI_INPUT_KEY key;
    do {
        status = wait_for_key_and_get_it(&key);
        RETURN_IF_NOT_SUCCESS(
            status,
            "Failed to wait for key"
        );
    }
    while (key.UnicodeChar != unicode_char);
    return status;
}

EFI_STATUS wait_for_scan_code(
    const UINT16 unicode_char
) {
    EFI_STATUS status = EFI_SUCCESS;
    EFI_INPUT_KEY key;
    do {
        status = wait_for_key_and_get_it(&key);
        RETURN_IF_NOT_SUCCESS(
            status,
            "Failed to wait for key"
        );
    }
    while (key.ScanCode != unicode_char);
    return status;
}

EFI_STATUS wait_for_any_key() {
    EFI_STATUS status = EFI_SUCCESS;
    EFI_INPUT_KEY key;
    status = wait_for_key_and_get_it(&key);
    RETURN_IF_NOT_SUCCESS(
        status,
        "Failed to wait for key"
    );
    return status;
}

EFI_STATUS print_input_text_handle_names(
    EFI_HANDLE imageHandle
) {
    EFI_DEVICE_PATH_PROTOCOL* device_path_protocols = NULL;
    EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL* text_input_protocols = NULL;
    EFI_HANDLE* handles = NULL;
    UINTN number_of_handles = 0;
    UINTN number_of_device_path_protocols = 0;
    UINTN number_of_text_input_protocols = 0;

#define FREE(status, string_on_error) \
    EFI_STATUS original_status = status; \
    do { \
        if (number_of_text_input_protocols > 0) { \
            status = close_protocols_and_free_pool( \
                handles, \
                number_of_text_input_protocols, \
                &gEfiSimpleTextInputExProtocolGuid, \
                imageHandle, \
                (VOID **)&text_input_protocols \
            ); \
        } \
        if (number_of_device_path_protocols > 0) { \
            status = close_protocols_and_free_pool( \
                handles, \
                number_of_device_path_protocols, \
                &gEfiDevicePathProtocolGuid, \
                imageHandle, \
                (VOID **)&device_path_protocols \
            ); \
        } \
        status = free_handle_pool(handles); \
        RETURN_IF_NOT_SUCCESS( \
            original_status, \
            string_on_error \
        ); \
        return EFI_SUCCESS; \
    } while(0)
    EFI_STATUS status = get_protocol_from_handles(
        imageHandle,
        &handles,
        (VOID**)&text_input_protocols,
        &number_of_handles,
        &number_of_text_input_protocols,
        &gEfiSimpleTextInputExProtocolGuid
    );
    if (EFI_ERROR(status)) {
        FREE(
            status,
            "Failed to get text input protocols"
        );
    }

    if (text_input_protocols != NULL) {
        status = AsciiPrint(
            "Number of text input protocols: %u\n",
            number_of_text_input_protocols
        );
        if (EFI_ERROR(status)) {
            FREE(
                status,
                "Failed to print number of text input protocols"
            );
        }
    } else {
        status = EFI_ABORTED;
        FREE(
            status,
            "Text input protocols were null"
        );
    }

    if (handles != NULL) {
        status = get_protocol_from_handles(
            imageHandle,
            &handles,
            (VOID**)&device_path_protocols,
            &number_of_handles,
            &number_of_device_path_protocols,
            &gEfiDevicePathProtocolGuid
        );

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
                "Failed to get device path protocols"
            );
        }

        if (device_path_protocols != NULL) {}
    }

    return EFI_SUCCESS;
}
