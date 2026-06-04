#include "os_widgets/top_bar_widget.h"

#include "font/fonts/myra_font.h"
#include "gfx/gfx.h"
#include "heap/heap.h"
#include "image/image_loader.h"
#include "libc_kernel/string.h"
#include "rtc/rtc.h"
#include "ui/widgets/label_widget.h"
#include "power/power.h"

typedef struct top_bar_data_t {
    argb_t color;
    argb_t* power_pixels;
    int power_w;
    int power_h;
} top_bar_data_t;

static void widget_os_top_bar_draw(widget_t* w) {
    font_state_t font_state = font_save_state();

    top_bar_data_t* data = (top_bar_data_t*) w->data;

    // Draw background
    gfx_fill_rect(LAYER_UI, w->x, w->y, w->width, w->height, data->color);

    // Draw time
    datetime_t dt = rtc_get_system_datetime();
    font_set_cursor((cursor_t){16, w->y + (w->height - myra_font.height) / 2});
    font_set_color(0xFFFFFFFF);
    font_writef("%02d/%02d %02d:%02d", LAYER_UI, dt.day, dt.month, dt.hour, dt.minute);

    // Draw "MyraOS" label
    const char* title = "MyraOS";
    int title_x = (w->width - kstrlen(title) * myra_font.width) / 2;
    int title_y = w->y + (w->height - myra_font.height) / 2;

    font_set_cursor((cursor_t){title_x, title_y});
    font_write(title, LAYER_UI);

    font_restore_state(font_state);

    // Draw "Power off" icon
    const int draw_w = 24;
    const int draw_h = 24;

    int icon_x = (int)w->width - draw_w - 16;
    int icon_y = (int)w->y + ((int)w->height - draw_h) / 2;

    if (data->power_pixels != NULL && data->power_w > 0 && data->power_h > 0) {
        gfx_blit(
            LAYER_UI,
            (uint32_t)icon_x, (uint32_t)icon_y,
            (uint32_t)draw_w, (uint32_t)draw_h,
            (uint32_t*)data->power_pixels,
            (uint32_t)data->power_w, (uint32_t)data->power_h,
            (uint32_t)(data->power_w * 4)
        );
    } else {
        gfx_fill_rect(LAYER_UI, (uint32_t)icon_x, (uint32_t)icon_y, draw_w, draw_h, 0xFFFFFFFFu);
    }
}

static void widget_os_top_bar_event(widget_t* w, const ui_event_t* event) {
    if (event->type == UI_EVENT_TICK) {
        if (event->tick.system_ticks % 6000 == 0) {
            ui_set_dirty(w);
        }
    } else if (event->type == UI_EVENT_CLICK) {
        const int draw_w = 16;
        const int draw_h = 16;

        int icon_x = (int)w->width - draw_w - 16;
        int icon_y = (int)w->y + ((int)w->height - draw_h) / 2;

        uint32_t gx = w->x + event->click.rel_x;
        uint32_t gy = w->y + event->click.rel_y;

        if (gx >= (uint32_t)icon_x && gx < (uint32_t)(icon_x + draw_w) &&
            gy >= (uint32_t)icon_y && gy < (uint32_t)(icon_y + draw_h)) {
            power_shutdown();
            return;
        }
    }
}

widget_t* widget_os_top_bar_create(uint32_t x, uint32_t y, int height, argb_t color) {
    top_bar_data_t* data = kmalloc(sizeof(top_bar_data_t));
    data->color = color;
    data->power_pixels = NULL;
    data->power_w = 0;
    data->power_h = 0;

    const char* icon_path = "/myra/components/turn_off_button.bmp";
    image_t* icon = image_parse(icon_path);
    if (icon != NULL) {
        data->power_pixels = (uint32_t*)icon->pixels;
        data->power_w = (int)icon->width;
        data->power_h = (int)icon->height;
    }

    widget_t* w = kmalloc(sizeof(widget_t));
    w->x = x;
    w->y = y;
    w->width = fb_info.width;
    w->height = height;
    w->data = (void*) data;
    w->visible = true;
    w->dirty = true;
    w->destroy = false;
    w->draw = widget_os_top_bar_draw;
    w->on_event = widget_os_top_bar_event;

    return w;
}