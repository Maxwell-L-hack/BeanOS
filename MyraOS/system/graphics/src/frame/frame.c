#include "frame/frame.h"

#include <stdbool.h>

#include "gfx/gfx.h"
#include "libc_kernel/string.h"
#include "mouse/mouse.h"
#include "pit/pit.h"
#include "ui/ui.h"

static screen_t* current_screen;

static void draw_mouse(void);

void frame_render(void) {
    if (current_screen) {
        ui_frame();

        ui_event_t tick_event = {
            .type = UI_EVENT_TICK,
            .tick = {
                .system_ticks = pit_ticks()
            }
        };
        ui_dispatch_event(&tick_event);
    }

    draw_mouse();

    gfx_flush_dirty();
}

void frame_set_screen(screen_t* new_screen) {
    ui_clear();

    current_screen = new_screen;
    if (current_screen->init) {
        current_screen->init();
    }
}

static void draw_mouse(void) { 
    if (!cursor_enabled) {
        return;
    }

    uint32_t mouse_fill_x = prev_mouse_x - CURSOR_SIZE > 0 ? prev_mouse_x - CURSOR_SIZE : 0;
    uint32_t mouse_fill_y = prev_mouse_y - CURSOR_SIZE > 0 ? prev_mouse_y - CURSOR_SIZE : 0;
    gfx_fill_rect(LAYER_CURSOR, mouse_fill_x, mouse_fill_y, CURSOR_SIZE * 2 + 1, CURSOR_SIZE * 2 + 1, 0x00000000);

    gfx_fill_circle(LAYER_CURSOR, mouse_x, mouse_y, CURSOR_SIZE, 0xFFFFFFFF);

    gfx_mark_dirty(LAYER_CURSOR, mouse_fill_x, mouse_fill_y);

    prev_mouse_x = mouse_x;
    prev_mouse_y = mouse_y;
}