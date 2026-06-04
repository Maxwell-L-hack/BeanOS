#include "ext2/file_desc.h"
#include "heap/heap.h"

uint32_t sys_close(uint32_t fd, uint32_t _, uint32_t __, uint32_t ___) {
    (void) _;
    (void) __;
    (void) ___;

    if (fd < FIRST_FILE_FD || fd >= MAX_OPEN_FILES || !open_files[fd].used)
        return -1;

    kfree(open_files[fd].data);
    open_files[fd] = (file_descriptor_t) {0};
    return 0;
}
