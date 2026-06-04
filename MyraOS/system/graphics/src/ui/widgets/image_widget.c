#include "ui/widgets/image_widget.h"

#include "heap/heap.h"
#include "gfx/gfx.h"

typedef struct {
    const image_t* image;
    layer_id_t layer;
} image_widget_data_t;

static void image_draw(widget_t* w) {
    image_widget_data_t* data = (image_widget_data_t*) w->data;
    const image_t* img = data->image;

    for (uint32_t y = 0; y < w->height && y < img->height; y++) {
        for (uint32_t x = 0; x < w->width && x < img->width; x++) {
            argb_t color = img->pixels[y * img->width + x];
            gfx_draw_pixel(data->layer, w->x + x, w->y + y, color);
        }
    }
}

widget_t* widget_image_create(uint32_t x, uint32_t y, layer_id_t layer, const image_t* image) {
    image_widget_data_t* data = kmalloc(sizeof(image_widget_data_t));
    data->image = image;
    data->layer = layer;

    widget_t* w = kmalloc(sizeof(widget_t));
    w->x = x;
    w->y = y;
    w->width = image->width;
    w->height = image->height;
    w->data = data;
    w->visible = true;
    w->dirty = true;
    w->destroy = false;
    w->draw = image_draw;
    w->on_event = NULL;

    return w;
}
