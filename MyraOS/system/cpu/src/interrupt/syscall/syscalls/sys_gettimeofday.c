#include "rtc/rtc.h"
#include "libc_kernel/string.h"

#define KERNEL_BASE 0xC0000000

int sys_gettimeofday(uint32_t tv_ptr, uint32_t _, uint32_t __, uint32_t ___) {
    (void) _;
    (void) __;
    (void) ___;

    if (!tv_ptr) {
        return -1;
    }

    struct timeval {
        uint32_t tv_sec;
        uint32_t tv_usec;
    };

    struct timeval* tv = (struct timeval*) tv_ptr;
    tv->tv_sec = rtc_get_system_epoch();
    tv->tv_usec = 0;
    
    return 0;
}
