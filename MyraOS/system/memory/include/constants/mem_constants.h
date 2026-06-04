#ifndef MEM_CONSTANTS_H
#define MEM_CONSTANTS_H

#define TOTAL_MEMORY (1024 * 1024 * 1024)        // 1 GB
#define PAGE_SIZE 4096                           // 4 KB
#define PAGES_AMOUNT (TOTAL_MEMORY / PAGE_SIZE)  // 262,144
#define PAGE_MASK (~(PAGE_SIZE - 1))

#define PAGE_ENTRIES 1024

#define BYTE_SIZE 8

#define KERNEL_PMA 0x100000
#define KERNEL_VMA 0xC0000000

// Alignment macros
#define PAGE_ALIGN_UP(addr)      (((addr) + PAGE_SIZE - 1) & PAGE_MASK)
#define PAGE_ALIGN_DOWN(addr)    ((addr) & PAGE_MASK)
#define IS_PAGE_ALIGNED(addr)    (((addr) & (PAGE_SIZE - 1)) == 0)

// For other common alignments
#define CACHE_LINE_SIZE 64
#define ALIGN_UP(val, align) (((val) + (align) - 1) & ~((align) - 1))
#define ALIGN_DOWN(val, align) ((val) & ~((align) - 1))

#endif // MEM_CONSTANTS_H