#ifndef ELF_H
#define ELF_H

#include <stdint.h>
#include <stdbool.h>

#include "log/log.h"

typedef uint16_t elf32_half;
typedef uint32_t elf32_off;
typedef uint32_t elf32_addr;
typedef uint32_t elf32_word;
typedef int32_t  elf32_sword;

#define ELF_NIDENT 16

// ELF magic values
#define EI_MAG0_VALUE 0X7f
#define EI_MAG1_VALUE 'E'
#define EI_MAG2_VALUE 'L'
#define EI_MAG3_VALUE 'F'

// ELF class
#define ELFCLASSNONE 0
#define ELFCLASS32 1
#define ELDCLASS64 2

// ELF data 
#define ELFDATANONE 0
#define ELFDATA2LSB 1 
#define ELFDATA2MSB 2

// ELF current data
# define EM_386		3
# define EV_CURRENT	1

typedef enum elf_ident_t {
    EI_MAG0		    = 0, // 0x7F
	EI_MAG1		    = 1, // 'E'
	EI_MAG2		    = 2, // 'L'
	EI_MAG3		    = 3, // 'F'
	EI_CLASS	    = 4, // Architecture (32/64)
	EI_DATA		    = 5, // Byte Order
	EI_VERSION	    = 6, // ELF Version
	EI_OSABI	    = 7, // OS Specific
	EI_ABIVERSION	= 8, // OS Specific
	EI_PAD		    = 9  // Padding
} elf_ident_t;

typedef enum elf_type_t {
    ET_NONE		= 0, // Unkown Type
	ET_REL		= 1, // Relocatable File
	ET_EXEC		= 2  // Executable File
} elf_type_t;

typedef enum {
    PT_NULL    = 0,
    PT_LOAD    = 1, 
    PT_DYNAMIC = 2,
    PT_INTERP  = 3,
    PT_NOTE    = 4,
    PT_SHLIB   = 5,
    PT_PHDR    = 6
} elf_ph_type_t;

typedef struct elf32_header_t {
    uint8_t     e_ident[ELF_NIDENT];
    elf32_half  e_type;
    elf32_half  e_machine;
    elf32_word  e_version;
    elf32_addr  e_entry;
    elf32_off   e_phoff;
    elf32_off   e_shoff;
    elf32_word  e_flags;
    elf32_half  e_ehsize;
    elf32_half  e_phentsize;
    elf32_half  e_phnum;
    elf32_half  e_shentsize;
    elf32_half  e_shnum;
    elf32_half  e_shstrndx;
} elf32_header_t;

typedef struct elf32_phdr_t {
    elf32_word p_type;
    elf32_off  p_offset;
    elf32_addr p_vaddr;
    elf32_addr p_paddr;
    elf32_word p_filesz;
    elf32_word p_memsz;
    elf32_word p_flags;
    elf32_word p_align;
} elf32_phdr_t;

typedef struct elf_segment_t {
    void* src;
    void* virt_addr;
    size_t file_size;
    size_t mem_size;
    uint32_t flags;
} elf_segment_t;

typedef struct elf_load_info_t {
    elf_segment_t* segments;
    size_t segment_count;
    void* entry_point;
} elf_load_info_t;

bool elf_parse(void* elf_data, elf_load_info_t* out_info);

#endif // ELF_H