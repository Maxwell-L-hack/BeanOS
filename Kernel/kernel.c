#include "../drivers/screen.h"
#include "util.h"
#include "../CPU/isr.h"
#include "../CPU/idt.h"

void main() {
    isr_install();
    __asm__ __volatile__("int $2");
    __asm__ __volatile__("int $3");
}