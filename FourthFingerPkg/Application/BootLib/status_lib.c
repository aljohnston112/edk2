#include "status_lib.h"

EFI_STATUS check_allocate_pool_status(const EFI_STATUS status) {
    RETURN_IF_INVALID_PARAMETER_STATUS(
        status,
        "Pool allocation error"
    );
    if (status == EFI_OUT_OF_RESOURCES) {
        RETURN_IF_NOT_SUCCESS(
            status,
            "Could not allocate pool; out of resources"
        );
    }
    return EFI_SUCCESS;
}
