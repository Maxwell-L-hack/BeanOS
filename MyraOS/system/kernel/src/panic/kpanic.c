#include "panic/panic.h"

#include <stdbool.h>

#include "interrupt/isr/isr.h"
#include "print/print.h"
#include "gfx/gfx.h"
#include "font/font.h"
#include "mouse/mouse.h"

void kpanic(const char* message) {
    mouse_set(false);
    for (uint32_t layer = 0; layer < LAYER_COUNT; layer++) {
        gfx_clear(layer, 0x00000000);
    }
    gfx_clear(LAYER_BACKGROUND, 0xFFFF6961);

    font_restore_default();
    font_writef("Kernel panic! Please provide this error and reset: %s", LAYER_UI, message);

    gfx_flush();

    kclear_screen();
    kset_color(COLOR_RED);

    kprintln("KERNEL PANIC!");
    kprintln(message);

    kset_color(COLOR_WHITE);

    while (true) {
        __asm__ volatile("hlt");
    }
}