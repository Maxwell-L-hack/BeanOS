#include "input/input.h"
#include "keyboard/keyboard.h"
#include "keyboard/make_codes.h"
#include "tty/tty.h"

void input_process(void) {
    key_event_t e;
    if (!keyboard_read_event(&e)) {
        return;
    }

    if (tty_get_flags() & TTYF_ICANON) {
        if (e.pressed) {
            if ((unsigned char)e.ascii_value >= 0x20) { 
                tty_handle_printable((uint8_t)e.ascii_value);
                return;
            }

            switch (e.make_code) {
                case KEY_ENTER:     tty_handle_enter();     break;
                case KEY_BACKSPACE: tty_handle_backspace(); break;
                case KEY_TAB:       tty_push_byte('\t');    break;
                case KEY_ESC:       tty_push_byte(0x1B);    break;
                default: break;
            }
        }
        return;
    }

    uint8_t code = ((uint8_t)e.make_code) & 0x7F;
    uint8_t b = code | (e.pressed ? 0x00 : 0x80);
    tty_push_byte(b);
}

