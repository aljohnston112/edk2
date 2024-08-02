#ifndef SYSTEM_TABLE_LIB_H
#define SYSTEM_TABLE_LIB_H

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include "status_lib.h"
#include "system_table_lib.h"

EFI_STATUS
PrintEfiSystemTable() {

    EFI_STATUS status = AsciiPrint(
        "Hdr Signature: %lx\n",
        gST->Hdr.Signature
    );
    RETURN_IF_NOT_SUCCESS(
        status,
        "Failed to print Hdr Signature"
    );

    status = AsciiPrint(
        "Hdr Revision: %x\n",
        gST->Hdr.Revision
    );
    RETURN_IF_NOT_SUCCESS(
        status,
        "Failed to print Hdr Revision"
    );

    status = AsciiPrint(
        "Hdr HeaderSize: %x\n",
        gST->Hdr.HeaderSize
    );
    RETURN_IF_NOT_SUCCESS(
        status,
        "Failed to print Hdr HeaderSize"
    );

    status = AsciiPrint(
        "Hdr CRC32: %x\n",
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
        "FirmwareRevision: %u\n",
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
