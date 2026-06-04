#ifndef VGA_H
#define VGA_H

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

#define VGA_VIDEO_MEMORY (uint16_t*) 0xC00B8000

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

extern volatile uint16_t *video_memory;
extern volatile uint8_t color;

// clear
void vga_clear_screen(void);

void vga_clear(size_t n);

// write
void vga_put_char(const char c);

void vga_write(const char *str);

void vga_write_format(const char *fmt, const va_list argp);

void vga_write_int(const int num);

void vga_write_hex(const uint32_t num);

// colors
void vga_set_color(uint8_t new_color);

// cursor
void vga_set_cursor(uint16_t cursor_pos);

uint16_t vga_get_cursor(void);

// scroll
void vga_scroll(uint8_t lines);

#endif  // VGA_H