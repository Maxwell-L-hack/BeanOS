[bits 32]

KERNEL_DATA_SEG equ 0x10

extern syscall_handler

global _syscall_stub
_syscall_stub:
    ; Push the int num (required by the reg struct)
    push 0
    push 0x80

    pusha

    push ds
	push es
	push fs
	push gs

    xor eax, eax
    mov ax, KERNEL_DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov eax, esp
    push eax

    cld
    sti
    call syscall_handler
    add esp, 4

    pop gs
	pop fs
	pop es
	pop ds

    popa

    add esp, 8
    iret
    