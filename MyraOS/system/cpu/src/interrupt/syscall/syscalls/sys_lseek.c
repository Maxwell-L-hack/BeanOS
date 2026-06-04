#include <stdint.h>
#include <stddef.h>

#include "ext2/file_desc.h"

uint32_t sys_lseek(uint32_t fd, uint32_t off, uint32_t whence, uint32_t _) {
    (void) _;

    if (fd < FIRST_FILE_FD || fd >= MAX_OPEN_FILES) {
        return (uint32_t)-1;
    }

    file_descriptor_t* f = &open_files[fd];
    if (!f->used) {
        return (uint32_t)-1;
    }

    int64_t base = 0;
    if (whence == 0) {
        base = 0;
    } else if (whence == 1) {
        base = f->offset;
    } else if (whence == 2) {
        base = f->size;
    } else {
        return (uint32_t)-1;
    }

    int64_t newpos = base + (int32_t)off;
    if (newpos < 0) {
        newpos = 0;
    }

    if ((uint64_t)newpos > f->size) {
        newpos = f->size;
    }

    f->offset = (size_t)newpos;
    return (uint32_t)f->offset;
}
