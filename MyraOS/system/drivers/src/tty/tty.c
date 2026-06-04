#include "tty/tty.h"
#include "circular_buffer/circular_buffer.h"
#include "print/print.h"

#define ASCII_NULL        0x00
#define ASCII_BEL         0x07
#define ASCII_BACKSPACE   0x08
#define ASCII_TAB         0x09
#define ASCII_LF          0x0A  
#define ASCII_CR          0x0D 
#define ASCII_ESC         0x1B
#define ASCII_DEL         0x7F

static circular_buffer_t q;
static uint32_t flags;
static uint32_t echo_len;

static void echo_byte(uint8_t b) {
    if (!(flags & TTYF_ECHO)) {
        return;
    }

    if (b == ASCII_LF) { 
        kprint("\n");
        echo_len = 0; 

        return; 
    }

    if (b == ASCII_DEL) { 
        if (echo_len) { 
            echo_len--; 
            kclear(1);
        } 

        return;
    }

    if (b >= 0x20) { 
        echo_len++;
        kprintf("%c", b);
    }
}

void tty_init(void) {
    cb_init(&q, sizeof(uint8_t), TTY_BUF_CAP);
    flags = TTYF_ECHO | TTYF_ICRNL;
    echo_len = 0;
}

void tty_set_flags(uint32_t f) { 
    flags = f;
}
uint32_t tty_get_flags(void) { 
    return flags;
}

void tty_push_byte(uint8_t b) { 
    cb_write(&q, &b);
}

void tty_push_seq(const char* s) { 
    while (*s) { 
        uint8_t b = (uint8_t)*s++;
        cb_write(&q, &b);
    } 
}

void tty_handle_printable(uint8_t ch) {
    if (flags & TTYF_ICANON) {
        if (ch >= 0x20) { 
            tty_push_byte(ch);
            echo_byte(ch);
        }
    } else {
        tty_push_byte(ch);
        echo_byte(ch);
    }
}

void tty_handle_enter(void) {
    uint32_t f = tty_get_flags();
    uint8_t b;
    
    if (f & TTYF_ICANON) {
        b = (f & TTYF_ICRNL) ? ASCII_LF : ASCII_CR;
    } else {
        b = ASCII_CR;                                 
    }

    tty_push_byte(b);
    echo_byte(b);
}

void tty_handle_backspace(void) {
    if (flags & TTYF_ICANON) { 
        echo_byte(ASCII_DEL);
        return;
    }

    tty_push_byte(ASCII_DEL);
    echo_byte(ASCII_DEL);
}

size_t tty_read(uint8_t* out, size_t max_len, bool blocking) {
    if (flags & TTYF_ICANON) {
        return tty_readline(out, max_len, blocking);
    }

    size_t n = 0;
    for (;;) {
        uint8_t b;
        if (!cb_read(&q, &b)) {
            if (!blocking || n) {
                break;
            }

            __asm__ volatile("hlt");
            continue;
        }
        out[n++] = b;

        if (n == max_len || !blocking) {
            break;
        }
    }

    return n;
}

size_t tty_readline(uint8_t* out, size_t max_len, bool blocking) {
    if (max_len == 0) {
        return 0;
    }

    size_t n = 0;
    for (;;) {
        uint8_t b;
        if (!cb_read(&q, &b)) {
            if (!blocking) {
                return 0;
            }

            __asm__ volatile("hlt");
            continue;
        }
        if (b == ASCII_DEL || b == ASCII_BACKSPACE) { 
            if (n) {
                n--;
            }
            
            continue;
        }

        if ((flags & TTYF_ICRNL) && b == ASCII_CR) {
            b = ASCII_LF;
        }

        out[n++] = b;

        if (b == ASCII_LF || n == max_len) {
            break;
        }
    }

    return n;
}
