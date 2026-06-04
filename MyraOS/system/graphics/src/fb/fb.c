#include "fb/fb.h"

fb_info_t fb_info;

void fb_init(fb_info_t info) {
    fb_info = info;
}

void fb_draw_pixel(uint32_t x, uint32_t y, argb_t color) {
    if (x >= fb_info.width || y >= fb_info.height) {
        return;
    }

    uint32_t offset = y * (fb_info.pitch / sizeof(argb_t)) + x;
    fb_info.addr[offset] = color;
}

argb_t fb_get_pixel(uint32_t x, uint32_t y) {
    if (x >= fb_info.width || y >= fb_info.height) {
        return 0;
    }

    uint32_t offset = y * (fb_info.pitch / sizeof(argb_t)) + x;
    return fb_info.addr[offset];
}

void fb_flush(argb_t* buffer) {
    for (uint32_t y = 0; y < fb_info.height; y++) {
        uint32_t* dst_row = &fb_info.addr[y * fb_info.pixels_per_row];
        uint32_t* src_row = &buffer[y * fb_info.pixels_per_row];
        for (uint32_t x = 0; x < fb_info.width; x++) {
            dst_row[x] = src_row[x];
        }
    }
}
