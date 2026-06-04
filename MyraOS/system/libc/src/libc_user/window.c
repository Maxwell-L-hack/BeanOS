#include <stdint.h>
#include <libc_user/window.h>

#define SYS_WINDOW_CREATE          350
#define SYS_WINDOW_GET_SURFACE     351
#define SYS_WINDOW_DESTROY         352
#define SYS_WINDOW_PRESENT         353
#define SYS_WINDOW_SET_TITLE       354

static inline int syscall(int num, int arg1, int arg2, int arg3) {
    int ret;
    asm volatile (
        "int $0x80"
        : "=a"(ret)
        : "a"(num), "b"(arg1), "c"(arg2), "d"(arg3)
    );
    return ret;
}

window_t window_create(uint32_t w, uint32_t h, uint32_t flags) {
    return (window_t) syscall(SYS_WINDOW_CREATE, (int)w, (int)h, (int)flags);
}

int window_get_surface(window_t h, win_surface_desc_t* out_info) {
    return syscall(SYS_WINDOW_GET_SURFACE, (int)h, (int)out_info, 0);
}

int window_present(window_t h) {
    return syscall(SYS_WINDOW_PRESENT, (int)h, 0, 0);
}

int window_destroy(window_t h) {
    return syscall(SYS_WINDOW_DESTROY, (int)h, 0, 0);
}

int window_set_title(win_handle_t h, const char* title) {
    return syscall(SYS_WINDOW_SET_TITLE, h, (uint32_t)title, 0);
}
