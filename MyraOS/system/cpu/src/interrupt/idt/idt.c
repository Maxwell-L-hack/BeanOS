#include "interrupt/idt/idt.h"

idt_entry_t idt[IDT_SIZE];
idt_descriptor_t idt_descriptor;

void idt_set_gate(const uint8_t index, const uint32_t base, const uint16_t segment, const uint8_t flags) {
    idt[index].offset_low = base & 0xFFFF;
    idt[index].segment_selector = segment;
    idt[index].reserved = 0;
    idt[index].type_attr = flags;
    idt[index].offset_high = (base >> 16) & 0xFFFF;
}

void idt_init(void) {
    idt_descriptor.limit = IDT_ENTRY_SIZE * IDT_SIZE - 1;
    idt_descriptor.idt = idt;

    _idt_load(&idt_descriptor);
}
