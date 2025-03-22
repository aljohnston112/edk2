#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDevicePathLib/UefiDevicePathLib.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/PciRootBridgeIo.h>

#include "console_lib.h"
#include "guid_lib.h"
#include "handle_lib.h"
#include "page_lib.h"
#include "status_lib.h"

EFI_STATUS free_handle_pool(EFI_HANDLE* handles) {
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
    EFI_HANDLE** handles,
    UINTN* number_of_handles,
    EFI_GUID* protocol
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

void* align_ptr(void* ptr, const UINTN alignment) {
    return (void*)((UINTN)((UINT8*)ptr + (alignment - 1)) & ~(alignment - 1));
}

EFI_STATUS print_device_path_node_info(EFI_DEVICE_PATH_PROTOCOL* protocol) {
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
            const EFI_GUID* firmware_file_name = (EFI_GUID*)((UINT8*)protocol + 4);
            AsciiPrint("    GUID: ");
            print_guid(firmware_file_name);
            AsciiPrint("\n\n");
            break;
        case 7:
            AsciiPrint("    Firmware Volume Media Device Path\n");
            const EFI_GUID* firmware_volume_name = (EFI_GUID*)((UINT8*)protocol + 4);
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

EFI_STATUS process_pci_root_bridge(EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL* protocol) {
    UINT8* resources = NULL;
    EFI_STATUS status = protocol->Configuration(
        protocol,
        (void*)resources
    );
    RETURN_IF_NOT_SUCCESS(
        status,
        "Error acquiring PCI root bridge resource descriptors"
    );

    if(resources != NULL) {
        const UINT8 first_btye = *resources;
        if (first_btye == 0x8A) {
            const UINT16 length = *(INT16*)(resources + 1);
            AsciiPrint("    Length of descriptor: %u\n", length);
            const UINT8 resource_type = *(resources + 3);
            const UINT8 type_specific_flags = *(resources + 5);

            switch (resource_type) {
            case 0:
                AsciiPrint("    Memory range\n");
                if (type_specific_flags & (0b1 << 0)) {
                    AsciiPrint("    Read/Write\n");
                } else {
                    AsciiPrint("    Read Only\n");
                }
                switch ((type_specific_flags & (0b11 << 1)) >> 1) {
                case 0:
                    AsciiPrint("    Non-cachable\n");
                    break;
                case 1:
                    AsciiPrint("    Cachable\n");
                    break;
                case 2:
                    AsciiPrint("    Cachable and supports write combining\n");
                    break;
                default:
                    AsciiPrint("    Cachable and prefetchable\n");
                    break;
                }
                break;
            case 1:
                AsciiPrint("    I/O range\n");
                break;
            case 2:
                AsciiPrint("    Bus number range\n");
                break;
            case 3
                ...
                191
                :
                    AsciiPrint("    Reserved\n");
                break;
            default:
                AsciiPrint("    Hardware vendor defined\n");
                break;
            }
            const UINT8 flags = *(resources + 4);
            if (flags & (0b1 << 1)) {
                AsciiPrint("    Bridge subtractively decodes this address\n");
            } else {
                AsciiPrint("    Bridge positively decodes this address\n");
            }
            if (flags & (0b1 << 2)) {
                AsciiPrint("    Minimum memory address is fixed\n");
            } else {
                AsciiPrint("    Minimum memory address is not fixed\n");
            }
            if (flags & (0b1 << 3)) {
                AsciiPrint("    Maximum memory address is fixed\n");
            } else {
                AsciiPrint("    Maximum memory address is not fixed\n");
            }
        } else if (first_btye == 0x79) {} else {
            RETURN_IF_NOT_SUCCESS(
                status,
                "First byte of resource descriptor was invalid"
            );
        }
    } else {
        RETURN_IF_NOT_SUCCESS(
                status,
                "Failed to get resources form the configuration table"
            );
    }


    return EFI_SUCCESS;
}

EFI_STATUS get_all_handles(
    EFI_HANDLE image_handle
) {
    __label__ error;

    EFI_STATUS status = EFI_SUCCESS;
    char* error_string = "Success";
    EFI_HANDLE* handle_buffer = NULL;
    EFI_GUID** protocol_gui_buffer = NULL;
    UINTN guid_count = 0;

    void free_pools()
    {
        if (handle_buffer != NULL) {
            gBS->FreePool(handle_buffer);
        }
        if (protocol_gui_buffer != NULL) {
            for (UINTN j = 0; j < guid_count; j++) {
                gBS->FreePool(protocol_gui_buffer[j]);
            }
        }
    }

    void free()
    {
        free_pools();
        goto error;
    }

    BOOLEAN free_on_error(EFI_STATUS s, char* string_on_error)
    {
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
        ByProtocol,
        &gEfiDevicePathProtocolGuid,
        NULL,
        &handle_count,
        &handle_buffer
    );
    check_locate_handle_status(status);
    RETURN_IF_NOT_SUCCESS(status, "Error locating handles");

    UINTN number_of_pages = handle_count;
    EFI_STATUS on_next_page(UINTN current_page)
    {
        status = clear_console();
        free_on_error(status, "Failed to clear console");

        AsciiPrint(
            "%u out of %u\n\n",
            current_page + 1,
            number_of_pages
        );
        EFI_HANDLE* current_handle = &handle_buffer[current_page];
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
                EFI_LOADED_IMAGE_PROTOCOL* protocol = NULL;
                status = gBS->OpenProtocol(
                    *current_handle,
                    &gEfiLoadedImageProtocolGuid,
                    (VOID**)&protocol,
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
                EFI_DEVICE_PATH_PROTOCOL* protocol = NULL;
                status = gBS->OpenProtocol(
                    *current_handle,
                    &gEfiDevicePathProtocolGuid,
                    (VOID**)&protocol,
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
            } else if (CompareGuid(protocol_gui_buffer[j], &gEfiLoadedImageDevicePathProtocolGuid)) {
                AsciiPrint("EFI_LOADED_IMAGE_DEVICE_PATH_PROTOCOL\n");
                AsciiPrint("GUID: ");
                print_guid(protocol_gui_buffer[j]);
                AsciiPrint("\n");
                EFI_DEVICE_PATH_PROTOCOL* protocol = NULL;
                status = gBS->OpenProtocol(
                    *current_handle,
                    &gEfiLoadedImageDevicePathProtocolGuid,
                    (VOID**)&protocol,
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
            } else if (CompareGuid(protocol_gui_buffer[j], &gEfiPciRootBridgeIoProtocolGuid)) {
                AsciiPrint("EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL\n");
                AsciiPrint("GUID: ");
                print_guid(protocol_gui_buffer[j]);
                AsciiPrint("\n\n");

                EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL* protocol = NULL;
                status = gBS->OpenProtocol(
                    *current_handle,
                    &gEfiPciRootBridgeIoProtocolGuid,
                    (VOID**)&protocol,
                    image_handle,
                    NULL,
                    EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL
                );
                free_on_error(
                    status,
                    "Failed to open EFI_LOADED_IMAGE_PROTOCOL protocol"
                );

                if (protocol != NULL) {
                    process_pci_root_bridge(protocol);
                }
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

    start_paging(
        free_on_error,
        on_next_page,
        free,
        number_of_pages
    );

    while (1) {}

error:
    RETURN_IF_NOT_SUCCESS(status, error_string);
    return status;
}
