#ifndef TEXT_INPUT_LIB_H
#define TEXT_INPUT_LIB_H

EFI_STATUS get_text_input_handles(
    EFI_HANDLE imageHandle,
    EFI_HANDLE** handles,
    EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL** text_input_protocols,
    UINTN* text_input_buffer_size
);

EFI_STATUS print_input_text_handle_names();

#endif //TEXT_INPUT_LIB_H
