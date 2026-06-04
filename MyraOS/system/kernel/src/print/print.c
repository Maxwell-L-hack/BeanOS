//
// Created by dvir on 5/19/25.
//

#include "print/print.h"

#include <stdint.h>

#include "vga/vga.h"
#include "vga/vga_color.h"

void kprint(const char *str) { vga_write(str); }

void kprintln(const char *str) {
    vga_write(str);
    vga_write("\n");
}

void kprintf(const char *fmt, ...) {
    va_list argp;
    va_start(argp, fmt);

    vga_write_format(fmt, argp);

    va_end(argp);
}

void kclear_screen(void) { vga_clear_screen(); }

void kclear(size_t n) { vga_clear(n); }

void kset_color(const color_t color) {
    uint8_t vga_color;

    switch (color) {
        case COLOR_WHITE:
            vga_color = VGA_COLOR_WHITE;
            break;
        case COLOR_YELLOW:
            vga_color = VGA_COLOR_YELLOW;
            break;
        case COLOR_GREEN:
            vga_color = VGA_COLOR_GREEN;
            break;
        case COLOR_BLUE:
            vga_color = VGA_COLOR_BLUE;
            break;
        case COLOR_RED:
            vga_color = VGA_COLOR_RED;
            break;
        default:
            vga_color = VGA_COLOR_WHITE;
            break;
    }

    vga_set_color(vga_color);
}
