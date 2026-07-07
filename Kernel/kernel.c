#include "../CPU/isr.h"
#include "../libc/string.h"
#include "../drivers/screen.h"
#include "kernel.h"

void main() {
    isr_install();
    irq_install();
    kprint("When something is typed, it will go through to the kernel\n"
    "Type END to halt the CPU\n>");
}

void user_input(char *input) {
    if (strcmp(input, "END") == 0) {
        kprint("Stopping the CPU.\n");
        asm volatile("hlt");
    }
    kprint("You typed: ");
    kprint(input);
    kprint("\n> ");
}