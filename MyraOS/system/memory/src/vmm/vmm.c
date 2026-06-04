#include "vmm/vmm.h"

#include "libc_kernel/string.h"
#include "panic/panic.h"
#include "pmm/pmm.h"

extern uint32_t _kernel_start;
extern uint32_t _kernel_end;

#define KERNEL_SIZE_IN_PAGES (((_kernel_end - _kernel_start) + PAGE_SIZE - 1) / PAGE_SIZE)

extern void _load_page_directory(uint32_t page_dir);

uint32_t* kernel_page_directory_phys;
page_directory_t* current_page_directory_virt;

void vmm_init(void) {
    __asm__ volatile("mov %%cr3, %0" : "=r"(kernel_page_directory_phys));
    current_page_directory_virt = (page_directory_t*) RECURSIVE_PAGE_DIRECTORY_BASE;
}

void vmm_map_page(uint32_t virtual_addr, uint32_t physical_addr, uint32_t flags) {
    uint32_t dir_index = (virtual_addr >> 22) & 0x3FF;
    uint32_t table_index = (virtual_addr >> 12) & 0x3FF;

    // in case there is no page table
    if (current_page_directory_virt->entries[dir_index] == 0) {
        page_table_t* new_page_table = (page_table_t*) pmm_alloc_page();
        if (new_page_table == NULL) {
            kpanic("Out of memory while creating page table");
        }

        if (flags & PAGE_USER) {
            current_page_directory_virt->entries[dir_index] = MAKE_ENTRY(new_page_table, PAGE_PRESENT | PAGE_WRITE | PAGE_USER);
        } else {
            current_page_directory_virt->entries[dir_index] = MAKE_ENTRY(new_page_table, PAGE_PRESENT | PAGE_WRITE);
        }
        
        new_page_table = (page_table_t*)(RECURSIVE_PAGE_TABLE_BASE + (dir_index * PAGE_SIZE));
        kmemset(new_page_table, 0, PAGE_SIZE);

        __asm__ volatile("invlpg (%0)" :: "r" (new_page_table));
    }

    page_table_t* page_table = (page_table_t*)(RECURSIVE_PAGE_TABLE_BASE + dir_index * PAGE_SIZE);
    page_table->entries[table_index] = MAKE_ENTRY(physical_addr, flags);

    __asm__ volatile("invlpg (%0)" :: "r" (virtual_addr) : "memory");
}

void vmm_unmap_page(uint32_t virtual_addr) {
    uint32_t dir_index = (virtual_addr >> 22) & 0x3FF;
    uint32_t table_index = (virtual_addr >> 12) & 0x3FF;

    page_table_t* page_table = (page_table_t*)(RECURSIVE_PAGE_TABLE_BASE + dir_index * PAGE_SIZE);
    page_table->entries[table_index] = 0;

    __asm__ volatile("invlpg (%0)" :: "r" (virtual_addr) : "memory");
}

uint32_t vmm_virt_to_phys(uint32_t virtual_addr) {
    uint32_t pd_index = (virtual_addr) >> 22;                        
    uint32_t pt_index = ((virtual_addr) >> 12) & 0x3FF;          

    uint32_t* pt = (uint32_t*)(0xFFC00000 + (pd_index << 12));

    return (pt[pt_index] & ~0xFFF) | ((virtual_addr) & 0xFFF); 
}