#ifndef GFX_H
#define GFX_H

#include "fb/fb.h"

#include <stddef.h>

typedef enum {
    LAYER_BACKGROUND = 0,
    LAYER_APP,
    LAYER_UI,
    LAYER_OVERLAY,
    LAYER_CURSOR,
    LAYER_COUNT
} layer_id_t;

void gfx_init(void);

void gfx_draw_pixel(layer_id_t layer, uint32_t x, uint32_t y, argb_t color);
argb_t gfx_get_pixel(uint32_t x, uint32_t y);
void gfx_draw_line(layer_id_t layer, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, argb_t color);
void gfx_draw_rect(layer_id_t layer, uint32_t x, uint32_t y, uint32_t width, uint32_t height, argb_t color);
void gfx_fill_rect(layer_id_t layer, uint32_t x, uint32_t y, uint32_t width, uint32_t height, argb_t color);
void gfx_draw_round_rect(layer_id_t layer, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t r, argb_t color);
void gfx_fill_round_rect(layer_id_t layer, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t r, argb_t color);
void gfx_draw_circle(layer_id_t layer, uint32_t cx, uint32_t cy, uint32_t radius, argb_t color);
void gfx_fill_circle(layer_id_t layer, uint32_t cx, uint32_t cy, uint32_t radius, argb_t color);
void gfx_draw_polygon(layer_id_t layer, const uint32_t* xs, const uint32_t* ys, size_t count, argb_t color);

void gfx_blit(layer_id_t layer, uint32_t dst_x, uint32_t dst_y, uint32_t dst_w, uint32_t dst_h, const uint32_t* src_pixels, uint32_t src_w, uint32_t src_h, uint32_t src_pitch_bytes);

void gfx_clear(layer_id_t layer, argb_t color);

void gfx_flush_dirty(void);
void gfx_flush(void);

void gfx_mark_dirty(layer_id_t layer, int x, int y);
void gfx_mark_dirty_rect(layer_id_t layer, int x, int y, int w, int h);

#endif // GFX_H