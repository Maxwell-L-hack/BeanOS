#include "power/power.h"
#include "io/port_io.h"

static bool is_vm(void) {
    uint32_t eax, ebx, ecx, edx;

    __asm__ volatile(
        "cpuid"
        : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
        : "a"(1), "c"(0)
    );

    return (ecx >> 31) & 1u;
}

void power_shutdown(void) {
    if (is_vm()) {
        outw(0x604, 0x2000);
        outw(0xB004, 0x2000);
        outw(0x4004, 0x3400);
        outw(0x0604, 0x2000);
        outb(0xF4, 0x00);
    }

    for (;;) {
        __asm__ volatile("cli; hlt");
    }
}

void power_reboot(void) {
    for (;;) {
        outb(0x64, 0xFE);
    }
}