#ifndef FONT_H
#define FONT_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#include "fb/fb.h"
#include "gfx/gfx.h"

typedef struct font_t {
    uint8_t width;
    uint8_t height;
    const uint8_t* data;
} font_t;

typedef struct cursor_t {
    uint32_t x;
    uint32_t y;
} cursor_t;

typedef struct box_limit {
    uint32_t x, y;
    uint32_t width, height;
} box_limit_t;

typedef struct font_state_t {
    cursor_t cursor;
    font_t* font;
    argb_t color;
    argb_t background_color;
    box_limit_t box_limit;
    bool scrolling_enabled;
} font_state_t;

void font_init_default(font_t* default_font);
void font_restore_default(void);

font_state_t font_save_state(void);
void font_restore_state(font_state_t state);

void font_set_font(font_t* new_font);
font_t* font_get_font(void);

void font_set_box_limit(box_limit_t* box_limit);
box_limit_t font_get_box_limit(void);

void font_set_cursor(cursor_t c);
cursor_t font_get_cursor(void);

void font_set_color(argb_t color);
argb_t font_get_color(void);

void font_set_background_color(argb_t color) ;
argb_t font_get_background_color(void);

void font_set_scrolling(bool enabled);
void font_scroll(layer_id_t layer, uint32_t lines);

void font_write_char(char c, layer_id_t layer);
void font_write_char_at(layer_id_t layer, char c, uint32_t x, uint32_t y);
void font_write(const char* str, layer_id_t layer);
void font_writef(const char* fmt, layer_id_t layer, ...);
void font_write_format(const char* fmt, layer_id_t layer, va_list ap); 
void font_clear(layer_id_t layer, argb_t color);

#endif // FONT_H