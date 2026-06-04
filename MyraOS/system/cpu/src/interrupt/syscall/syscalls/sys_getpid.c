#include "schedule/schedule.h"

uint32_t sys_getpid() {
    return schedule_current_proc->pid;
}