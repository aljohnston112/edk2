#ifndef SYSTEM_TABLE_LIB_H
#define SYSTEM_TABLE_LIB_H

#include <Library/UefiBootServicesTableLib.h>

#include "status_lib.h"
#include "system_table_lib.h"

EFI_STATUS
PrintEfiSystemTable() {

    RETURN_IF_NOT_SUCCESS(
        AsciiPrint(
            "Hdr Signature: %lx\n",
            gST->Hdr.Signature
        ),
        "Failed to print Hdr Signature"
    );

    RETURN_IF_NOT_SUCCESS(
        AsciiPrint(
            "Hdr Revision: %x\n",
            gST->Hdr.Revision
        ),
        "Failed to print Hdr Revision"
    );

    RETURN_IF_NOT_SUCCESS(
        AsciiPrint(
            "Hdr HeaderSize: %x\n",
            gST->Hdr.HeaderSize
        ),
        "Failed to print Hdr HeaderSize"
    );

    RETURN_IF_NOT_SUCCESS(
        AsciiPrint(
            "Hdr CRC32: %x\n",
            gST->Hdr.CRC32
        ),
        "Failed to print Hdr CRC32"
    );

    RETURN_IF_NOT_SUCCESS(
        AsciiPrint(
            "FirmwareVendor: %s\n",
            gST->FirmwareVendor
        ),
        "Failed to print FirmwareVendor"
    );

    RETURN_IF_NOT_SUCCESS(
        AsciiPrint(
            "FirmwareRevision: %u\n",
            gST->FirmwareRevision
        ),
        "Failed to print FirmwareRevision"
    );

    RETURN_IF_NOT_SUCCESS(
        AsciiPrint(
            "NumberOfTableEntries: %lu\n",
            gST->NumberOfTableEntries
        ),
        "Failed to print NumberOfTableEntries"
    );

    return EFI_SUCCESS;
}

#endif //SYSTEM_TABLE_LIB_H
