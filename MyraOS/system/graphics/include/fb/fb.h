#ifndef FB_H
#define FB_H

#include <stdint.h>

typedef uint32_t argb_t;

typedef struct fb_info_t {
    argb_t* addr;
    uint32_t pitch;
    uint32_t width, height;
    uint8_t bpp;
    uint32_t pixels_per_row;
} fb_info_t;

extern fb_info_t fb_info;

void fb_init(fb_info_t info);

void fb_draw_pixel(uint32_t x, uint32_t y, argb_t color);
argb_t fb_get_pixel(uint32_t x, uint32_t y);

void fb_flush(argb_t* buffer);

#endif // FB_H