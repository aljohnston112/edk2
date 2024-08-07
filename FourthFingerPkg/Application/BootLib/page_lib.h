#ifndef PAGE_LIB_H
#define PAGE_LIB_H

typedef void (*OnDone)();
typedef BOOLEAN (*OnError)(EFI_STATUS, char*);
typedef EFI_STATUS (*OnNextPage)(UINTN);

void start_paging(
    OnError free_on_error,
    OnNextPage on_next_page,
    OnDone on_done,
    UINTN number_of_pages
);

#endif //PAGE_LIB_H
