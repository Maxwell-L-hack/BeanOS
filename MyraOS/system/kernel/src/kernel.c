#include <stdbool.h>
#include <stddef.h>

#include "block_device/pata.h"
#include "ext2/ext2.h"
#include "font/font.h"
#include "frame/frame.h"
#include "gdt/gdt.h"
#include "gfx/gfx.h"
#include "heap/heap.h"
#include "image/image_loader.h"
#include "interrupt/idt/idt.h"
#include "interrupt/irq/irq.h"
#include "interrupt/isr/isr.h"
#include "interrupt/syscall/syscall.h"
#include "keyboard/keyboard.h"
#include "mouse/mouse.h"
#include "pit/pit.h"
#include "pmm/pmm.h"
#include "print/print.h"
#include "rtc/rtc.h"
#include "schedule/schedule.h"
#include "screen/screen.h"
#include "stack/stack.h"
#include "tty/tty.h"
#include "vmm/vmm.h"

extern uint32_t multiboot_info_addr;
extern void parse_multiboot_info(uint32_t addr);

void kernel_main(void);
void kernel_memory_setup(void);
void kernel_idle(void);

void kernel_memory_setup(void) {
    pmm_init();
    vmm_init();
    
    heap_init(HEAP_START_ADDR, HEAP_SIZE);
    stack_init(STACK_BASE, STACK_SIZE, kernel_main);
}

void kernel_main(void) {
    gdt_init();

    idt_init();
    isr_init();
    irq_init();
    syscall_init();

    pit_init();
    rtc_init();

    keyboard_driver_install();
    tty_init();

    mouse_init();

    pata_init();

    block_device_t* block_device = block_get_device("hd0");
    if (!block_device) {
        klog_error("hd0 not found");
    }

    root_fs = (ext2_fs_t*) kmalloc(sizeof(ext2_fs_t));
    if (!ext2_mount(root_fs, block_device)) {
        klog_error("FS mount failed");
    }

    parse_multiboot_info(multiboot_info_addr);
    frame_set_screen(&screen_desktop);

    mouse_set(true);
    schedule_init();

    kernel_idle();
}

void kernel_idle(void) {
    while (true) {
        __asm__ volatile("sti; hlt;");
    }

    __builtin_unreachable();
}