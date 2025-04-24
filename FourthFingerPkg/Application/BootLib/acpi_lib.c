#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

#include "acpi_lib.h"
#include "status_lib.h"

void process_xsdt(
    EFI_ACPI_DESCRIPTION_HEADER* xsdt,
    OnError free_on_error
) {
    EFI_ACPI_DESCRIPTION_HEADER* entries;
    if (xsdt == NULL || xsdt->Signature != EFI_ACPI_2_0_EXTENDED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE) {
        free_on_error(EFI_ABORTED, "Invalid XSDT table");
    } else {
        const UINT32 bytes_before_entry = 36;
        const UINT32 bytes_per_entry = 8;
        const UINT32 number_of_entries = (xsdt->Length - bytes_before_entry) / bytes_per_entry;
        AsciiPrint("Found %u entries in XSDT\n", number_of_entries);
        EFI_STATUS status = check_allocate_pool_status(
            gBS->AllocatePool(
                EfiLoaderData,
                bytes_before_entry * number_of_entries,
                (VOID**)&entries
            )
        );
        free_on_error(
            status,
            "Unknown error allocating pool for memory map"
        );
        for (UINTN i = 0; i < number_of_entries; i++) {
            entries[i] = *(EFI_ACPI_DESCRIPTION_HEADER*)*(UINT64*)(((UINT8*)(xsdt + 1)) + (bytes_per_entry * i));
            AsciiPrint(
                "Found: %c%c%c%c\n",
                ((UINT8*)&entries[i].Signature)[0],
                ((UINT8*)&entries[i].Signature)[1],
                ((UINT8*)&entries[i].Signature)[2],
                ((UINT8*)&entries[i].Signature)[3]
            );
        }
    }
}

void process_acpi_20(
    EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER* table,
    OnError free_on_error
) {
    EFI_ACPI_DESCRIPTION_HEADER* xsdt = NULL;
    if (table->Signature != EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER_SIGNATURE) {
        free_on_error(EFI_ABORTED, "Invalid ACPI 2.0+ table");
    } else {
        AsciiPrint("OEM id: %a\n", table->OemId);
        AsciiPrint("Revision: %u\n", table->Revision);
        xsdt = (EFI_ACPI_DESCRIPTION_HEADER*)table->XsdtAddress;
    }
    process_xsdt(xsdt, free_on_error);
}
