#ifndef STACK_H
#define STACK_H

#define STACK_SIZE (1024 * 1024 * 16)
#define STACK_BASE 0xD0000000u 

#include <stdint.h>

extern uint32_t kstack_base;
extern uint32_t kstack_top;

__attribute__((noreturn))
void stack_init(uint32_t base_va, uint32_t size_bytes, void (*entry)(void));

#endif // STACK_H