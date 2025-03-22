#ifndef STATUS_LIB_H
#define STATUS_LIB_H

#define RETURN_IF_NOT_SUCCESS(status, string_on_error) \
    do{ \
        if (EFI_ERROR(status)) { \
            AsciiPrint( \
                "Status: %u\nError: ", \
                status \
            ); \
            AsciiPrint( \
                string_on_error \
            ); \
            AsciiPrint( \
                "\n" \
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

#define RETURN_IF_NOT_FOUND_STATUS(status, string_on_error) \
    do { \
        if (status == EFI_NOT_FOUND) { \
            RETURN_IF_NOT_SUCCESS( \
                status, \
                string_on_error \
            ); \
        } \
    } while(0);

#define RETURN_IF_OUT_OF_RESOURCSE_STATUS(status, string_on_error) \
    do { \
        if (status == EFI_OUT_OF_RESOURCES) { \
            RETURN_IF_NOT_SUCCESS( \
                status, \
                string_on_error \
            ); \
    } \
} while(0);

EFI_STATUS check_allocate_pool_status(EFI_STATUS status);

#endif //STATUS_LIB_H
