#include "interrupt/isr/isr.h"

#include "interrupt/idt/idt.h"

#include <stddef.h>
#include <stdint.h>

void isr_init(void) {
    for (size_t i = 0; i < ISR_EXCEPTION_AMOUNT; i++) {
        idt_set_gate(
            i,
            (uint32_t) isr_stubs[i],
            KERNEL_CODE_SEG,
            IDT_FLAG_PRESENT | IDT_FLAG_RING0 | IDT_FLAG_GATE_TASK_32BIT_INT
        );
    }
}