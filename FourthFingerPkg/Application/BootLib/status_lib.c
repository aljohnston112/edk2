#include "status_lib.h"

#include <Library/UefiLib.h>

EFI_STATUS check_allocate_pool_status(const EFI_STATUS status) {
    RETURN_IF_INVALID_PARAMETER_STATUS(
        status,
        "Pool allocation error"
    );
    RETURN_IF_OUT_OF_RESOURCSE_STATUS(
        status,
        "Could not allocate pool; out of resources"
    );
    return EFI_SUCCESS;
}
