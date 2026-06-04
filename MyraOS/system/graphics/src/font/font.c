#include "font/font.h"

#include <stdbool.h>

#include "gfx/gfx.h"
#include "libc_kernel/stdlib.h"
#include "libc_kernel/string.h"

#define FONT_FIRST_READABLE_ASCII 32
#define FONT_ASCII_COUNT 95

#define FONT_DEFAULT_BACKGROUND 0xFF222222
#define FONT_DEFAULT_COLOR 0xFFFFFFFF
#define FONT_DEFAULT_LOCATION 50

static enum {
    FONT_ANSI_NONE,
    FONT_ANSI_ESC,
    FONT_ANSI_CSI
} font_ansi_state = FONT_ANSI_NONE;

static bool font_handle_escape_char(layer_id_t layer, char c);
static void font_apply_ansi_color(const char* code);

static char font_ansi_buf[16];
static size_t font_ansi_len = 0;
static font_state_t font_state;

void font_init_default(font_t* default_font) {
    box_limit_t box_limit;
    box_limit.x = FONT_DEFAULT_LOCATION;
    box_limit.y = FONT_DEFAULT_LOCATION;
    box_limit.height = fb_info.height;
    box_limit.width = fb_info.width;
    font_set_box_limit(&box_limit);
    font_set_font(default_font);
    font_set_background_color(FONT_DEFAULT_BACKGROUND);
    font_set_color(FONT_DEFAULT_COLOR);
    font_set_cursor((cursor_t) {FONT_DEFAULT_LOCATION, FONT_DEFAULT_LOCATION});
    font_set_scrolling(false);
}

void font_restore_default(void) {
    box_limit_t box_limit;
    box_limit.x = FONT_DEFAULT_LOCATION;
    box_limit.y = FONT_DEFAULT_LOCATION;
    box_limit.height = fb_info.height;
    box_limit.width = fb_info.width;
    font_set_box_limit(&box_limit);
    font_set_background_color(FONT_DEFAULT_BACKGROUND);
    font_set_color(FONT_DEFAULT_COLOR);
    font_set_cursor((cursor_t) {FONT_DEFAULT_LOCATION, FONT_DEFAULT_LOCATION});
    font_set_scrolling(false);
}

font_state_t font_save_state(void) {
    return font_state;
}

void font_restore_state(font_state_t state) {
    font_state = state;
}

void font_set_font(font_t* new_font) {
    font_state.font = new_font;
}

font_t* font_get_font(void) {
    return font_state.font;
}

void font_set_box_limit(box_limit_t* box_limit) {
    font_state.box_limit = (box_limit_t) {
        .height = box_limit->height,
        .width = box_limit->width,
        .x = box_limit->x,
        .y = box_limit->y,
    };
}

box_limit_t font_get_box_limit(void) {
    return font_state.box_limit;
}

void font_set_cursor(cursor_t c) {
    font_state.cursor = c;
}

cursor_t font_get_cursor(void) {
    return font_state.cursor;
}

void font_set_color(argb_t color) {
    font_state.color = color;
}

argb_t font_get_color(void) {
    return font_state.color;
}

void font_set_background_color(argb_t color) {
    font_state.background_color = color;
}

argb_t font_get_background_color(void) {
    return font_state.background_color;
}

void font_set_scrolling(bool enabled) {
    font_state.scrolling_enabled = enabled;
}

void font_scroll(layer_id_t layer, uint32_t lines) {
    if (!font_state.scrolling_enabled) {
        return;
    }

    uint32_t dy = lines * font_state.font->height;
    for (uint32_t y = font_state.box_limit.y; y < font_state.box_limit.y + font_state.box_limit.height - dy; y++) {
        for (uint32_t x = font_state.box_limit.x; x < font_state.box_limit.x + font_state.box_limit.width; x++) {
            gfx_draw_pixel(layer, x, y, gfx_get_pixel(x, y + dy));
        }
    }

    // Clear the new space at the bottom
    for (uint32_t y = font_state.box_limit.y + font_state.box_limit.height - dy;
         y < font_state.box_limit.y + font_state.box_limit.height; y++) {
        for (uint32_t x = font_state.box_limit.x; x < font_state.box_limit.x + font_state.box_limit.width; x++) {
            gfx_draw_pixel(layer, x, y, font_state.background_color);
        }
    }
}

void font_write_char(char c, layer_id_t layer) {
    // Handle ANSI escape sequences
    if (font_ansi_state != FONT_ANSI_NONE) {
        if (font_ansi_state == FONT_ANSI_ESC && c == '[') {
            font_ansi_state = FONT_ANSI_CSI;
            font_ansi_len = 0;
            return;
        }

        if (font_ansi_state == FONT_ANSI_CSI) {
            if ((c >= '0' && c <= '9') || c == ';') {
                if (font_ansi_len < sizeof(font_ansi_buf) - 1) {
                    font_ansi_buf[font_ansi_len++] = c;
                }
                return;
            } else {
                font_ansi_buf[font_ansi_len] = 0;
                if (c == 'm') {
                    font_apply_ansi_color(font_ansi_buf);
                } else if (c == 'J') {
                    font_clear(layer, font_state.background_color);
                }
                font_ansi_state = FONT_ANSI_NONE;
                return;
            }
        }

        font_ansi_state = FONT_ANSI_NONE;
        return;
    }

    if (c == '\x1b') {
        font_ansi_state = FONT_ANSI_ESC;
        return;
    }

    if (font_handle_escape_char(layer, c)) {
        return;
    }

    uint32_t char_width = font_state.font->width;
    uint32_t char_height = font_state.font->height;
    
    if (font_state.cursor.x + char_width > font_state.box_limit.x + font_state.box_limit.width) {
        font_state.cursor.x = font_state.box_limit.x;
        font_state.cursor.y += char_height;
        
        if (font_state.cursor.y + char_height > font_state.box_limit.y + font_state.box_limit.height) {
            font_scroll(layer, 1);
            font_state.cursor.y -= char_height;
        }
    }

    uint32_t bytes_per_row = (char_width + 7) / 8;
    if ((uint8_t)c < FONT_FIRST_READABLE_ASCII || (uint8_t)c >= FONT_FIRST_READABLE_ASCII + FONT_ASCII_COUNT) {
        return;
    }

    uint8_t char_index = (uint8_t)c - FONT_FIRST_READABLE_ASCII;
    const uint8_t* char_bitmap = &font_state.font->data[char_index * char_height * bytes_per_row];

    for (uint32_t y = 0; y < char_height; y++) {
        for (uint32_t x = 0; x < char_width; x++) {
            uint32_t byte_index = y * bytes_per_row + (x / 8);
            uint8_t bit_mask = 0x80 >> (x % 8);
            if (char_bitmap[byte_index] & bit_mask) {
                gfx_draw_pixel(layer, font_state.cursor.x + x, font_state.cursor.y + y, font_state.color);
            }
        }
    }

    font_state.cursor.x += char_width;
}

