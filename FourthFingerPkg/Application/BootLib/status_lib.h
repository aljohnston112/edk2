#ifndef STATUS_LIB_H
#define STATUS_LIB_H

#include <stdbool.h>
#include <Library/UefiLib.h>

#define RETURN_IF_NOT_SUCCESS(status, string_on_error) \
    do{ \
        if (EFI_ERROR(status)) { \
            AsciiPrint( \
                "Error: %s\n\r", \
                string_on_error \
            ); \
            return status; \
        } \
    } while(0);

#define RETURN_IF_INVALID_PARAMETER_STATUS(status, string_on_error) \
    do { \
        if (status == EFI_INVALID_PARAMETER) { \
            RETURN_IF_NOT_SUCCESS( \
                status, \
                string_on_error \
            ); \
        } \
    } while(0);

#endif //STATUS_LIB_H
