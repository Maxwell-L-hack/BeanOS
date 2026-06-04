.section .text
.global _start
.extern main
.extern _exit
.extern _libc_reent_init
.extern __libc_init_array

_start:
    cld 

    call _libc_reent_init
    call __libc_init_array 

    call main
    push %eax
    call _exit