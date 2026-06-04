#include <stdint.h>
#include <stdbool.h>

#include "fb/fb.h"
#include "font/font.h"
#include "font/fonts/myra_font.h"
#include "gfx/gfx.h"
#include "vmm/vmm.h"

#define MB_TAG_TYPE_FRAMEBUFFER 8

uint32_t multiboot_info_addr;

typedef struct multiboot2_tag_framebuffer_t {
    uint32_t type;
    uint32_t size;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;
    uint16_t reserved;
} __attribute__((packed)) multiboot2_tag_framebuffer_t;

void parse_multiboot_info(uint32_t addr) {
    addr += 8; // skip total size + reserved

    while (1) {
        uint32_t type = *(uint32_t*) addr;
        uint32_t size = *(uint32_t*)(addr + 4);

        if (type == 0) {
            break;
        }

        if (type == MB_TAG_TYPE_FRAMEBUFFER) {
            multiboot2_tag_framebuffer_t* fb = (multiboot2_tag_framebuffer_t*) addr;

            uint32_t fb_phys = (uint32_t)(fb->framebuffer_addr);
            uint32_t fb_size = fb->framebuffer_pitch * fb->framebuffer_height;

            for (uint32_t i = 0; i < fb_size; i += PAGE_SIZE) {
                vmm_map_page(fb_phys + i, fb_phys + i, PAGE_PRESENT | PAGE_WRITE);
            }

            fb_info_t fb_info = {
                .addr = (argb_t*) fb_phys,
                .pitch = fb->framebuffer_pitch,
                .width = fb->framebuffer_width,
                .height = fb->framebuffer_height,
                .bpp = fb->framebuffer_bpp,
                .pixels_per_row = fb->framebuffer_pitch / sizeof(argb_t),
            };

            fb_init(fb_info);
            gfx_init();
            font_init_default(&myra_font);
        }

        addr += (size + 7) & ~7; // 8-byte align
    }
}
