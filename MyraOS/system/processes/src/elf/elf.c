#include "elf/elf.h"

#include "heap/heap.h"

#define READ_EXEC 0x5

static bool check_file(elf32_header_t* header) {
    if (!header) {
        return false;
    }

    if (
        header->e_ident[EI_MAG0] != EI_MAG0_VALUE 
        || header->e_ident[EI_MAG1] != EI_MAG1_VALUE
        || header->e_ident[EI_MAG2] != EI_MAG2_VALUE 
        || header->e_ident[EI_MAG3] != EI_MAG3_VALUE
    ) {
        klog_warn("ELF magic is incorrect");
        return false;
    }

    return true;
}

static bool check_file_support(elf32_header_t* header) {
    if (!check_file(header)) {
        klog_warn("Invalid ELF file");
        return false;
    }

    bool is_valid = true;
    if (header->e_ident[EI_CLASS] != ELFCLASS32) {
        klog_warn("Unsupported ELF File Class");
        is_valid = false;
    }

    if (header->e_ident[EI_DATA] != ELFDATA2LSB) {
        klog_warn("Unsupported ELF file byte order");
        is_valid = false;
    }

    if (header->e_machine != EM_386) {
        klog_warn("Unsupported ELF file target");
        is_valid = false;
    }

    if (header->e_ident[EI_VERSION] != EV_CURRENT) {
        klog_warn("Unsupported ELF File version");
        is_valid = false;
    }


    if (header->e_type != ET_REL && header->e_type != ET_EXEC) {
        klog_warn("Unsupported ELF File type");
        is_valid = false;
    }

    return is_valid;
}

bool elf_parse(void* elf_data, elf_load_info_t* out_info) {
    elf32_header_t* header = (elf32_header_t*) elf_data;
    if (!check_file_support(header)) {
        return false;
    }

    size_t seg_count = 0;
    elf_segment_t* segments_arr = kmalloc(sizeof(elf_segment_t) * header->e_phnum);
    elf32_phdr_t* phdrs = (elf32_phdr_t*) ((uint8_t*) elf_data + header->e_phoff);

    for (size_t i = 0; i < header->e_phnum; i++) {
        elf32_phdr_t* phdr = &phdrs[i];
        elf_segment_t segment;
        
        if (phdr->p_type != PT_LOAD) {
            continue;
        }

        segment.file_size = phdr->p_filesz;
        segment.flags = phdr->p_flags;
        segment.mem_size = phdr->p_memsz;
        segment.virt_addr = (void*) phdr->p_vaddr;
        segment.src = (void*)((uint8_t*) elf_data + phdr->p_offset);

        segments_arr[seg_count++] = segment;
    }

    segments_arr = krealloc(segments_arr, sizeof(elf_segment_t) * seg_count);
    
    out_info->segment_count = seg_count;
    out_info->segments = segments_arr;
    out_info->entry_point = (void*) header->e_entry;

    return true;
}