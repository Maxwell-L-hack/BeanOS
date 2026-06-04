#include "os_widgets/icon_widget.h"

#include "font/fonts/myra_font.h"
#include "gfx/gfx.h"
#include "heap/heap.h"
#include "image/image_loader.h"
#include "libc_kernel/string.h"
#include "launcher/launcher.h"
#include "schedule/schedule.h"

#define ICON_W 64
#define ICON_H 64

typedef struct icon_widget_data_t {
    char* title;
    char* icon_path;
    char* exec_path;
    argb_t* pixels;
    int sw;
    int sh;
} icon_widget_data_t;

static int inside(widget_t* w, uint32_t x, uint32_t y) {
    return x >= w->x && x < (w->x + w->width) && y >= w->y && y < (w->y + w->height);
}

static void icon_draw(widget_t* w) {
    icon_widget_data_t* d = (icon_widget_data_t*)w->data;

    int tw = (int)kstrlen(d->title) * myra_font.width;
    int icon_x = (int)w->x + (int)((int)w->width - ICON_W) / 2;
    int icon_y = (int)w->y;
    int text_x = (int)w->x + (int)((int)w->width - tw) / 2;
    int text_y = icon_y + ICON_H + 6;

    if (d->pixels != NULL && d->sw > 0 && d->sh > 0) {
        gfx_blit(
            LAYER_BACKGROUND,
            (uint32_t)icon_x, (uint32_t)icon_y,
            ICON_W, ICON_H,
            d->pixels, (uint32_t)d->sw, (uint32_t)d->sh, (uint32_t)(d->sw * 4)
        );
    }

    font_state_t st = font_save_state();
    font_set_cursor((cursor_t){ text_x, text_y });
    font_set_color(0xFFFFFFFF);
    font_write(d->title, LAYER_BACKGROUND);
    font_restore_state(st);
}

static void icon_event(widget_t* w, const ui_event_t* ev) {
    if (ev->type == UI_EVENT_CLICK) {
        uint32_t gx = w->x + ev->click.rel_x;
        uint32_t gy = w->y + ev->click.rel_y;
        if (inside(w, gx, gy) && !schedule_current_proc) {
            icon_widget_data_t* d = (icon_widget_data_t*)w->data;
            launcher_request_launch(d->exec_path);
        }
    }
}

widget_t* widget_os_icon_create(uint32_t x, uint32_t y, icon_desc_t* desc) {
    icon_widget_data_t* d = kmalloc(sizeof(icon_widget_data_t));
    d->title = kstrdup(desc->title);
    d->icon_path = kstrdup(desc->icon_path);
    d->exec_path = kstrdup(desc->exec_path);
    d->pixels = NULL;
    d->sw = 0;
    d->sh = 0;

    image_t* img = image_parse(d->icon_path);
    if (img != NULL) {
        d->pixels = (uint32_t*)img->pixels;
        d->sw = (int)img->width;
        d->sh = (int)img->height;
    } else {
        int size = ICON_W * ICON_H;

        d->pixels = kmalloc((size_t)size * 4);
        if (d->pixels != NULL) {
            for (int i = 0; i < size; i++) {
                d->pixels[i] = 0xFF888888u;
            }
        }

        d->sw = ICON_W;
        d->sh = ICON_H;
    }

    uint32_t w = (uint32_t)(ICON_W + 20);
    uint32_t h = (uint32_t)(ICON_H + 6 + myra_font.height);

    widget_t* wi = kmalloc(sizeof(widget_t));
    wi->x = x;
    wi->y = y + 40;
    wi->width = w;
    wi->height = h;
    wi->data = d;
    wi->visible = true;
    wi->dirty = true;
    wi->destroy = false;
    wi->draw = icon_draw;
    wi->on_event = icon_event;

    return wi;
}