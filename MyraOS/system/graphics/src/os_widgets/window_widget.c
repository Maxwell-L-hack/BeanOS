#include "os_widgets/window_widget.h"

#include "heap/heap.h"
#include "gfx/gfx.h"
#include "font/fonts/myra_font.h"
#include "libc_kernel/string.h"
#include "font/font.h"
#include "schedule/schedule.h"

typedef struct window_widget_data_t {
    char* title;
    win_handle_t handle;
    uint32_t app_w;
    uint32_t app_h;
    void* pixels;
    uint32_t pitch;

    uint32_t title_bar_h;
    uint32_t close_w;
    uint32_t close_h;
    uint32_t close_rx;
    uint32_t close_x;
    uint32_t close_y;
} window_widget_data_t;

static void widget_os_window_draw(widget_t* w) {
    window_widget_data_t* d = (window_widget_data_t*) w->data;

    gfx_fill_rect(LAYER_UI, w->x, w->y, w->width, d->title_bar_h, 0xCCF2F2F2);

    int title_x = w->x + (w->width - (int)kstrlen(d->title) * (int)myra_font.width) / 2;
    int title_y = w->y + ((int)d->title_bar_h - (int)myra_font.height) / 2;

    font_set_cursor((cursor_t){title_x, title_y});
    font_set_color(0xFF111111);
    font_write(d->title, LAYER_UI);

    uint32_t bx = w->x + d->close_x;
    uint32_t by = w->y + d->close_y;
    uint32_t side = d->close_h - 4;
    uint32_t ix = bx + (d->close_w - side) / 2;
    uint32_t iy = by + (d->close_h - side) / 2;
    uint32_t ix2 = ix + side - 1;
    uint32_t iy2 = iy + side - 1;

    gfx_draw_line(LAYER_UI, ix, iy, ix2, iy2, 0xFFE2554D);
    gfx_draw_line(LAYER_UI, ix, iy2, ix2, iy, 0xFFE2554D);
}

static void widget_os_window_event(widget_t* w, const ui_event_t* e) {
    window_widget_data_t* d = (window_widget_data_t*) w->data;

    if (e->type == UI_EVENT_CLICK) {
        uint32_t rx = e->click.rel_x;
        uint32_t ry = e->click.rel_y;

        if (rx >= d->close_x && rx < d->close_x + d->close_w &&
            ry >= d->close_y && ry < d->close_y + d->close_h) {
            window_destroy(d->handle);
            ui_destroy_widget(w);

            schedule_close_current_proc();
            
            return;
        }
    }
}

widget_t* widget_os_window_create(char* title, win_handle_t handle, uint32_t x, uint32_t y, uint32_t app_w, uint32_t app_h) {
    window_widget_data_t* d = kmalloc(sizeof(window_widget_data_t));
    d->title = kstrdup(title);
    d->handle = handle;
    d->app_w = app_w;
    d->app_h = app_h;

    d->title_bar_h = 28;
    d->close_w = 20;
    d->close_h = 16;
    d->close_rx = 6;
    d->close_x = d->close_w - 10;
    d->close_y = (d->title_bar_h - d->close_h) / 2;

    window_t* win = window_from_handle(d->handle);
    if (!win) {
        kfree(d);
        return NULL;
    }

    d->pixels = win->surface_kptr;
    d->pitch = win->pitch;

    widget_t* w = kmalloc(sizeof(widget_t));
    w->x = x;
    w->y = y;
    w->width = app_w + 2;
    w->height = app_h + d->title_bar_h + 2;
    w->data = d;
    w->visible = true;
    w->dirty = true;
    w->destroy = false;
    w->draw = widget_os_window_draw;
    w->on_event = widget_os_window_event;

    return w;
}

