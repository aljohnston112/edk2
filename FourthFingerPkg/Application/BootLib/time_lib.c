#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include "status_lib.h"
#include "time_lib.h"

#include <text_input_lib.h>

EFI_STATUS
PrintEfiTime() {
    EFI_TIME* time;
    EFI_STATUS status = gBS->AllocatePool(
        EfiLoaderData,
        sizeof(EFI_TIME),
        (VOID**)&time
    );
    RETURN_IF_NOT_SUCCESS(
        status,
        "Alloc for time failed"
    );

    EFI_TIME_CAPABILITIES* capabilities = NULL;
    status = gBS->AllocatePool(
        EfiLoaderData,
        sizeof(EFI_TIME_CAPABILITIES),
        (VOID**)&capabilities
    );
    RETURN_IF_NOT_SUCCESS(
        status,
        "Alloc for time capabilities failed"
    );

    status = gRT->GetTime(
        time,
        capabilities
    );
    RETURN_IF_NOT_SUCCESS(
        status,
        "GetTime failed"
    );

    if (capabilities != NULL) {
        status = gBS->FreePool(capabilities);
        RETURN_IF_NOT_SUCCESS(
            status,
            "Error freeing capabilities"
        );
    }

    if (time != NULL) {
        status = AsciiPrint(
            "Year: %u\n",
            time->Year
        );
        RETURN_IF_NOT_SUCCESS(
            status,
            "Failed to print Year"
        );

        status = AsciiPrint(
            "Month: %u\n",
            time->Month
        );
        RETURN_IF_NOT_SUCCESS(
            status,
            "Failed to print Month"
        );

        status = AsciiPrint(
            "Day: %u\n",
            time->Day
        );
        RETURN_IF_NOT_SUCCESS(
            status,
            "Failed to print Day"
        );

        status = AsciiPrint(
            "Hour: %u\n",
            time->Hour
        );
        RETURN_IF_NOT_SUCCESS(
            status,
            "Failed to print Hour"
        );

        status = AsciiPrint(
            "Minute: %u\n",
            time->Minute
        );
        RETURN_IF_NOT_SUCCESS(
            status,
            "Failed to print Minute"
        );

        status = AsciiPrint(
            "Second: %u\n",
            time->Second
        );
        RETURN_IF_NOT_SUCCESS(
            status,
            "Failed to print Second"
        );

        status = AsciiPrint(
            "Nanosecond: %u\n",
            time->Nanosecond
        );
        RETURN_IF_NOT_SUCCESS(
            status,
            "Failed to print Nanosecond"
        );

        status = AsciiPrint(
            "TimeZone: %d\n",
            time->TimeZone
        );
        RETURN_IF_NOT_SUCCESS(
            status,
            "Failed to print TimeZone"
        );

        status = AsciiPrint(
            "Daylight: %u\n",
            time->Daylight
        );
        RETURN_IF_NOT_SUCCESS(
            status,
            "Failed to print Daylight"
        );

        status = gBS->FreePool(time);
        RETURN_IF_NOT_SUCCESS(
            status,
            "Error freeing time"
        );
    } else {
        RETURN_IF_NOT_SUCCESS(
            EFI_INVALID_PARAMETER,
            "Time was null*"
        );
    }

    AsciiPrint("\nPress enter to return\n");
    status = wait_for_unicode(UNICODE_ENTER);
    RETURN_IF_NOT_SUCCESS(status , "Failed to wait for key");

    return EFI_SUCCESS;
}
