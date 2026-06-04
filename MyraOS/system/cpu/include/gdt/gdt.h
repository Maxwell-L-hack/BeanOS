#ifndef GDT_H
#define GDT_H

#include <stdint.h>

#define GDT_KERNEL_CODE 0x08  /* index 1, RPL 0 */
#define GDT_KERNEL_DATA 0x10  /* index 2, RPL 0 */
#define GDT_USER_CODE   0x1B  /* index 3, RPL 3 */
#define GDT_USER_DATA   0x23  /* index 4, RPL 3 */

/* ───── GDT entry & pointer ─────────────────────────────────────── */
typedef struct gdt_entry_t {
    uint16_t limit;
    uint16_t base_low;
    uint8_t  base_mid;
    uint8_t  access;
    uint8_t  gran;
    uint8_t  base_high;
} __attribute__((packed)) gdt_entry_t;

typedef struct gdt_ptr_t {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) gdt_ptr_t;

/* ───── 32-bit TSS ─────────────────── */
typedef struct tss_entry_t {
    uint32_t prev;
    uint32_t esp0; uint32_t ss0;
    uint32_t esp1, ss1;
    uint32_t esp2, ss2;
    uint32_t cr3, eip, eflags, eax, ecx, edx, ebx;
    uint32_t esp, ebp, esi, edi;
    uint32_t es, cs, ss, ds, fs, gs;
    uint32_t ldt;
    uint16_t trap, iomap;
} __attribute__((packed)) tss_entry_t;

extern tss_entry_t tss;

void gdt_init(void);

void gdt_set_gate(int i, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

void gdt_set_kernel_stack(uint32_t esp0);

void tss_set_kernel_stack(uint32_t esp0);

#endif
