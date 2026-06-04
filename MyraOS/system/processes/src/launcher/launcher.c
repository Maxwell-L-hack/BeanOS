#include "launcher/launcher.h"

#include <stddef.h>
#include <stdint.h>

#include "exec/exec.h"
#include "ext2/ext2.h"
#include "heap/heap.h"
#include "libc_kernel/string.h"
#include "schedule/schedule.h"
#include "process/process.h"

static bool should_launch = false;
static char* launch_pending = NULL;

void launcher_request_launch(const char* path) {
    if (launch_pending) {
        kfree((void*)launch_pending);
    }

    launch_pending = kstrdup(path);
    should_launch = true;
}

void launcher_launch_pending(void) {
    if (!should_launch) {
        return;
    }

    should_launch = false;
    launcher_launch(launch_pending);
}

bool launcher_launch(const char* path) {
    size_t elf_size = 0;
    bool succeeded = true;
    void* shell_elf_data = (void*) ext2_read_file(root_fs, path, &elf_size, &succeeded);
    if (!succeeded) {
        return false;
    }

    elf_load_info_t load_info;
    if (!elf_parse(shell_elf_data, &load_info)) {
        return false;
    }

    exec_info_t exec_info;
    exec_parse_info_elf(&load_info, &exec_info);

    process_t* proc = proc_create(&exec_info);
    schedule_proc(proc);
    schedule_next();

    return true;
}