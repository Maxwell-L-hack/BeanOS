#include "ui/ui.h"

#include <stddef.h>

#include "heap/heap.h"
#include "gfx/gfx.h"

static widget_t* ui_widgets[MAX_WIDGETS];
static size_t ui_widget_count = 0;

void ui_clear(void) {
    for (size_t i = 0; i < ui_widget_count; i++) {
        widget_t* w = ui_widgets[i];

        if (w->data) {
            kfree(w->data);
        }

        kfree(w);
        ui_widgets[i] = NULL;
    }

    ui_widget_count = 0;
}

void ui_add_widget(widget_t* w) {
    if (ui_widget_count == MAX_WIDGETS) {
        return;
    }

    w->dirty = true;
    ui_widgets[ui_widget_count++] = w;
}

void ui_frame(void) {
    ui_destroy();
    ui_render();
}

void ui_render(void) {
    for (size_t i = 0; i < ui_widget_count; i++) {
        widget_t* w = ui_widgets[i];
        if (w->visible && w->draw && w->dirty) {
            w->draw(w);
            w->dirty = false;
        }
    }
}

void ui_destroy(void) {
    for (size_t i = 0; i < ui_widget_count;) {
        widget_t* w = ui_widgets[i];
        if (w->destroy) {
            gfx_fill_rect(LAYER_UI, w->x, w->y, w->width, w->height, 0x00000000);
            gfx_mark_dirty_rect(LAYER_UI, w->x, w->y, w->width, w->height);
            gfx_fill_rect(LAYER_APP, w->x, w->y, w->width, w->height, 0x00000000);
            gfx_mark_dirty_rect(LAYER_APP, w->x, w->y, w->width, w->height);

            for (size_t j = 0; j < ui_widget_count; j++) {
                widget_t* cw = ui_widgets[j];
                if (cw == w) {
                    continue;
                }

                bool overlaps = !(cw->x + cw->width <= w->x || cw->x >= w->x + w->width || cw->y + cw->height <= w->y || cw->y >= w->y + w->height);
                if (overlaps) {
                    ui_set_dirty(cw);
                }
            }

            kfree(w->data);
            kfree(w);

            for (size_t j = i; j < ui_widget_count - 1; j++) {
                ui_widgets[j] = ui_widgets[j + 1];
            }
            ui_widget_count--;
            ui_widgets[ui_widget_count] = NULL;
        } else {
            i++;
        }
    }
}

void ui_dispatch_event(const ui_event_t* e) {
    for (int i = ui_widget_count - 1; i >= 0; i--) {
        widget_t* w = ui_widgets[i];
        if (w->visible && w->on_event) {
            w->on_event(w, e);
        }
    }
}

void ui_handle_click(uint32_t x, uint32_t y) {
    for (int i = ui_widget_count - 1; i >= 0; i--) {
        widget_t* w = ui_widgets[i];
        if (!w->visible || !w->on_event) {
            continue;
        }

        if (x >= w->x && x < w->x + w->width && y >= w->y && y < w->y + w->height) {
            uint32_t rel_x = x - w->x;
            uint32_t rel_y = y - w->y;

            ui_event_t e = {
                .type = UI_EVENT_CLICK,
                .click = {
                    .rel_x = rel_x,
                    .rel_y = rel_y
                }
            };
            w->on_event(w, &e);
            
            return; 
        }
    }
}

void ui_destroy_widget(widget_t* w) {
    w->destroy = true;
}

void ui_set_dirty(widget_t* w) {
    w->dirty = true;
}