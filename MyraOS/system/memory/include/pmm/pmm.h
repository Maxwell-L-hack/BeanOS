#ifndef PMM_H
#define PMM_H

#include <stddef.h>
#include <stdint.h>

#include "constants/mem_constants.h"

#define BITMAP_SIZE PAGES_AMOUNT / 8

#define BIOS_PART 512

// init
void pmm_init(void);

// allocate
void* pmm_alloc_page(void);

// free
void pmm_free_page(void* addr);

// get free amount
size_t pmm_get_free_page_amount(void);

#endif  // PMM_H
