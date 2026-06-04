#include <stdint.h>

#include "ext2/file_desc.h"
#include "keyboard/keyboard.h"
#include "tty/tty.h"
#include "libc_kernel/string.h"

#define STDIN 0

uint32_t sys_read(uint32_t fd, uint32_t buf_ptr, uint32_t len, uint32_t _) {
    (void)_;

    if (!buf_ptr) {
        return (uint32_t)-1;
    }

    if (len == 0) {
        return 0;
    }

    if (fd == STDIN) {
        bool blocking = (tty_get_flags() & TTYF_ICANON) != 0;
        return (uint32_t)tty_read((uint8_t*)buf_ptr, (size_t)len, blocking);
    }

    if (fd < FIRST_FILE_FD || fd >= MAX_OPEN_FILES) {
        return (uint32_t)-1;
    }

    file_descriptor_t* f = &open_files[fd];
    if (!f->used) {
        return -1;
    }

    if (f->offset >= f->size) {
        return 0;
    }

    size_t avail = f->size - f->offset;
    size_t to_copy = (len < avail) ? len : avail;

    kmemcpy((void*)buf_ptr, f->data + f->offset, to_copy);
    f->offset += to_copy;

    return (uint32_t)to_copy;
}
