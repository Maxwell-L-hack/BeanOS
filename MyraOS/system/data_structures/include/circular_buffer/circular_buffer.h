#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stdbool.h>
#include <stddef.h>

#define CIRCULAR_BUFFER_SIZE 256

typedef struct circular_buffer_t {
    void *data;
    size_t element_size;
    size_t capacity;
    size_t head, tail, count;
} circular_buffer_t;

// init
extern void cb_init(circular_buffer_t* cb, size_t element_size, size_t capacity);

// write
extern bool cb_write(circular_buffer_t* cb, const void* element);

// read
extern bool cb_read(circular_buffer_t* cb, void* out);

extern void* cb_peek(circular_buffer_t* cb);

#endif  // CIRCULAR_BUFFER_H