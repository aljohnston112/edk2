
#ifndef SYSTEM_TABLE_LIB_H
#define SYSTEM_TABLE_LIB_H

#include <IndustryStandard/SmBios.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include "acpi_lib.h"
#include "console_lib.h"
#include "guid_lib.h"
#include "page_lib.h"
#include "status_lib.h"
#include "system_table_lib.h"
#include "text_input_lib.h"

UINT64 convert_to_uint64(const MISC_BIOS_CHARACTERISTICS* characteristics) {
    UINT64 result = 0;

    result |= (UINT64)characteristics->Reserved << 0;
    result |= (UINT64)characteristics->Unknown << 2;
    result |= (UINT64)characteristics->BiosCharacteristicsNotSupported << 3;
    result |= (UINT64)characteristics->IsaIsSupported << 4;
    result |= (UINT64)characteristics->McaIsSupported << 5;
    result |= (UINT64)characteristics->EisaIsSupported << 6;
    result |= (UINT64)characteristics->PciIsSupported << 7;
    result |= (UINT64)characteristics->PcmciaIsSupported << 8;
    result |= (UINT64)characteristics->PlugAndPlayIsSupported << 9;
    result |= (UINT64)characteristics->ApmIsSupported << 10;
    result |= (UINT64)characteristics->BiosIsUpgradable << 11;
    result |= (UINT64)characteristics->BiosShadowingAllowed << 12;
    result |= (UINT64)characteristics->VlVesaIsSupported << 13;
    result |= (UINT64)characteristics->EscdSupportIsAvailable << 14;
    result |= (UINT64)characteristics->BootFromCdIsSupported << 15;
    result |= (UINT64)characteristics->SelectableBootIsSupported << 16;
    result |= (UINT64)characteristics->RomBiosIsSocketed << 17;
    result |= (UINT64)characteristics->BootFromPcmciaIsSupported << 18;
    result |= (UINT64)characteristics->EDDSpecificationIsSupported << 19;
    result |= (UINT64)characteristics->JapaneseNecFloppyIsSupported << 20;
    result |= (UINT64)characteristics->JapaneseToshibaFloppyIsSupported << 21;
    result |= (UINT64)characteristics->Floppy525_360IsSupported << 22;
    result |= (UINT64)characteristics->Floppy525_12IsSupported << 23;
    result |= (UINT64)characteristics->Floppy35_720IsSupported << 24;
    result |= (UINT64)characteristics->Floppy35_288IsSupported << 25;
    result |= (UINT64)characteristics->PrintScreenIsSupported << 26;
    result |= (UINT64)characteristics->Keyboard8042IsSupported << 27;
    result |= (UINT64)characteristics->SerialIsSupported << 28;
    result |= (UINT64)characteristics->PrinterIsSupported << 29;
    result |= (UINT64)characteristics->CgaMonoIsSupported << 30;
    result |= (UINT64)characteristics->NecPc98 << 31;
    result |= (UINT64)characteristics->ReservedForVendor << 32;

    return result;
}

void print_chars(const CHAR8* p, const UINTN number) {
    for (UINTN i = 0; i < number; ++i) {
        const CHAR8 ch = *(p + i);
        AsciiPrint("Char %2u: 0x%2X %c | ", i + 1, (UINT8)ch, (ch >= 32 && ch <= 126) ? ch : '.');
        if (i % 4 == 0) {
            AsciiPrint("\n");
        }
    }
    AsciiPrint("\n");
}

EFI_STATUS extract_smbios_strings(
    CHAR8* string_start,
    UINTN* string_count,
    CHAR8*** strings
) {
    (*string_count) = 0;
    CHAR8* current_string = string_start;

    if (*current_string != 0 || *(current_string + 1) != 0) {
        ++(*string_count);
        current_string += AsciiStrLen(current_string) + 1;

        while (*(current_string - 1) != 0 || *(current_string) != 0) {
            ++(*string_count);
            current_string += AsciiStrLen(current_string) + 1;
        }

        EFI_STATUS status = gBS->AllocatePool(
            EfiLoaderData,
            (*string_count) * sizeof(CHAR8*),
            (void**)strings
        );
        if (EFI_ERROR(status) || (*strings) == NULL) {
            (*string_count) = 0;
            RETURN_IF_NOT_SUCCESS(
                status,
                "Failed to allocate for strings"
            );
        }

        current_string = string_start;
        for (UINTN i = 0; i < (*string_count); i++) {
            (*strings)[i] = current_string;
            current_string += AsciiStrLen((*strings)[i]) + 1;
        }
    }

    return EFI_SUCCESS;
}

