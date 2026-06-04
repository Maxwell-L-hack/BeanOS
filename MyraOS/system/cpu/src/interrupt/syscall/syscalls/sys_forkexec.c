#include <stdint.h>

#include "schedule/schedule.h"
#include "ext2/ext2.h"
#include "exec/exec.h"

#define FORKEXEC_FILE_FAIL -1
#define FORKEXEC_PARSE_FAIL -2
#define FORKEXEC_PROC_FAIL -3

uint32_t sys_forkexec(
    uint32_t path_addr,
    uint32_t unused0 __attribute__((unused)),
    uint32_t unused1 __attribute__((unused)),
    uint32_t unused2 __attribute__((unused))
) {
    const char* path = (const char*) path_addr;

    size_t file_size;
    bool success;
    void* file_data = ext2_read_file(root_fs, path, &file_size, &success);
    if (!success || !file_data) {
        return FORKEXEC_FILE_FAIL;
    }
    
    elf_load_info_t elf_info;
    if (!elf_parse(file_data, &elf_info)) {
        return FORKEXEC_PARSE_FAIL;
    }
        
    exec_info_t exec_info;
    exec_parse_info_elf(&elf_info, &exec_info);

    process_t* proc = proc_create(&exec_info);
    if (!proc) {
        return FORKEXEC_PROC_FAIL;
    }

    schedule_proc(proc);
    return proc->pid;
}