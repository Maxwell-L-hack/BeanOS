[bits 32]

; void _idt_load(idt_descriptor_t* idt_descriptor);
global _idt_load

_idt_load:
    ; make new call frame
    push ebp
    mov ebp, esp

    ; load idt
    mov eax, [esp + 8]
    lidt [eax]

    ; restore old call frame
    mov esp, ebp
    pop ebp
    
    ret 