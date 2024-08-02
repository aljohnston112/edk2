#ifndef HANDLE_LIB_H
#define HANDLE_LIB_H

EFI_STATUS free_handle_pool(EFI_HANDLE *handles);

EFI_STATUS get_handles(
    EFI_HANDLE **handles,
    UINTN *buffer_size,
    EFI_GUID *protocol
);

#endif //HANDLE_LIB_H
