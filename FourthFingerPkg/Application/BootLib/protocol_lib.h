#ifndef PROTOCOL_LIB_H
#define PROTOCOL_LIB_H

EFI_STATUS close_protocols_and_free_pool(
    EFI_HANDLE *handles,
    UINTN number_of_protocols,
    EFI_GUID *protocolGuid,
    EFI_HANDLE imageHandle,
    VOID **protocols
);

EFI_STATUS get_protocol_from_handles(
    EFI_HANDLE imageHandle,
    EFI_HANDLE **handles,
    VOID **protocols,
    UINTN *number_of_handles,
    UINTN *number_of_protocols,
    EFI_GUID *protocolGuid
);

#endif //PROTOCOL_LIB_H
