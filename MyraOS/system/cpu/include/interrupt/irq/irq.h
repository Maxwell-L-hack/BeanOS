//
// Created by Emily Gofeld on 20/05/2025.
// Edited by Dvir Biton
//

#ifndef IRQ_H
#define IRQ_H

#include "interrupt/idt/idt.h"

#define IRQ_SIZE 16
#define IRQ_INTERRUPT_OFFSET 32

typedef void (*irq_call_t)(registers_t*);

extern irq_call_t irq_routines[IRQ_SIZE];

extern void (*const irq_stubs[IRQ_SIZE])(void);

// init
extern void irq_install_handler(int irq, void (*handler)(registers_t* regs));

extern void irq_init(void);

// handler
extern void irq_handler(registers_t* regs);

extern void irq_remap(void);

extern void _irq0();
extern void _irq1();
extern void _irq2();
extern void _irq3();
extern void _irq4();
extern void _irq5();
extern void _irq6();
extern void _irq7();
extern void _irq8();
extern void _irq9();
extern void _irq10();
extern void _irq11();
extern void _irq12();
extern void _irq13();
extern void _irq14();
extern void _irq15();

#endif  // IRQ_H
