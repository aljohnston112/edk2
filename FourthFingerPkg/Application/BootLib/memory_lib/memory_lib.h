#ifndef MEMORY_LIB_H
#define MEMORY_LIB_H

#include "efi_memory_type.h"

EFI_STATUS print_memory_map();

EFI_STATUS free_memory_descriptor_lists();

EFI_STATUS get_memory_map(
    efi_memory_descriptor_list* memory_descriptor_lists,
    UINTN* map_key
);

#endif //MEMORY_LIB_H
