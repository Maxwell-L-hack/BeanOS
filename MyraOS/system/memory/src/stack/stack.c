#include "stack/stack.h"

#include <stddef.h>

#include "pmm/pmm.h"
#include "vmm/vmm.h"

uint32_t kstack_base;
uint32_t kstack_top;

static inline void switch_stack(uint32_t top, void (*entry)(void)) {
    __asm__ volatile(
        "cli\n\t"
        "mov %0, %%esp\n\t"
        "mov %0, %%ebp\n\t"
        "jmp *%1\n\t"
        :
        : "r"(top), "r"(entry)
        : "memory"
    );

    __builtin_unreachable();
}

void stack_init(uint32_t base_va, uint32_t size_bytes, void (*entry)(void)) {
    uint32_t pages = (size_bytes + PAGE_SIZE - 1) / PAGE_SIZE;

    for (uint32_t i = 0; i < pages; i++) {
        uint32_t frame = (uint32_t) pmm_alloc_page();
        vmm_map_page(base_va + i * PAGE_SIZE, frame, PAGE_PRESENT | PAGE_WRITE);
    }

    kstack_base = base_va;
    kstack_top  = base_va + pages * PAGE_SIZE;

    switch_stack(kstack_top, entry);

    __builtin_unreachable();
}