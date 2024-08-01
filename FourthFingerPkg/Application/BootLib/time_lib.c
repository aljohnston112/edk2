#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include "status_lib.h"
#include "time_lib.h"

EFI_STATUS
PrintEfiTime() {

    EFI_TIME *time;
    RETURN_IF_NOT_SUCCESS(
         gBS->AllocatePool (
            EfiLoaderData,
            sizeof (EFI_TIME),
            (VOID **)&time
        ),
        "Alloc for time failed"
    );

    EFI_TIME_CAPABILITIES *capabilities;
    RETURN_IF_NOT_SUCCESS(
        gBS->AllocatePool (
            EfiLoaderData,
            sizeof (EFI_TIME_CAPABILITIES),
            (VOID **)&capabilities
        ),
        "Alloc for capabilities failed"
    );

    RETURN_IF_NOT_SUCCESS(
        gRT->GetTime (
            time,
            capabilities
        ),
        "GetTime failed"
    );

    if (time != NULL) {
        RETURN_IF_NOT_SUCCESS(
            AsciiPrint(
                "Year: %u\n",
                time->Year
            ),
            "Failed to print Year"
        );

        RETURN_IF_NOT_SUCCESS(
            AsciiPrint(
                "Month: %u\n",
                time->Month
            ),
            "Failed to print Month"
        );

        RETURN_IF_NOT_SUCCESS(
            AsciiPrint(
                "Day: %u\n",
                time->Day
            ),
            "Failed to print Day"
        );

        RETURN_IF_NOT_SUCCESS(
            AsciiPrint(
                "Hour: %u\n",
                time->Hour
            ),
            "Failed to print Hour"
        );

        RETURN_IF_NOT_SUCCESS(
            AsciiPrint(
                "Minute: %u\n",
                time->Minute
            ),
            "Failed to print Minute"
        );

        RETURN_IF_NOT_SUCCESS(
            AsciiPrint(
                "Second: %u\n",
                time->Second
            ),
            "Failed to print Second"
        );

        RETURN_IF_NOT_SUCCESS(
            AsciiPrint(
                "Nanosecond: %u\n",
                time->Nanosecond
            ),
            "Failed to print Nanosecond"
        );

        RETURN_IF_NOT_SUCCESS(
            AsciiPrint(
                "TimeZone: %d\n",
                time->TimeZone
            ),
            "Failed to print TimeZone"
        );

        RETURN_IF_NOT_SUCCESS(
            AsciiPrint(
                "Daylight: %u\n",
                time->Daylight
            ),
            "Failed to print Daylight"
        );

        RETURN_IF_NOT_SUCCESS(
            gBS->FreePool(time),
            "Error freeing time"
        );

        RETURN_IF_NOT_SUCCESS(
            gBS->FreePool(capabilities),
            "Error freeing capabilities"
        );

    } else {
        RETURN_IF_NOT_SUCCESS(
            EFI_INVALID_PARAMETER,
            "Invalid EFI_TIME*"
        );
    }

    return EFI_SUCCESS;
}