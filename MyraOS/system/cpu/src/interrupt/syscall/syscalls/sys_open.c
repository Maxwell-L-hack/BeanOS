#include "ext2/file_desc.h"
#include "ext2/ext2.h"

uint32_t sys_open(uint32_t path_ptr, uint32_t flags, uint32_t _, uint32_t __) {
    (void) flags; (void) _; (void) __;

    if (!path_ptr) {
        return (uint32_t) -1;
    }

    const char* path = (const char*) path_ptr;

    for (int i = FIRST_FILE_FD; i < MAX_OPEN_FILES; i++) {
        if (!open_files[i].used) {
            size_t size;
            bool ok = false;
            uint8_t* data = ext2_read_file(root_fs, path, &size, &ok);

            if (!ok || !data) {
                return -1;
            }

            open_files[i] = (file_descriptor_t){
                .used = true,
                .data = data,
                .size = size,
                .offset = 0
            };

            return i;
        }
    }

    return -1;
}
