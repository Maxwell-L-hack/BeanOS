bits 32

%define GDT_USER_CODE 0x1B
%define GDT_USER_DATA 0x23

; void _switch_to_proc_space(uint32_t entry, uint32_t user_stack, uint32_t user_stack_top, uint32_t page_dir_phys)
global _switch_to_proc_space
_switch_to_proc_space:
    cli

    ; No need to create stack frame as we have no return
    mov ebx, [esp + 4]
    mov ecx, [esp + 8]
    mov edx, [esp + 12]
    mov esi, [esp + 16]

    mov cr3, esi

    xor eax, eax
    mov ax, GDT_USER_DATA
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax     

    ; stack for iret: SS, ESP, EFLAGS, CS, EIP
    push dword GDT_USER_DATA   ; SS  (ring‑3)
    push ecx                   ; ESP
    push dword 0x202           ; EFLAGS (IF=1)
    push dword GDT_USER_CODE   ; CS  (ring‑3)
    push ebx                   ; EIP

    ; iret sets esp, we also need to set ebp
    mov ebp, edx

    sti
    iretd                      ; pop & jump to ring‑3
