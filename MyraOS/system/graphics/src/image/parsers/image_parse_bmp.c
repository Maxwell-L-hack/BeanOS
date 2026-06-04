#include "image/image_loader.h"

#include "heap/heap.h"

#define BMP_TYPE 0x4D42

typedef struct bmp_file_header_t {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
} __attribute__((packed)) bmp_file_header_t;

typedef struct bmp_info_header_t {
    uint32_t size;
    int32_t  width;
    int32_t  height;
    uint16_t planes;
    uint16_t bit_count;
    uint32_t compression;
    uint32_t image_size;
    int32_t  x_pixels_per_meter;
    int32_t  y_pixels_per_meter;
    uint32_t colors_used;
    uint32_t colors_important;
} __attribute__((packed)) bmp_info_header_t;

image_t* image_parse_bmp(const uint8_t* data, size_t size) {
    if (size < sizeof(bmp_file_header_t) + sizeof(bmp_info_header_t)) {
        return NULL;
    }

    const bmp_file_header_t* file_hdr = (const bmp_file_header_t*) data;
    const bmp_info_header_t* info_hdr = (const bmp_info_header_t*) (data + sizeof(bmp_file_header_t));

    if (file_hdr->type != BMP_TYPE) {
        return NULL;
    }

    if (info_hdr->bit_count != 24 || info_hdr->compression != 0) {
        return NULL;
    }

    int row_size = ((info_hdr->width * 3 + 3) / 4) * 4;
    const uint8_t* pixel_data = data + file_hdr->offset;

    if ((size_t)(row_size * info_hdr->height + file_hdr->offset) > size) {
        return NULL;
    }

    image_t* img = kmalloc(sizeof(image_t) + info_hdr->width * info_hdr->height * sizeof(argb_t));
    img->width = info_hdr->width;
    img->height = info_hdr->height;

    for (int y = 0; y < info_hdr->height; y++) {
        const uint8_t* row = pixel_data + row_size * (info_hdr->height - 1 - y);
        for (int x = 0; x < info_hdr->width; x++) {
            const uint8_t* px = row + x * 3;
            uint8_t b = px[0], g = px[1], r = px[2];
            img->pixels[y * info_hdr->width + x] = 0xFF000000 | (r << 16) | (g << 8) | b;
        }
    }

    return img;
}