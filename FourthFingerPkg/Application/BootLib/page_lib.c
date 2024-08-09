#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include "console_lib.h"
#include "page_lib.h"
#include "status_lib.h"
#include "text_input_lib.h"


void start_paging(
    OnError free_on_error,
    OnNextPage on_next_page,
    OnDone on_done,
    UINTN number_of_pages
) {
    clear_console();
    UINTN current_page = 0;
    BOOLEAN user_done = FALSE;
    while (user_done == FALSE) {
        BOOLEAN valid_key = FALSE;
        EFI_STATUS status = on_next_page(current_page);
        if (free_on_error(status, "Failed to get protocols for handle")) {
            goto end;
        }
        AsciiPrint("\nPress enter to go back\n");

        if (user_done == FALSE) {
            EFI_INPUT_KEY key;
            while (valid_key == FALSE) {
                status = wait_for_key_and_get_it(&key);
                if (free_on_error(status, "Failed to wait for key")) {
                    goto end;
                }

                if (key.ScanCode == UEFI_KEY_LEFT ||
                    key.ScanCode == UEFI_KEY_RIGHT ||
                    key.UnicodeChar == UNICODE_ENTER
                ) {
                    valid_key = TRUE;
                }
            }

            if (key.ScanCode == UEFI_KEY_LEFT) {
                if (current_page == 0) {
                    current_page = number_of_pages - 1;
                } else {
                    current_page--;
                }
            } else if (key.ScanCode == UEFI_KEY_RIGHT) {
                if (current_page == number_of_pages - 1) {
                    current_page = 0;
                } else {
                    current_page++;
                }
            } else if (key.UnicodeChar == UNICODE_ENTER) {
                user_done = TRUE;
            } else {
                free_on_error(EFI_ABORTED, "Bug in page_through_memory_map");
            }
        }
    }
    end:
        on_done();
}