#ifndef UI_H
#define UI_H

#include <stdint.h>
#include <stdbool.h>

#include "ui_event.h"

#define MAX_WIDGETS 64

typedef struct widget_t {
    uint32_t x, y, width, height;

    bool dirty;
    bool visible;
    bool destroy;

    void (*draw)(struct widget_t*);
    void (*on_event)(struct widget_t*, const ui_event_t*);

    void* data;
} widget_t;

void ui_clear(void);
void ui_add_widget(widget_t* w);
void ui_frame(void);
void ui_render(void);
void ui_destroy(void);
void ui_dispatch_event(const ui_event_t* e);
void ui_handle_click(uint32_t x, uint32_t y);
void ui_destroy_widget(widget_t* w);
void ui_set_dirty(widget_t* w);

#endif // UI_H