#ifndef SYSCALL_H
#define SYSCALL_H

#include "interrupt/idt/idt.h"

#define SYSCALL_INTERRUPT 0x80
#define SYSCALL_MAX 512

void syscall_init(void);

void syscall_handler(const registers_t* regs);

#endif // SYSCALL_H