#ifndef EFI_MEMORY_TYPE_H
#define EFI_MEMORY_TYPE_H

#include <Uefi.h>

typedef struct {
    EFI_MEMORY_DESCRIPTOR **memory_descriptors;
    UINT64 size;
} efi_memory_descriptor_list;

inline void add_memory_descriptor(
    efi_memory_descriptor_list *memory_descriptor_list,
    EFI_MEMORY_DESCRIPTOR *memory_descriptor
) {
    memory_descriptor_list->memory_descriptors[memory_descriptor_list->size++] =
        memory_descriptor;
}

#endif //EFI_MEMORY_TYPE_H
