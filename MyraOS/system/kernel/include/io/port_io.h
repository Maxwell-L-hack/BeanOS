#ifndef PORT_IO_H
#define PORT_IO_H

#include <stdint.h>

// byte
void outb(uint16_t port, uint8_t data);

uint8_t inb(uint16_t port);

// word
void outw(uint16_t port, uint16_t value);

uint16_t inw(uint16_t port);

// long
void outl(uint16_t port, uint32_t value);

uint32_t inl(uint16_t port);

#endif // PORT_IO_H