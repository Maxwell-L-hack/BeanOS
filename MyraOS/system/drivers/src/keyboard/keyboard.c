#include "keyboard/keyboard.h"

#include "interrupt/irq/irq.h"
#include "io/port_io.h"
#include "print/print.h"

circular_buffer_t keyboard_buffer;

static bool extended = false;

static uint8_t is_shift_pressed = 0;
static uint8_t is_ctrl_pressed = 0;
static uint8_t is_alt_pressed = 0;

static bool is_caps_lock_on = false;

static key_event_t create_key_event(uint8_t scan_code, bool released);
static void update_modifier_state(uint8_t make_code, bool released);

void keyboard_driver_install(void) {
    cb_init(&keyboard_buffer, sizeof(key_event_t), KEYBOARD_BUFFER_CAPACITY);

    irq_install_handler(KEYBOARD_IRQ, keyboard_handler);
}

void keyboard_handler(registers_t *regs) {
    // regs are pushed to the stack from the isr, but unneeded here
    (void)regs;

    const uint8_t scan_code = inb(0x60);

    if (scan_code == 0xE0) {
        extended = true;
        return;
    }

    // get the make code, scan code - without the msb
    uint8_t make_code = scan_code & 0x7F;
    bool released = scan_code & 0x80;

    if (extended) {
        extended = false;
    }

    if (make_code == KEY_CAPS_LOCK && !released) {
        is_caps_lock_on = !is_caps_lock_on;
        return;
    }

    update_modifier_state(make_code, released);
    
    key_event_t event = create_key_event(scan_code, released);
    cb_write(&keyboard_buffer, &event);
}

bool keyboard_read_event(key_event_t *out) {
    return cb_read(&keyboard_buffer, out);
}

void keyboard_read_blocking(void) {
    key_event_t event;

    while (true) {
        if (keyboard_read_event(&event)) {
            if (event.ascii_value != 0) {
                break;
            }
        }

        __asm__ volatile ("hlt");
    }
}

static void update_modifier_state(uint8_t make_code, bool released) {
    if (make_code == KEY_LEFT_SHIFT || make_code == KEY_RIGHT_SHIFT) {
        released ? is_shift_pressed-- : is_shift_pressed++;
    } else if (make_code == KEY_LEFT_CTRL || (make_code == KEY_RIGHT_CTRL && extended)) {
        released ? is_ctrl_pressed-- : is_ctrl_pressed++;
    } else if (make_code == KEY_LEFT_ALT) {
        released ? is_alt_pressed-- : is_alt_pressed++; 
    }
}

static key_event_t create_key_event(uint8_t scan_code, bool released) {
    key_event_t e;

    e.make_code = scan_code & 0x7F;
    e.pressed = !released;
    e.extended = extended;
    e.ctrl = is_ctrl_pressed;
    e.shift = is_shift_pressed;
    e.alt = is_alt_pressed;

    char ch = 0;
    if (!is_shift_pressed) {
        ch = keyboard_scan_codes[e.make_code];

        if (is_caps_lock_on && ch >= 'a' && ch <= 'z') {
            ch -= 32; // convert to uppercase
        } 
    } else {
        ch = keyboard_shift_scan_codes[e.make_code];

        if (is_caps_lock_on && ch >= 'A' && ch <= 'Z') {
            ch += 32; // convert to lowercase
        } 
    }

    if (is_ctrl_pressed) {
        // convert to control char form
        if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
            ch = (ch & 0x1F);
        } else {
            ch = 0;
        }
    }

    e.ascii_value = ch;

    return e;
}
