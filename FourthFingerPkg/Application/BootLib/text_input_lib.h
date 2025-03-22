#ifndef TEXT_INPUT_LIB_H
#define TEXT_INPUT_LIB_H

enum Unicode {
    UEFI_KEY_UP = 0x0001,
    UEFI_KEY_DOWN = 0x0002,
    UEFI_KEY_RIGHT = 0x0003,
    UEFI_KEY_LEFT = 0x0004,

    UNICODE_SPACE = 0x20,
    UNICODE_ENTER = 0x0D,

    UNICODE_ZERO = 0x0030,
    UNICODE_ONE = 0x0031,
    UNICODE_TWO = 0x0032,
    UNICODE_THREE = 0x0033,
    UNICODE_FOUR = 0x0034,
    UNICODE_FIVE = 0x0035,
    UNICODE_SIX = 0x0036,
    UNICODE_SEVEN = 0x0037,
    UNICODE_EIGHT = 0x0038,
    UNICODE_NINE = 0x0039
};

EFI_STATUS print_input_text_handle_names();

EFI_STATUS wait_for_key();

EFI_STATUS wait_for_key_and_get_it(
    EFI_INPUT_KEY* key
);

EFI_STATUS wait_for_unicode(
    UINT16 unicode_char
);

EFI_STATUS wait_for_scan_code(
    UINT16 unicode_char
);

EFI_STATUS wait_for_any_key();

#endif //TEXT_INPUT_LIB_H
