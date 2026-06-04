#include "mouse/mouse.h"

#include <stdbool.h>

#include "fb/fb.h"
#include "gfx/gfx.h"
#include "io/port_io.h"
#include "interrupt/irq/irq.h"
#include "libc_kernel/string.h"
#include "ui/ui.h"

static int8_t mouse_cycle = 0;
static int8_t mouse_bytes[3];

static bool prev_left_btn = false;

int32_t prev_mouse_x = 0, prev_mouse_y = 0;
int32_t mouse_x = 0, mouse_y = 0;

bool cursor_enabled = false;

static void mouse_handler(registers_t* _);
static void handle_click(void);

void mouse_init(void) {
    outb(0x64, 0xA8);
    outb(0x64, 0x20);
    
    uint8_t status = inb(0x60);
    outb(0x64, 0x60);
    outb(0x60, status | 2);

    mouse_write(0xF6); mouse_read();
    mouse_write(0xF4); mouse_read();

    irq_install_handler(MOUSE_IRQ, mouse_handler);
}

void mouse_write(uint8_t val) {
    while (inb(0x64) & 2);
    outb(0x64, 0xD4);
    while (inb(0x64) & 2);
    outb(0x60, val);
}

uint8_t mouse_read(void) {
    while (!(inb(0x64) & 1));
    return inb(0x60);
}

void mouse_set(bool is_enabled) {
    cursor_enabled = is_enabled;
}

static void mouse_handler(registers_t* _) {
    (void) _;

    uint8_t data = inb(0x60);
    mouse_bytes[mouse_cycle++] = data;

    if (mouse_cycle == 3) {
        handle_click();

        int8_t dx = mouse_bytes[1];
        int8_t dy = mouse_bytes[2];
        mouse_x += dx;
        mouse_y -= dy;

        if (mouse_x < 0) {
            mouse_x = 0;
        } else if ((uint32_t) mouse_x >= fb_info.width) {
            mouse_x = fb_info.width - 1;
        }

        if (mouse_y < 0) {
            mouse_y = 0;
        } else if ((uint32_t) mouse_y >= fb_info.height) {
            mouse_y = fb_info.height - 1;
        }

        mouse_cycle = 0;
    }
}

static void handle_click(void) {
    bool left_btn = mouse_bytes[0] & 0x1;

    if (left_btn && !prev_left_btn) {
        ui_handle_click((uint32_t)mouse_x, (uint32_t)mouse_y);
    }

    prev_left_btn = left_btn;
}