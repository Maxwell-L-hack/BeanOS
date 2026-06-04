#include <stdint.h>

#include "pit/pit.h"

uint32_t sys_time(
    uint32_t unused0 __attribute__((unused)),
    uint32_t unused1 __attribute__((unused)),
    uint32_t unused2 __attribute__((unused)),
    uint32_t unused3 __attribute__((unused))
) {
    return pit_ticks() * 10;
}