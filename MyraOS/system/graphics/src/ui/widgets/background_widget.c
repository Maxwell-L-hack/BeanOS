#include "ui/widgets/background_widget.h"

#include "heap/heap.h"

typedef struct background_data_t {
    argb_t color;
} background_data_t;

static void widget_background_draw(widget_t* w) {
    background_data_t* data = (background_data_t*) w->data;
    gfx_clear(LAYER_BACKGROUND, data->color);
}

widget_t* widget_background_create(argb_t color) {
    background_data_t* data = (background_data_t*) kmalloc(sizeof(background_data_t));
    data->color = color;

    widget_t* w = kmalloc(sizeof(widget_t));
    w->x = 0;
    w->y = 0;
    w->width = fb_info.width;
    w->height = fb_info.height;
    w->data = (void*) data;
    w->visible = true;
    w->dirty = true;
    w->destroy = false;
    w->draw = widget_background_draw;
    w->on_event = NULL;

    return w;
}