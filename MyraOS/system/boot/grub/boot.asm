[bits 32]

%define KERNEL_VMA 0xC0000000

%define PAGE_PRESENT 0x1
%define PAGE_WRITE 0x2
%define FLAGS (PAGE_PRESENT | PAGE_WRITE)

section .bss
align 16
stack_bottom:
resb 1024 * 128
stack_top:

section .boot.text
global _start
_start:
    cli
    
    ; Init paging
    extern _load_page_directory
    mov ecx, (page_directory - KERNEL_VMA)
    push ecx
    call _load_page_directory

    extern _enable_paging
    call _enable_paging

    jmp higher_half

section .text
higher_half:
    mov esp, stack_top
    push ebx
    push eax
    xor ebp, ebp

    extern multiboot_info_addr
    mov [multiboot_info_addr], ebx
    
    extern kernel_memory_setup
    call kernel_memory_setup
.hang:
    cli

    hlt
    jmp .hang

section .data
align 4096
global page_directory
page_directory:
    dd page_table - KERNEL_VMA + FLAGS  
    times 768-1 dd 0

    dd page_table - KERNEL_VMA + FLAGS       
    
    times 256-2 dd 0

    dd page_directory - KERNEL_VMA + FLAGS   

align 4096
global page_table
page_table:
    %assign i 0
    %rep 1024
        dd (i << 12) | FLAGS
        %assign i i+1
    %endrep