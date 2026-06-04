#include "ui/widgets/label_widget.h"

#include "heap/heap.h"
#include "libc_kernel/string.h"

typedef struct label_data_t {
    const char* text;
    font_t* font;
    argb_t color;
} label_data_t;

static void widget_label_draw(widget_t* w) {
    font_state_t font_state = font_save_state();

    label_data_t* label_data = (label_data_t*) w->data;

    font_set_color(label_data->color);
    font_set_cursor((cursor_t) {w->x, w->y});
    font_set_font(label_data->font);
    
    font_write(label_data->text, LAYER_UI);

    font_restore_state(font_state);
}

widget_t* widget_label_create(uint32_t x, uint32_t y, const char* text, font_t* font, argb_t color) {
    label_data_t* label_data = (label_data_t*) kmalloc(sizeof(label_data_t));
    label_data->text = text;
    label_data->font = font;
    label_data->color = color;

    widget_t* w = kmalloc(sizeof(widget_t));
    w->x = x;
    w->y = y;
    w->width = kstrlen(text) * font->width;
    w->height = font->height;
    w->data = (void*) label_data;
    w->visible = true;
    w->dirty = true;
    w->destroy = false;
    w->draw = widget_label_draw;
    w->on_event = NULL;
    
    return w;
}