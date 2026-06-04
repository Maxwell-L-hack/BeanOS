#ifndef FILE_DESC_H
#define FILE_DESC_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define MAX_OPEN_FILES 32
#define FIRST_FILE_FD 3

typedef struct {
    bool used;
    uint8_t* data;
    size_t size;
    size_t offset;
} file_descriptor_t;

extern file_descriptor_t open_files[MAX_OPEN_FILES];

#endif // FILE_DESC_H
