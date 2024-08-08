#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDevicePathLib/UefiDevicePathLib.h>
#include <Pi/PiFirmwareFile.h>
#include <Pi/PiFirmwareVolume.h>

#include "console_lib.h"
#include "handle_lib.h"

#include <Library/BaseMemoryLib.h>
#include <Protocol/LoadedImage.h>

#include "page_lib.h"
#include "protocol_lib.h"
#include "status_lib.h"
#include "text_input_lib.h"

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

void *align_ptr(void *ptr, const UINTN alignment) {
    return (void *) ((UINTN) ((UINT8 *) ptr + (alignment - 1)) & ~(alignment - 1));
}

void print_guid(const EFI_GUID *guid) {
    AsciiPrint(
        "{ 0x%8X 0x%4X 0x%4X { 0x%2X 0x%2X 0x%2X 0x%2X 0x%2X 0x%2X 0x%2X 0x%2X }}",
        guid->Data1,
        guid->Data2,
        guid->Data3,
        guid->Data4[0],
        guid->Data4[1],
        guid->Data4[2],
        guid->Data4[3],
        guid->Data4[4],
        guid->Data4[5],
        guid->Data4[6],
        guid->Data4[7]
    );
}

EFI_STATUS print_device_path_node_info(EFI_DEVICE_PATH_PROTOCOL *protocol) {
    AsciiPrint(
        "    Type: 0x%X\n    Subtype: 0x%X\n    Length of Data: %u\n",
        protocol->Type,
        protocol->SubType,
        DevicePathNodeLength(protocol)
    );
    switch (protocol->Type) {
        case 4:
            switch (protocol->SubType) {
                case 6:
                    AsciiPrint("    Firmware File Media Device Path\n");
                    const EFI_GUID *firmware_file_name = (EFI_GUID *) ((UINT8 *) protocol + 4);
                    AsciiPrint("    GUID: ");
                    print_guid(firmware_file_name);
                    AsciiPrint("\n\n");
                    break;
                case 7:
                    AsciiPrint("    Firmware Volume Media Device Path\n");
                    const EFI_GUID *firmware_volume_name = (EFI_GUID *) ((UINT8 *) protocol + 4);
                    AsciiPrint("    GUID: ");
                    print_guid(firmware_volume_name);
                    AsciiPrint("\n\n");

                    break;
                default:
                    break;
            }
        default:
            break;
    }
    return EFI_SUCCESS;
}

