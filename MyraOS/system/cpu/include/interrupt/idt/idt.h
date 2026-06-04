//
// Created by Dvir Biton on 15/05/2025.
//

#ifndef IDT_H
#define IDT_H

#include <stdint.h>

#define IDT_SIZE 256
#define IDT_ENTRY_SIZE sizeof(idt_entry_t)

#define KERNEL_CODE_SEG 0x08
#define KERNEL_DATA_SEG 0x10
#define USER_CODE_SEG 0x18

typedef struct idt_entry_t {
    uint16_t offset_low;
    uint16_t segment_selector;
    uint8_t reserved;
    uint8_t type_attr;
    uint16_t offset_high;
} __attribute__((packed)) idt_entry_t;

typedef struct idt_descriptor_t {
    uint16_t limit;
    idt_entry_t* idt;
} __attribute__((packed)) idt_descriptor_t;

typedef enum idt_flags {
    IDT_FLAG_GATE_TASK = 0x05,
    IDT_FLAG_GATE_TASK_16BIT_INT = 0x06,
    IDT_FLAG_GATE_TASK_16BIT_TRAP = 0x07,
    IDT_FLAG_GATE_TASK_32BIT_INT = 0x0E,
    IDT_FLAG_GATE_TASK_32BIT_TRAP = 0x0F,

    IDT_FLAG_RING0 = (0 << 5),
    IDT_FLAG_RING1 = (1 << 5),
    IDT_FLAG_RING2 = (2 << 5),
    IDT_FLAG_RING3 = (3 << 5),

    IDT_FLAG_PRESENT = 0X80,
} idt_flags;

typedef struct {
    uint32_t gs, fs, es, ds;               // manually pushed
    uint32_t edi, esi, ebp, esp;           // pusha
    uint32_t ebx, edx, ecx, eax;
    uint32_t int_no, err_code;             // manually
    uint32_t eip, cs, eflags, useresp, ss; // CPU pushed
} __attribute__((packed)) registers_t;

// idt setup
void _idt_load(idt_descriptor_t* idt_descriptor);

void idt_set_gate(uint8_t index, uint32_t base, uint16_t segment, uint8_t flags);

void idt_init(void);

// idt
extern idt_entry_t idt[IDT_SIZE];
extern idt_descriptor_t idt_descriptor;

#endif  // IDT_H
