#include <stdint.h>
#include <stdbool.h>

#include "schedule/schedule.h"

uint32_t sys_exit(
    uint32_t exit_code,
    uint32_t unused0 __attribute__((unused)),
    uint32_t unused1 __attribute__((unused)),
    uint32_t unused2 __attribute__((unused))
) {
    schedule_current_proc->state = PROCESS_TERMINATED;

    // Wait for IRQ
    while (true) {
        __asm__ volatile("hlt");
    }

    return exit_code;
}