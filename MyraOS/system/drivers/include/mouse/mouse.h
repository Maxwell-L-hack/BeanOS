#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>
#include <stdbool.h>

#define MOUSE_IRQ 12

#define CURSOR_SIZE 5
#define CURSOR_COLOR 0xFFFFFFFF

extern int32_t prev_mouse_x, prev_mouse_y;
extern int32_t mouse_x, mouse_y;

extern bool cursor_enabled;

void mouse_init(void);

void mouse_write(uint8_t val);
uint8_t mouse_read(void);

void mouse_set(bool is_enabled);

#endif // MOUSE_H