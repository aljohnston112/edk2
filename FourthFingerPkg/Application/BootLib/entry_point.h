#ifndef ENTRY_POINT_C_H
#define ENTRY_POINT_C_H

#include <Library/UefiLib.h>

EFI_STATUS
EFIAPI
UefiMain (
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
);

#endif //ENTRY_POINT_C_H
