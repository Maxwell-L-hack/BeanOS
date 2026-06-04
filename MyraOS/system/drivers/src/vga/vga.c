#include "vga/vga.h"

#include <stdbool.h>
#include <stddef.h>

#include "io/port_io.h"
#include "vga/vga_color.h"
#include "libc_kernel/stdlib.h"
#include "libc_kernel/string.h"

static enum {
    ANSI_NONE,
    ANSI_ESC,
    ANSI_CSI
} ansi_state = ANSI_NONE;

static char ansi_buf[16];
static size_t ansi_len = 0;

volatile uint16_t *video_memory = VGA_VIDEO_MEMORY;
volatile uint8_t color = VGA_COLOR_WHITE;

static bool check_for_escape_chars(const uint16_t c, uint16_t cursor_pos);
static void apply_ansi_color(const char* code);

void vga_clear_screen(void) {
    const uint16_t blank = ' ' | color << 8;

    for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        video_memory[i] = blank;
    }

    vga_set_cursor(0);
}

void vga_clear(size_t n) {
    const uint16_t blank = ' ' | color << 8;
    uint16_t cursor_pos = vga_get_cursor();

    while (n-- && cursor_pos > 0) {
        vga_set_cursor(--cursor_pos);
        video_memory[cursor_pos] = blank;
    }
}

void vga_put_char(const char c) {
    static uint16_t cursor_pos;

    if (ansi_state != ANSI_NONE) {
        if (ansi_state == ANSI_ESC && c == '[') {
            ansi_state = ANSI_CSI;
            ansi_len = 0;
            return;
        }

        if (ansi_state == ANSI_CSI) {
            if ((c >= '0' && c <= '9') || c == ';') {
                if (ansi_len < sizeof(ansi_buf) - 1) {
                    ansi_buf[ansi_len++] = c;
                }
                return;
            } else {
                ansi_buf[ansi_len] = 0;

                if (c == 'm') {
                    apply_ansi_color(ansi_buf);
                } else if (c == 'J') {
                    vga_clear_screen();
                }

                ansi_state = ANSI_NONE;
                return;
            }
        }

        ansi_state = ANSI_NONE;
        return;
    }

    if (c == '\x1b') {
        ansi_state = ANSI_ESC;
        return;
    }

    cursor_pos = vga_get_cursor();
    const uint16_t char_with_color = c | color << 8;

    if (cursor_pos == VGA_WIDTH * VGA_HEIGHT) {
        vga_scroll(1);
        cursor_pos -= VGA_WIDTH;
    }

    if (check_for_escape_chars(c, cursor_pos)) {
        return;
    }

    video_memory[cursor_pos] = char_with_color;
    vga_set_cursor(cursor_pos + 1);
}

void vga_write(const char *str) {
    for (size_t i = 0; str[i] != 0; i++) {
        vga_put_char(str[i]);
    }
}

void vga_write_format(const char *fmt, const va_list argp) {
    while (*fmt) {
        if (*fmt != '%') {
            vga_put_char(*fmt++);
            continue;
        }

        fmt++; // Skip '%'

        bool zero_pad = false;
        int pad_width = 0;

        if (*fmt == '0') {
            zero_pad = true;
            fmt++;
        }

        // Parse width digits (e.g., 2 in %02d)
        while (*fmt >= '0' && *fmt <= '9') {
            pad_width = pad_width * 10 + (*fmt - '0');
            fmt++;
        }

        char buf[32];

        switch (*fmt) {
            case 'd': {
                int val = va_arg(argp, int);
                
                kitoa(val, buf, 10);
                int len = kstrlen(buf);

                for (int i = 0; i < pad_width - len; i++) {
                    vga_put_char(zero_pad ? '0' : ' ');
                }
                    
                vga_write(buf);
                break;
            }
            case 'x': {
                uint32_t val = va_arg(argp, uint32_t);

                kutoa(val, buf, 16);
                int len = kstrlen(buf);

                for (int i = 0; i < pad_width - len; i++) {
                    vga_put_char(zero_pad ? '0' : ' ');
                }
                    
                vga_write(buf);
                break;
            }
            case 'u': {
                uint32_t val = va_arg(argp, uint32_t);
                kutoa(val, buf, 10);
                int len = kstrlen(buf);

                for (int i = 0; i < pad_width - len; i++) {
                    vga_put_char(zero_pad ? '0' : ' ');
                }
                    
                vga_write(buf);
                break;
            }
            case 's': {
                const char *str = va_arg(argp, const char *);
                vga_write(str);
                break;
            }
            case 'c': {
                char c = (char)va_arg(argp, int);
                vga_put_char(c);
                break;
            }
            case '%': {
                vga_put_char('%');
                break;
            }
            default: {
                vga_put_char('%');
                vga_put_char(*fmt);
                break;
            }
        }

        fmt++;
    }
}

