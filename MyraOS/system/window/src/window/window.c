#include "window/window.h"

#include "heap/heap.h"
#include "gfx/gfx.h"
#include "libc_kernel/string.h"
#include "vmm/vmm.h"
#include "pmm/pmm.h"

#define MAX_WINDOWS 32

#define WIN_SURF_KBASE 0xE8000000u
static uint32_t g_win_kva_next = WIN_SURF_KBASE;

static window_t g_windows[MAX_WINDOWS];
static win_handle_t g_next_handle = 1;

static int alloc_slot(void);

void window_init(void) {
    for (size_t i = 0; i < MAX_WINDOWS; i++) {
        g_windows[i].used = false;
    }
}

void window_present(win_handle_t h) {
    window_t* win = window_from_handle(h);
    if (win == NULL) {
        return;
    }

    const uint32_t* src = (const uint32_t*) win->surface_kptr;
    gfx_blit(
        LAYER_APP,
        0, 40 + 28,
        fb_info.width, fb_info.height - 68,
        src,
        win->logical_w, win->logical_h, win->pitch
    );
}

win_handle_t window_create(uint32_t owner_pid, uint32_t w, uint32_t h, uint32_t flags) {
    int idx = alloc_slot();
    if (idx < 0) {
        return 0;
    }

    size_t bpp = 4;
    size_t pitch = (size_t)w * bpp;
    size_t size_bytes = (size_t)h * pitch;

    uint32_t npages = (uint32_t)((size_bytes + PAGE_SIZE - 1) / PAGE_SIZE);

    uint32_t* frames = (uint32_t*) kmalloc(sizeof(uint32_t) * npages);
    if (frames == NULL) {
        return 0;
    }

    for (uint32_t i = 0; i < npages; i++) {
        frames[i] = (uint32_t) pmm_alloc_page();
        if (frames[i] == 0) {
            for (uint32_t j = 0; j < i; j++) {
                pmm_free_page((void*) frames[j]);
            }
            kfree(frames);
            
            return 0;
        }
    }

    uint32_t kbase = g_win_kva_next;
    for (uint32_t i = 0; i < npages; i++) {
        vmm_map_page(kbase + i * PAGE_SIZE, frames[i], PAGE_PRESENT | PAGE_WRITE);
    }
    g_win_kva_next += npages * PAGE_SIZE;

    kmemset((void*) kbase, 0, npages * PAGE_SIZE);

    window_t* win = &g_windows[idx];

    win->used = true;
    win->handle = g_next_handle++;
    win->owner_pid = owner_pid;

    win->logical_w = w;
    win->logical_h = h;

    win->surface_kptr = (void*) kbase;
    win->surface_uptr = NULL;
    win->pitch = (uint32_t) pitch;

    win->flags = flags;

    win->npages = npages;
    win->frames = frames;

    win->has_widget = false;
    win->title = NULL;

    return win->handle;
}

window_t* window_from_handle(win_handle_t h) {
    for (int i = 0; i < MAX_WINDOWS; i++) {
        if (g_windows[i].used && g_windows[i].handle == h) {
            return &g_windows[i];
        }
    }

    return NULL;
}

void window_destroy(win_handle_t h) {
    window_t* win = window_from_handle(h);
    if (win == NULL) {
        return;
    }

    if (win->frames != NULL) {
        uint32_t kbase = (uint32_t) win->surface_kptr;

        for (uint32_t i = 0; i < win->npages; i++) {
            vmm_unmap_page(kbase + i * PAGE_SIZE);
        }

        for (uint32_t i = 0; i < win->npages; i++) {
            pmm_free_page((void*) win->frames[i]);
        }

        kfree(win->frames);
        win->frames = NULL;
    }

    win->surface_kptr = NULL;
    win->surface_uptr = NULL;
    win->used = false;
    win->title = NULL;
    win->has_widget = false;
}

static int alloc_slot(void) {
    for (int i = 0; i < MAX_WINDOWS; i++) {
        if (!g_windows[i].used) {
            return i;
        }
    }

    return -1;
}