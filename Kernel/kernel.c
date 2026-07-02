#include "../CPU/isr.h"
#include "../CPU/timer.h"
#include "../drivers/keyboard.h"

void main() {
    isr_install();
    asm volatile("sti");
    init_timer(50);
    init_keyboard();
}