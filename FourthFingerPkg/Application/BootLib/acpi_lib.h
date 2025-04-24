#ifndef ACPI_LIB_H
#define ACPI_LIB_H

#include "page_lib.h"

void process_acpi_20(
    EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER* table,
    OnError free_on_error
);

#endif //ACPI_LIB_H
