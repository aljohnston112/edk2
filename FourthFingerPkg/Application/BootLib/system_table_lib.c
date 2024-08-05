#ifndef SYSTEM_TABLE_LIB_H
#define SYSTEM_TABLE_LIB_H

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include "status_lib.h"
#include "system_table_lib.h"

EFI_STATUS
print_efi_system_table() {

    EFI_STATUS status = AsciiPrint(
        "Hdr Signature: 0x%X\n",
        gST->Hdr.Signature
    );
    RETURN_IF_NOT_SUCCESS(
        status,
        "Failed to print Hdr Signature"
    );

    const UINTN major_version = gST->Hdr.Revision >> 16;
    const UINTN minor_version = (gST->Hdr.Revision & 0b1111111111111111) / 10;
    const UINTN patch_version = (gST->Hdr.Revision & 0b1111111111111111) % 10;
    status = AsciiPrint(
        "Hdr Revision: %u.%u.%u\n",
        major_version,
        minor_version,
        patch_version
    );
    RETURN_IF_NOT_SUCCESS(
        status,
        "Failed to print Hdr Revision"
    );

    status = AsciiPrint(
        "Hdr HeaderSize: %u bytes\n",
        gST->Hdr.HeaderSize
    );
    RETURN_IF_NOT_SUCCESS(
        status,
        "Failed to print Hdr HeaderSize"
    );

    status = AsciiPrint(
        "Hdr CRC32: 0x%X\n",
        gST->Hdr.CRC32
    );
    RETURN_IF_NOT_SUCCESS(
        status,
        "Failed to print Hdr CRC32"
    );

    status = AsciiPrint(
        "FirmwareVendor: %s\n",
        gST->FirmwareVendor
    );
    RETURN_IF_NOT_SUCCESS(
        status,
        "Failed to print FirmwareVendor"
    );

    status = AsciiPrint(
        "FirmwareRevision: 0x%X\n",
        gST->FirmwareRevision
    );
    RETURN_IF_NOT_SUCCESS(
        status,
        "Failed to print FirmwareRevision"
    );

    status = AsciiPrint(
        "NumberOfTableEntries: %lu\n",
        gST->NumberOfTableEntries
    );
    RETURN_IF_NOT_SUCCESS(
        status,
        "Failed to print NumberOfTableEntries"
    );

    return EFI_SUCCESS;
}

#endif //SYSTEM_TABLE_LIB_H
