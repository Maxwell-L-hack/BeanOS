#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include <stdint.h>
#include <stddef.h>

#include "fb/fb.h"

typedef struct image_t {
    uint32_t width, height;
    argb_t pixels[]; // (width * height) elements
} image_t;

typedef enum {
    IMAGE_FORMAT_UNKNOWN,
    IMAGE_FORMAT_BMP,
} image_format_t;

image_t* image_parse(const char* path);

#endif // IMAGE_LOADER_H