void font_write_char_at(layer_id_t layer, char c, uint32_t x, uint32_t y) {
    cursor_t prev = font_state.cursor;

    font_set_cursor((cursor_t){x, y});
    font_write_char(layer, c);

    font_set_cursor(prev);
}

void font_write(const char* str, layer_id_t layer) {
    while (*str) {
        char c = *str++;
        font_write_char(c, layer);
    }
}

void font_writef(const char* fmt, layer_id_t layer, ...) {
    va_list args;
    va_start(args, layer);
    font_write_format(fmt, layer, args);
    va_end(args);
}

void font_write_format(const char* fmt, layer_id_t layer, va_list ap) {
    while (*fmt) {
        if (*fmt != '%') {
            font_write_char(*fmt++, layer);
            continue;
        }

        fmt++; // Skip '%'

        bool zero_pad = false;
        int pad_width = 0;

        if (*fmt == '0') {
            zero_pad = true;
            fmt++;
        }

        while (*fmt >= '0' && *fmt <= '9') {
            pad_width = pad_width * 10 + (*fmt - '0');
            fmt++;
        }

        char buf[32];

        switch (*fmt) {
            case 'd': {
                int val = va_arg(ap, int);
                kitoa(val, buf, 10);

                int len = kstrlen(buf);
                for (int i = 0; i < pad_width - len; i++) {
                    font_write_char( zero_pad ? '0' : ' ', layer);
                }

                font_write(buf, layer);
                break;
            }
            case 'x': {
                uint32_t val = va_arg(ap, uint32_t);
                kutoa(val, buf, 16);

                int len = kstrlen(buf);
                for (int i = 0; i < pad_width - len; i++) {
                    font_write_char(zero_pad ? '0' : ' ', layer);
                }

                font_write(buf, layer);
                break;
            }
            case 'u': {
                uint32_t val = va_arg(ap, uint32_t);
                kutoa(val, buf, 10);

                int len = kstrlen(buf);
                for (int i = 0; i < pad_width - len; i++) {
                    font_write_char(zero_pad ? '0' : ' ', layer);
                }

                font_write(buf, layer);
                break;

            }
            case 's': {
                const char* str = va_arg(ap, const char*);
                font_write(str, layer);
                break;
            }
            case 'c': {
                char c = (char)va_arg(ap, int);
                font_write_char(c, layer);
                break;
            }
            case '%': {
                font_write_char('%', layer);
                break;
            }
            default: {
                font_write_char('%', layer);
                font_write_char(*fmt, layer);

                break;
            }
        }

        fmt++;
    }
}

void font_clear(layer_id_t layer, argb_t color) {
    for (uint32_t y = font_state.box_limit.y; y < font_state.box_limit.y + font_state.box_limit.height; y++) {
        for (uint32_t x = font_state.box_limit.x; x < font_state.box_limit.x + font_state.box_limit.width; x++) {
            gfx_draw_pixel(x, y, color, layer);
        }
    }
}

static bool font_handle_escape_char(layer_id_t layer, char c) {
    switch (c) {
        case '\n': {
            font_state.cursor.x = font_state.box_limit.x;
            font_state.cursor.y += font_state.font->height;

            if (font_state.cursor.y + font_state.font->height > font_state.box_limit.y + font_state.box_limit.height) {
                font_scroll(layer, 1);
                font_state.cursor.y -= font_state.font->height;
            }

            return true;
        }
        case '\t': {
            uint32_t tab_size = 4;
            uint32_t next_tab = (font_state.cursor.x + tab_size) & ~(tab_size - 1);
            
            while (font_state.cursor.x < next_tab) {
                font_write_char(' ', layer);
            }

            return true;
        }
        default:
            return false;
    }
}

static void font_apply_ansi_color(const char* code) {
    int val = katoi(code);

    switch (val) {
        case 0:  font_state.color = 0xFFFFFFFF; break; // Reset to white
        case 30: font_state.color = 0xFF000000; break; // Black
        case 31: font_state.color = 0xFFFF0000; break; // Red
        case 32: font_state.color = 0xFF00FF00; break; // Green
        case 33: font_state.color = 0xFFFFFF00; break; // Yellow
        case 34: font_state.color = 0xFF0000FF; break; // Blue
        case 35: font_state.color = 0xFFFF00FF; break; // Magenta
        case 36: font_state.color = 0xFF00FFFF; break; // Cyan
        case 37: font_state.color = 0xFFFFFFFF; break; // White
    }
}