void vga_write_int(int num) {
    char buf[12];
    int i = 0;
    bool is_negative = false;

    if (num == 0) {
        vga_put_char('0');
        return;
    }

    if (num < 0) {
        is_negative = true;
        num = -num;
    }

    while (num > 0) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }

    if (is_negative) {
        buf[i++] = '-';
    }

    // reverse the buffer
    while (--i >= 0) {
        vga_put_char(buf[i]);
    }
}

void vga_write_hex(const uint32_t num) {
    vga_write("0x");

    bool started = false;
    for (int i = 28; i >= 0; i -= 4) {
        uint8_t nibble = (num >> i) & 0xF;

        if (nibble == 0 && !started && i != 0) {
            // skip leading zeroes
            continue;
        }

        started = true;
        if (nibble < 10) {
            vga_put_char('0' + nibble);
        } else {
            vga_put_char('A' + (nibble - 10));
        }
    }

    if (!started) {
        vga_put_char('0');
    }
}

static bool check_for_escape_chars(const uint16_t c, uint16_t cursor_pos) {
    switch (c) {
        case '\n':
            cursor_pos = cursor_pos + (VGA_WIDTH - cursor_pos % VGA_WIDTH);
            break;
        case '\t':
            cursor_pos += 4 - (cursor_pos % 4);
            break;
        default:
            return false;
    }

    vga_set_cursor(cursor_pos);
    return true;
}

static void apply_ansi_color(const char* code) {
    int val = katoi(code);

    switch (val) {
        case 0:  vga_set_color(VGA_COLOR_LIGHT_GREY); break; // reset
        case 30: vga_set_color(VGA_COLOR_BLACK); break;
        case 31: vga_set_color(VGA_COLOR_RED); break;
        case 32: vga_set_color(VGA_COLOR_GREEN); break;
        case 33: vga_set_color(VGA_COLOR_BROWN); break;
        case 34: vga_set_color(VGA_COLOR_BLUE); break;
        case 35: vga_set_color(VGA_COLOR_MAGENTA); break;
        case 36: vga_set_color(VGA_COLOR_CYAN); break;
        case 37: vga_set_color(VGA_COLOR_WHITE); break;
    }
}

void vga_set_color(const uint8_t new_color) { color = new_color; }

void vga_set_cursor(const uint16_t cursor_pos) {
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(cursor_pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)(cursor_pos >> 8 & 0xFF));
}

uint16_t vga_get_cursor(void) {
    uint16_t cursor_pos = 0;

    outb(0x3D4, 0x0F);
    cursor_pos |= inb(0x3D5);
    outb(0x3D4, 0x0E);
    cursor_pos |= (uint16_t)inb(0x3D5) << 8;

    return cursor_pos;
}

void vga_scroll(const uint8_t lines) {
    // clear first n lines
    const uint16_t blank = ' ' | color << 8;
    for (size_t i = 0; i < lines * VGA_WIDTH; i++) {
        video_memory[i] = blank;
    }

    // transfer each line to the top free place
    for (size_t i = lines * VGA_WIDTH; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        video_memory[i - lines * VGA_WIDTH] = video_memory[i];
        video_memory[i] = blank;
    }

    vga_set_cursor(vga_get_cursor() - VGA_WIDTH * lines);
}
