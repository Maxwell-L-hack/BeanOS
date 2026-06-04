#ifndef PIT_H
#define PIT_H

#include "interrupt/idt/idt.h"

#define PIT_BASE_HZ 1193182
#define PIT_HZ 100

#define PIT_IRQ 0

// init
void pit_init(void);

// handler
void pit_handler(registers_t* regs);

// getter
uint32_t pit_ticks(void);

#endif // PIT_H