void process_smbios30(
    SMBIOS_TABLE_3_0_ENTRY_POINT* table,
    OnError free_on_error
) {
    if (table->AnchorString[0] != 0x5F ||
        table->AnchorString[1] != 0x53 ||
        table->AnchorString[2] != 0x4D ||
        table->AnchorString[3] != 0x33 ||
        table->AnchorString[4] != 0x5F) {
        free_on_error(EFI_ABORTED, "Invalid SMBIOS 3.0 table");
    } else {
        AsciiPrint("Entry Point Length: %u\n", table->EntryPointLength);
        AsciiPrint("Major Version: %u\n", table->MajorVersion);
        AsciiPrint("Minor Version: %u\n", table->MinorVersion);
        AsciiPrint("Patch Version: %u\n", table->DocRev);
        AsciiPrint("Entry Point Revision: %u\n", table->EntryPointRevision);
        AsciiPrint("Maxumum Table Size: %u\n", table->TableMaximumSize);

        SMBIOS_STRUCTURE_POINTER structure_table;
        structure_table.Hdr = (SMBIOS_STRUCTURE*)table->TableAddress;
        UINT8 structure_type = structure_table.Hdr->Type;
        UINT8 length = structure_table.Hdr->Length;

        while (structure_type != 127) {
            CHAR8** strings = NULL;
            UINTN number_of_strings = 0;

            CHAR8* string_ptr = (CHAR8*)structure_table.Raw + length;
            EFI_STATUS status = extract_smbios_strings(
                string_ptr,
                &number_of_strings,
                &strings
            );

            free_on_error(status, "Failed to extract SMBIOS strings");

            for (UINTN i = 0; i < number_of_strings; i++) {
                AsciiPrint("String %u: %a\n", i + 1, strings[i]);
            }

            AsciiPrint("Structure Type: %u\n", structure_type);
            switch (structure_type) {
            case 0:
                SMBIOS_TABLE_TYPE0* table0 = structure_table.Type0;
                AsciiPrint("BIOS Vendor: %a\n", strings[table0->Vendor - 1]);
                AsciiPrint("BIOS Version: %a\n", strings[table0->BiosVersion - 1]);
                AsciiPrint("BIOS Release Date: %a\n", strings[table0->BiosReleaseDate - 1]);
                AsciiPrint("BIOS Characteristics: 0x%8X\n", convert_to_uint64(&table0->BiosCharacteristics));
                AsciiPrint("BIOS Extended Characteristics Byte 1: 0x%2X\n", table0->BIOSCharacteristicsExtensionBytes[0]);
                AsciiPrint("BIOS Extended Characteristics Byte 1: 0x%2X\n", table0->BIOSCharacteristicsExtensionBytes[1]);
                break;
            case 1:
                break;
            case 3:
                break;
            case 4:
                SMBIOS_TABLE_TYPE4* table4 = structure_table.Type4;
                AsciiPrint("Socket Designation: %a\n", strings[table4->Socket - 1]);
                AsciiPrint("Processor Family: 0x%X\n", table4->ProcessorFamily);
                AsciiPrint("Processor Manufacturer: %a\n", strings[table4->ProcessorManufacturer - 1]);
                AsciiPrint("Processor ID: 0x%X\n", table4->ProcessorId);
                AsciiPrint("Processor Version: %a\n", strings[table4->ProcessorVersion - 1]);
                break;
            case 7:
                break;
            case 9:
                break;
            case 16:
                break;
            case 17:
                break;
            case 19:
                break;
            case 32:
                break;
            default:
                AsciiPrint(
                    "Unsupported Structure Type: %u\n",
                    structure_type
                );
                break;
            }

            if(strings != NULL) {
                status = gBS->FreePool(strings);
                free_on_error(status, "Failed to free strings");
            }

            EFI_INPUT_KEY key;
            BOOLEAN validKey = FALSE;
            BOOLEAN cont = TRUE;
            while (validKey == FALSE) {
                wait_for_key_and_get_it(&key);
                if (key.UnicodeChar == UNICODE_ENTER) {
                    validKey = TRUE;
                    cont = FALSE;
                } else if (key.UnicodeChar == UNICODE_SPACE) {
                    validKey = TRUE;
                }
            }
            if (cont == FALSE) {
                break;
            }

            structure_table.Raw = structure_table.Raw + length;

            while (*structure_table.Raw != 0 || *(structure_table.Raw + 1) != 0) {
                structure_table.Raw++;
            }
            structure_table.Raw += 2;
            structure_type = structure_table.Hdr->Type;
            length = structure_table.Hdr->Length;
        }
    }
}



