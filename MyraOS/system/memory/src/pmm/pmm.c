#include "pmm/pmm.h"

#include <stdbool.h>

#include "bit_operations/bit_operations.h"
#include "panic/panic.h"

static uint8_t bitmap[BITMAP_SIZE];

static size_t free_page_count;

void pmm_init(void) {
    // init the bitmap
    for (size_t i = 0; i < BITMAP_SIZE; i++) {
        bitmap[i] = 0;
    }

    // mark the bios part as used
    for (size_t i = 0; i < BIOS_PART / BYTE_SIZE; i++) {
        bitmap[i] = 0xFF;
    }

    free_page_count = PAGES_AMOUNT - BIOS_PART;
}

void* pmm_alloc_page(void) {
    if (free_page_count == 0) {
        kpanic("Out of memory");
        return NULL;
    }

    for (size_t i = BIOS_PART; i < BITMAP_SIZE; i++) {
        // check if at least 1 is free (set to 0)
        if (bitmap[i] < 0xFF) {
            for (size_t j = 0; j < BYTE_SIZE; j++) {
                size_t bit_index = i * BYTE_SIZE + j;

                // check if page is free
                if (!test_bit(bitmap, bit_index)) {
                    set_bit(bitmap, bit_index);
                    free_page_count--;

                    return (void *)((uintptr_t)bit_index * PAGE_SIZE);
                }
            }
        }
    }

    kpanic("Out of memory");
    return NULL;
}

void pmm_free_page(void *addr) {
    uintptr_t bitmap_index = (uintptr_t)addr / PAGE_SIZE;

    if (bitmap_index >= PAGES_AMOUNT || bitmap_index < BIOS_PART) {
        return;
    }

    if (test_bit(bitmap, bitmap_index)) {
        free_page_count++;
        clear_bit(bitmap, bitmap_index);
    }
}

size_t pmm_get_free_page_amount(void) { return free_page_count; }