EFI_STATUS get_all_handles(
    EFI_HANDLE image_handle
) {
    __label__ error;

    EFI_STATUS status = EFI_SUCCESS;
    char *error_string = "Success";
    EFI_HANDLE *handle_buffer = NULL;
    EFI_GUID **protocol_gui_buffer = NULL;
    UINTN guid_count = 0;

    void free_pools() {
        if (handle_buffer != NULL) {
            gBS->FreePool(handle_buffer);
        }
        if (protocol_gui_buffer != NULL) {
            for (UINTN j = 0; j < guid_count; j++) {
                gBS->FreePool(protocol_gui_buffer[j]);
            }
        }
    }

    void free() {
        free_pools();
        goto error;
    }

    BOOLEAN free_on_error(EFI_STATUS s, char *string_on_error) {
        status = s;
        if (EFI_ERROR(s)) {
            error_string = string_on_error;
            free();
            return TRUE;
        }
        return FALSE;
    }

    UINTN handle_count = 0;
    status = gBS->LocateHandleBuffer(
        AllHandles,
        NULL,
        NULL,
        &handle_count,
        &handle_buffer
    );
    check_locate_handle_status(status);
    RETURN_IF_NOT_SUCCESS(status, "Error locating handles");

    UINTN number_of_pages = handle_count;
    EFI_STATUS on_next_page(UINTN current_page) {
        clear_console();
        AsciiPrint(
            "%u out of %u\n\n",
            current_page + 1,
            number_of_pages
        );
        EFI_HANDLE *current_handle = &handle_buffer[current_page];
        status = gBS->ProtocolsPerHandle(
            *current_handle,
            &protocol_gui_buffer,
            &guid_count
        );
        free_on_error(status, "Failed to get protocols for handle");
        for (UINTN j = 0; j < guid_count; j++) {
            if (CompareGuid(protocol_gui_buffer[j], &gEfiLoadedImageProtocolGuid)) {
                AsciiPrint("EFI_LOADED_IMAGE_PROTOCOL\n");
                AsciiPrint("GUID: ");
                print_guid(protocol_gui_buffer[j]);
                AsciiPrint("\n");
                EFI_LOADED_IMAGE_PROTOCOL *protocol = NULL;
                status = gBS->OpenProtocol(
                    *current_handle,
                    &gEfiLoadedImageProtocolGuid,
                    (VOID **) &protocol,
                    image_handle,
                    NULL,
                    EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL
                );
                free_on_error(
                    status,
                    "Failed to open EFI_LOADED_IMAGE_PROTOCOL protocol"
                );

                if (protocol != NULL) {
                    status = print_device_path_node_info(
                        protocol->FilePath
                    );
                    free_on_error(
                        status,
                        "Failed to print device path node info"
                    );
                }
            } else if (CompareGuid(protocol_gui_buffer[j], &gEfiDevicePathProtocolGuid)) {
                AsciiPrint("EFI_DEVICE_PATH_PROTOCOL\n");
                AsciiPrint("GUID: ");
                print_guid(protocol_gui_buffer[j]);
                AsciiPrint("\n");
                EFI_DEVICE_PATH_PROTOCOL *protocol = NULL;
                status = gBS->OpenProtocol(
                    *current_handle,
                    &gEfiDevicePathProtocolGuid,
                    (VOID **) &protocol,
                    image_handle,
                    NULL,
                    EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL
                );
                free_on_error(
                    status,
                    "Failed to open EFI_DEVICE_PATH_PROTOCOL protocol"
                );

                if (protocol != NULL) {
                    status = print_device_path_node_info(protocol);
                    free_on_error(
                        status,
                        "Failed to print device path node info"
                    );
                } else if (CompareGuid(protocol_gui_buffer[j], &gEfiLoadedImageDevicePathProtocolGuid)) {
                    AsciiPrint("EFI_LOADED_IMAGE_DEVICE_PATH_PROTOCOL\n");
                    AsciiPrint("GUID: ");
                    print_guid(protocol_gui_buffer[j]);
                    AsciiPrint("\n");
                    EFI_DEVICE_PATH_PROTOCOL *protocol = NULL;
                    status = gBS->OpenProtocol(
                        *current_handle,
                        &gEfiLoadedImageDevicePathProtocolGuid,
                        (VOID **) &protocol,
                        image_handle,
                        NULL,
                        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL
                    );
                    free_on_error(
                        status,
                        "Failed to open EFI_DEVICE_PATH_PROTOCOL protocol"
                    );

                    if (protocol != NULL) {
                        status = print_device_path_node_info(protocol);
                        free_on_error(
                            status,
                            "Failed to print device path node info"
                        );
                    }
                } else if (CompareGuid(protocol_gui_buffer[j], &gEfiFirmwareVolume2ProtocolGuid)) {
                    AsciiPrint("EFI_FIRMWARE_VOLUME2_PROTOCOL\n");
                    AsciiPrint("GUID: ");
                    print_guid(protocol_gui_buffer[j]);
                    AsciiPrint("\n\n");
                } else if (CompareGuid(protocol_gui_buffer[j], &gEfiFirmwareVolumeBlock2ProtocolGuid)) {
                    AsciiPrint("EFI_FIRMWARE_VOLUME_BLOCK2_PROTOCOL\n");
                    AsciiPrint("GUID: ");
                    print_guid(protocol_gui_buffer[j]);
                    AsciiPrint("\n\n");
                } else {
                    AsciiPrint("GUID: ");
                    print_guid(protocol_gui_buffer[j]);
                    AsciiPrint("\n\n");
                }
            }
            gBS->FreePool(protocol_gui_buffer);
            protocol_gui_buffer = NULL;
            return status;
        }

        // Create event

        start_paging(
            free_on_error,
            on_next_page,
            free,
            number_of_pages
        );

        while (1) {
        }
    error:
        RETURN_IF_NOT_SUCCESS(status, error_string);
        return status;
    }
