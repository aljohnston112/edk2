#ifndef EFI_MEMORY_TYPE_H
#define EFI_MEMORY_TYPE_H

#include <Library/UefiLib.h>

typedef struct {
    EFI_MEMORY_DESCRIPTOR **memory_descriptor;
    UINT64 size;
} efi_memory_type;

typedef efi_memory_type efi_reserved_memory_type;
typedef efi_memory_type efi_loader_code;
typedef efi_memory_type efi_loader_data;
typedef efi_memory_type efi_boot_services_code;
typedef efi_memory_type efi_boot_services_data;
typedef efi_memory_type efi_runtime_services_code;
typedef efi_memory_type efi_runtime_services_data;
typedef efi_memory_type efi_conventional_memory;
typedef efi_memory_type efi_unusable_memory;
typedef efi_memory_type efi_acpi_reclaim_memory;
typedef efi_memory_type efi_acpi_memory_nvs;
typedef efi_memory_type efi_memory_mapped_io;
typedef efi_memory_type efi_memory_mapped_io_port_space;
typedef efi_memory_type efi_pal_code;
typedef efi_memory_type efi_persistent_memory;

inline void add_memory_descriptor(
    efi_memory_type *memory_struct,
    EFI_MEMORY_DESCRIPTOR *memory_descriptor
) {
    memory_struct->memory_descriptor[memory_struct->size++] = memory_descriptor;
}

typedef struct {
    efi_reserved_memory_type *reserved_memory;
    efi_loader_code *loader_code;
    efi_loader_data *loader_data;
    efi_boot_services_code *boot_services_code;
    efi_boot_services_data *boot_services_data;
    efi_runtime_services_code *runtime_services_code;
    efi_runtime_services_data *runtime_services_data;
    efi_conventional_memory *conventional_memory;
    efi_unusable_memory *unusable_memory;
    efi_acpi_reclaim_memory *acpi_reclaim_memory;
    efi_acpi_memory_nvs *acpi_memory_nvs;
    efi_memory_mapped_io *memory_mapped_io;
    efi_memory_mapped_io_port_space *memory_mapped_io_port_space;
    efi_pal_code *pal_code;
    efi_persistent_memory *persistent_memory;
} efi_memory_descriptors;

#endif //EFI_MEMORY_TYPE_H
