#include "image/image_loader.h"

#include "ext2/ext2.h"
#include "libc_kernel/string.h"

static image_format_t detect_format_from_path(const char* path);

extern image_t* image_parse_bmp(const uint8_t* data, size_t size);

image_t* image_parse(const char* path) {
    size_t image_size;
    bool succeeded;
    uint8_t* image_buffer = ext2_read_file(root_fs, path, &image_size, &succeeded);
    if (!succeeded) {
        return NULL;
    }

    image_format_t fmt = detect_format_from_path(path);
    switch (fmt) {
        case IMAGE_FORMAT_BMP: return image_parse_bmp(image_buffer, image_size);
        default: return NULL;
    }
}

static image_format_t detect_format_from_path(const char* path) {
    const char* ext = path;
    for (const char* p = path; *p; p++) {
        if (*p == '.') {
            ext = p;
        }
    }

    if (!ext || ext == path) {
        return IMAGE_FORMAT_UNKNOWN;
    }

    ext++; // skip '.'

    if (!kstrcmp(ext, "bmp")) {
        return IMAGE_FORMAT_BMP;
    }

    return IMAGE_FORMAT_UNKNOWN;
}
