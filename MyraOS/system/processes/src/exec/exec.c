#include "exec/exec.h"

#include "heap/heap.h"

void exec_parse_info_elf(elf_load_info_t* elf_load_info, exec_info_t* out_info) {
    out_info->format = EXEC_FORMAT_ELF;
    out_info->entry_point = elf_load_info->entry_point;
    out_info->segment_count = elf_load_info->segment_count;

    // Allocate and convert segments
    out_info->segments = (exec_segment_t*) kmalloc(sizeof(exec_segment_t) * out_info->segment_count);
    for (size_t i = 0; i < out_info->segment_count; i++) {
        elf_segment_t* src = &elf_load_info->segments[i];
        exec_segment_t* dst = &out_info->segments[i];

        dst->virt_addr = (uint32_t) src->virt_addr;
        dst->file_size = src->file_size;
        dst->mem_size = src->mem_size;
        dst->data = src->src;
        dst->flags = src->flags;
    }
}

void exec_parse_info_bin(void* file_data, size_t file_size, uint32_t load_address, exec_info_t* out_info) {
    out_info->format = EXEC_FORMAT_BINARY;
    out_info->entry_point = (void*) load_address;
    out_info->segment_count = 1;
    
    // Allocate single segment
    out_info->segments = (exec_segment_t*) kmalloc(sizeof(exec_segment_t));
    exec_segment_t* seg = &out_info->segments[0];
    
    seg->virt_addr = load_address;
    seg->file_size = file_size;
    seg->mem_size = file_size;
    seg->data = file_data;
    seg->flags = SEGMENT_RX;
}