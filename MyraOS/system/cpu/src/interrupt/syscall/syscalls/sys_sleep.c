#include <stdint.h>

#include "pit/pit.h"

uint32_t sys_sleep(
    uint32_t ms,
    uint32_t unused1 __attribute__((unused)),
    uint32_t unused2 __attribute__((unused)),
    uint32_t unused3 __attribute__((unused))
) {
    uint32_t ticks = (ms + 9) / 10;
    uint32_t start = pit_ticks();
    while (pit_ticks() - start < ticks);
    
    return 0;
}