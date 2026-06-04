#include "gdt/gdt.h"

#include "vmm/vmm.h"

static gdt_entry_t gdt[6];
static gdt_ptr_t   gp;

tss_entry_t tss;

/* defined in gdt_flush.asm */
extern void _gdt_flush(uint32_t);
extern void _tss_flush(uint16_t);

void gdt_init(void) {
    gp.limit = (sizeof(gdt_entry_t) * 6) - 1;
    gp.base  = (uint32_t) &gdt;

    gdt_set_gate(0, 0, 0, 0, 0);                 /* null */

    /* kernel code / data */
    gdt_set_gate(1, 0, 0xFFFFF, 0x9A, 0xCF);     /* 0x08 */
    gdt_set_gate(2, 0, 0xFFFFF, 0x92, 0xCF);     /* 0x10 */

    /* user code / data  */
    gdt_set_gate(3, 0, 0xFFFFF, 0xFA, 0xCF);     /* 0x1B */
    gdt_set_gate(4, 0, 0xFFFFF, 0xF2, 0xCF);     /* 0x23 */

    /* TSS gate (access=0x89, gran=0x00, limit = sizeof(tss)-1) */
    uint32_t tss_base = (uint32_t)&tss;
    gdt_set_gate(5, tss_base, sizeof(tss) - 1, 0x89, 0x00);

    _gdt_flush(vmm_virt_to_phys((uint32_t) &gp));

    tss.ss0  = 0x10; 
    tss.iomap = sizeof(tss);

    _tss_flush(0x28);
}

void gdt_set_gate(int i, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[i].limit      = limit & 0xFFFF;
    gdt[i].base_low   = base  & 0xFFFF;
    gdt[i].base_mid   = (base >> 16) & 0xFF;
    gdt[i].access     = access;
    gdt[i].gran       = ((limit >> 16) & 0x0F) | (gran & 0xF0);
    gdt[i].base_high  = (base >> 24) & 0xFF;
}

void tss_set_kernel_stack(uint32_t esp0) {
    tss.esp0 = esp0;
}
