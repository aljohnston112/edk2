#include <Library/UefiLib.h>

#include "guid_lib.h"

void print_guid(const EFI_GUID* guid) {
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
