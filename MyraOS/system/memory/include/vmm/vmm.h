#ifndef VMM_H
#define VMM_H

#include <stdint.h>

#include "constants/mem_constants.h"

#define RECURSIVE_PAGE_DIRECTORY_BASE 0xFFFFF000
#define RECURSIVE_PAGE_TABLE_BASE 0xFFC00000
#define KERNEL_ENTRY_START 768

#define MAKE_ENTRY(ptr, flags) (((uint32_t)(ptr) & RECURSIVE_PAGE_DIRECTORY_BASE) | ((flags) & 0xFFF))
#define VIRT_TO_PHYS(addr) \
    ( ((*(volatile uint32_t*)(RECURSIVE_PAGE_TABLE_BASE + (((addr) >> 22) << 12) + (((addr) >> 12) & 0x3FF) * 4)) & ~0xFFF)) \
      | ((addr) & 0xFFF) )

// types
typedef uint32_t page_table_entry_t;

typedef uint32_t page_directory_entry_t;

// page dir structs
typedef struct page_table_t {
    page_table_entry_t entries[PAGE_ENTRIES];
} page_table_t;

typedef struct page_directory_t {
    page_directory_entry_t entries[PAGE_ENTRIES];
} page_directory_t;

// flags
typedef enum page_flags_t {
    PAGE_PRESENT       = 0x1,   // Page is present in memory
    PAGE_WRITE         = 0x2,   // Writable (if 0, read-only)
    PAGE_USER          = 0x4,   // User-mode access allowed
    PAGE_WRITE_THROUGH = 0x8,   // Write-through caching
    PAGE_CACHE_DISABLE = 0x10,  // Disable caching
    PAGE_ACCESSED      = 0x20,  // Set by CPU on access
    PAGE_DIRTY         = 0x40,  // Set by CPU on write (only for PTEs)
    PAGE_SIZE_4MB      = 0x80,  // 4MB page size (only for PDEs)
    PAGE_GLOBAL        = 0x100, // Global page (not flushed on CR3 switch)
} page_flags_t;

extern uint32_t* page_directory;
extern uint32_t* kernel_page_directory_phys;
extern page_directory_t* current_page_directory_virt;

// Only sets recursive map, sets global ptr, etc.
void vmm_init(void);        

// Mmap a virtual address to a physical one
void vmm_map_page(uint32_t virtual_addr, uint32_t physical_addr, uint32_t flags);

// Unmap
void vmm_unmap_page(uint32_t virtual_addr);

// Convert between a virtual addr to a physical one
uint32_t vmm_virt_to_phys(uint32_t virtual_addr);

#endif // VMM_H