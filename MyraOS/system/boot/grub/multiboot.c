#include <stdint.h>

#define MB_MAGIC           0xE85250D6u        
#define MB_ARCH_I386       0
#define MB2_HEADER_LENGTH  48
#define MB_CHECKSUM -(MB_MAGIC + MB_ARCH_I386 + MB2_HEADER_LENGTH)

#define MB_FB_TAG_TYPE     5
#define MB_FB_TAG_SIZE     24

#define SCREEN_WIDTH       1920
#define SCREEN_HEIGHT      1080
#define SCREEN_BPP         32
#define SCREEN_DEPTH       32

#define MB_END_TAG         0
#define MB_END_TAG_SIZE    8

__attribute__((section(".multiboot"), used, aligned(8)))
static const uint32_t multiboot_header[] = {
    MB_MAGIC,
    MB_ARCH_I386,
    MB2_HEADER_LENGTH,
    MB_CHECKSUM,

    MB_FB_TAG_TYPE,
    MB_FB_TAG_SIZE,
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    SCREEN_BPP,
    SCREEN_DEPTH,

    MB_END_TAG,
    MB_END_TAG_SIZE
};