#include <stdint.h>

#include "print/print.h"
#include "font/font.h"

#define STDOUT 1
#define STDERR 2

uint32_t sys_write(uint32_t fd, uint32_t buf_ptr, uint32_t len, uint32_t _) {
    (void)_;
    const char* str = (const char*) buf_ptr;

    if (fd == STDERR) {
        kset_color(COLOR_RED);
    }

    if (fd == STDOUT || fd == STDERR) { 
        for (uint32_t i = 0; i < len; i++) {
            kprintf("%c", str[i]);
        }
            
        return len;
    }

    if (fd == STDERR) {
        kset_color(COLOR_WHITE);
    }

    return -1;
}
