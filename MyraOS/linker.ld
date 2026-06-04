ENTRY(_start)
OUTPUT_FORMAT("elf32-i386")

SECTIONS {
    . = 0x100000;

    _kernel_start = .;

    .multiboot ALIGN(8) : {
        KEEP(*(.multiboot))
    }

    .boot.text : {
        *(.boot.text)
    }

    .boot.data : {
        *(.boot.data)
    }

    .boot.bss (NOLOAD) : {
        *(COMMON)
        *(.boot.bss)
        . = ALIGN(0x1000);
    }

    . += 0xC0000000;

    .text ALIGN(4K) : AT(ADDR(.text) - 0xC0000000) {
        *(.text)
    }

    .rodata ALIGN(4K) : AT(ADDR(.rodata) - 0xC0000000) {
        *(.rodata)
    }

    .data ALIGN(4K) : AT(ADDR(.data) - 0xC0000000) {
        *(.data)
    }

    .bss ALIGN(4K) : AT(ADDR(.bss) - 0xC0000000) {
        *(COMMON)
        *(.bss)
    }

    . = ALIGN(0x1000);
    _kernel_end = .;
}
