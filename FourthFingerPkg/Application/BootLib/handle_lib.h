#ifndef HANDLE_LIB_H
#define HANDLE_LIB_H

EFI_STATUS free_handle_pool(EFI_HANDLE *handles);

EFI_STATUS get_handles(
    EFI_HANDLE **handles,
    UINTN *number_of_handles,
    EFI_GUID *protocol
);

EFI_STATUS get_all_handles(
    EFI_HANDLE image_handle
);

#endif //HANDLE_LIB_H
