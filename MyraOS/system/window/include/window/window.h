#ifndef WINDOW_H
#define WINDOW_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef uint32_t win_handle_t;

typedef struct win_surface_info_t { 
    uint32_t w, h; 
} win_surface_info_t;

typedef enum win_flags_t {
    WIN_FLAG_SCALE_NEAREST = 1u << 0
} win_flags_t;

typedef struct window_t {
    bool used;

    win_handle_t handle;
    uint32_t owner_pid;

    uint32_t logical_w;
    uint32_t logical_h;

    void* surface_kptr;
    void* surface_uptr;

    uint32_t pitch;

    uint32_t flags;

    uint32_t npages;
    uint32_t* frames;

    bool has_widget;
    char* title;
} window_t;

void window_init(void);
void window_present(win_handle_t h);
win_handle_t window_create(uint32_t owner_pid, uint32_t w, uint32_t h, uint32_t flags);
window_t* window_from_handle(win_handle_t h);
void window_destroy(win_handle_t h);

#endif // WINDOW_H