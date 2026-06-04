#ifndef TTY_H
#define TTY_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define TTY_BUF_CAP 2048

#define TTYF_ICANON  (1u<<0)
#define TTYF_ECHO    (1u<<1)
#define TTYF_ICRNL   (1u<<2)

void tty_init(void);
void tty_set_flags(uint32_t flags);
uint32_t tty_get_flags(void);

void tty_push_byte(uint8_t b);
void tty_push_seq(const char* s);

size_t tty_read(uint8_t* out, size_t max_len, bool blocking);
size_t tty_readline(uint8_t* out, size_t max_len, bool blocking);

void tty_handle_printable(uint8_t ch);
void tty_handle_enter(void);
void tty_handle_backspace(void);

#endif
