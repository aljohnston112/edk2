#include <stdlib.h>

#include <Library/UefiLib.h>

#include "handle_lib.h"
#include "protocol_lib.h"
#include "status_lib.h"
#include "text_input_lib.h"

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
    EFI_STATUS status = get_protocols(
        imageHandle,
        &handles,
        (VOID**)&text_input_protocols,
        &number_of_handles,
        &number_of_text_input_protocols,
        &gEfiSimpleTextInputExProtocolGuid
    );
    if(EFI_ERROR(status)) {
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
        status = get_protocols(
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
