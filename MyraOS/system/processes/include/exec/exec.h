#ifndef EXEC_H
#define EXEC_H

#include <stddef.h>

#include "elf/elf.h"

typedef enum exec_format_t {
    EXEC_FORMAT_ELF,
    EXEC_FORMAT_BINARY
} exec_format_t;

typedef enum {
    SEGMENT_EXECUTE = 1 << 0,
    SEGMENT_WRITE   = 1 << 1,
    SEGMENT_READ    = 1 << 2,
    
    // Common combinations
    SEGMENT_RX = SEGMENT_READ | SEGMENT_EXECUTE,
    SEGMENT_RW = SEGMENT_READ | SEGMENT_WRITE,
    SEGMENT_RWX = SEGMENT_READ | SEGMENT_WRITE | SEGMENT_EXECUTE
} segment_flags_t;

typedef struct exec_segment_t {
    uint32_t virt_addr;    
    size_t file_size;     
    size_t mem_size;       
    void* data;           
    uint32_t flags;      
} exec_segment_t;

typedef struct exec_info_t {
    exec_format_t format;
    void* entry_point;
    exec_segment_t* segments;
    size_t segment_count;
} exec_info_t;

void exec_parse_info_elf(elf_load_info_t* elf_load_info, exec_info_t* out_info);

void exec_parse_info_bin(void* file_data, size_t file_size, uint32_t load_address, exec_info_t* out_info);

#endif // EXEC_H