EFI_STATUS
print_efi_system_table() {
    __label__ error;

    EFI_STATUS status = EFI_SUCCESS;
    char* error_string = "Success";

    void free_pools() {}

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

    AsciiPrint(
        "Hdr Signature: 0x%X\n",
        gST->Hdr.Signature
    );

    const UINTN major_version = gST->Hdr.Revision >> 16;
    const UINTN minor_version = (gST->Hdr.Revision & 0b1111111111111111) / 10;
    const UINTN patch_version = (gST->Hdr.Revision & 0b1111111111111111) % 10;

    AsciiPrint(
        "Hdr Revision: %u.%u.%u\n",
        major_version,
        minor_version,
        patch_version
    );

    AsciiPrint(
        "Hdr HeaderSize: %u bytes\n",
        gST->Hdr.HeaderSize
    );

    AsciiPrint(
        "Hdr CRC32: 0x%X\n",
        gST->Hdr.CRC32
    );

    AsciiPrint(
        "FirmwareVendor: %s\n",
        gST->FirmwareVendor
    );

    AsciiPrint(
        "FirmwareRevision: 0x%X\n",
        gST->FirmwareRevision
    );

    AsciiPrint(
        "NumberOfTableEntries: %lu\n",
        gST->NumberOfTableEntries
    );

    AsciiPrint("\nPress enter to continue\n");
    status = wait_for_unicode(UNICODE_ENTER);

    EFI_CONFIGURATION_TABLE* configuration_table = gST->ConfigurationTable;
    UINTN number_of_pages = gST->NumberOfTableEntries;
    EFI_STATUS on_next_page(UINTN current_page)
    {
        status = clear_console();
        free_on_error(status, "Failed to clear console");

        AsciiPrint(
            "%u out of %u\n\n",
            current_page + 1,
            number_of_pages
        );
        AsciiPrint("Vendor GUID: ");
        EFI_GUID guid = configuration_table[current_page].VendorGuid;
        print_guid(&guid);
        AsciiPrint("\n");
        if (CompareGuid(&gEfiAcpi10TableGuid, &guid)) {
            AsciiPrint("    ACPI 1.0 RSDP\n");
        } else if (CompareGuid(&gEfiAcpi20TableGuid, &guid)) {
            AsciiPrint("    ACPI 2.0+ RSDP\n");
            EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER* table =
                configuration_table[current_page].VendorTable;
            process_acpi_20(table, free_on_error);
        } else if (CompareGuid(&gEfiSmbiosTableGuid, &guid)) {
            AsciiPrint("    SMBIOS 2.1\n");
        } else if (CompareGuid(&gEfiSmbios3TableGuid, &guid)) {
            AsciiPrint("    SMBIOS 3.0\n");
            SMBIOS_TABLE_3_0_ENTRY_POINT* table = configuration_table[current_page].VendorTable;
            process_smbios30(table, free_on_error);
        }

        return status;
    }

    start_paging(
        free_on_error,
        on_next_page,
        free,
        number_of_pages
    );

    while
    (1) {}
error:
    RETURN_IF_NOT_SUCCESS(status, error_string);
    return status;
}

#endif //SYSTEM_TABLE_LIB_H
