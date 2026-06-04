//
// Created by dvir on 5/19/25.
//

#ifndef PRINT_H
#define PRINT_H

#include <stdarg.h>
#include <stddef.h>

typedef enum color_t {
    COLOR_BLACK,
    COLOR_BLUE,
    COLOR_GREEN,
    COLOR_CYAN,
    COLOR_RED,
    COLOR_MAGENTA,
    COLOR_BROWN,
    COLOR_LIGHT_GREY,
    COLOR_DARK_GREY,
    COLOR_LIGHT_BLUE,
    COLOR_LIGHT_GREEN,
    COLOR_LIGHT_CYAN,
    COLOR_LIGHT_RED,
    COLOR_LIGHT_MAGENTA,
    COLOR_YELLOW,
    COLOR_WHITE,
} color_t;

// print
void kprint(const char *str);

void kprintln(const char *str);

void kprintf(const char *str, ...);

// clear
void kclear_screen(void);

void kclear(size_t n);

// set color
void kset_color(const color_t color);

#endif  // PRINT_H
