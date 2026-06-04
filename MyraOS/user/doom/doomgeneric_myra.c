#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <window.h>
#include <sys/time.h>

#include "doomgeneric.h"
#include "doomkeys.h"

extern pixel_t* DG_ScreenBuffer;
win_handle_t window_buffer;
win_surface_desc_t surface;

void DG_Init(void) {
    window_buffer = window_create(DOOMGENERIC_RESX, DOOMGENERIC_RESY, 0);
    window_get_surface(window_buffer, &surface);
}

void DG_DrawFrame(void) {
    for (uint32_t y = 0; y < surface.h; y++) {
        const uint32_t* src = (const uint32_t*)DG_ScreenBuffer + y * surface.w;
        uint32_t* dst = (uint32_t*)((uint8_t*)surface.pixels + y * surface.pitch);
        
        for (uint32_t x = 0; x < surface.w; x++) {
            dst[x] = src[x] | 0xFF000000u;
        }
    }

    window_present(window_buffer);
}

void DG_SleepMs(uint32_t ms) {
    usleep(ms * 1000);
}

uint32_t DG_GetTicksMs(void) {
    struct timeval tv;

    if (gettimeofday(&tv, 0) == 0) {
        unsigned long long ms = (unsigned long long)tv.tv_sec * 1000ull + (unsigned long long)tv.tv_usec / 1000ull;
        return (uint32_t)ms;
    }

    return (uint32_t)(time(0) * 1000u);
}

static const unsigned char make_to_ascii[128] = {
    [0x10]='q',[0x11]='w',[0x12]='e',[0x13]='r',[0x14]='t',[0x15]='y',[0x16]='u',[0x17]='i',[0x18]='o',[0x19]='p',
    [0x1E]='a',[0x1F]='s',[0x20]='d',[0x21]='f',[0x22]='g',[0x23]='h',[0x24]='j',[0x25]='k',[0x26]='l',
    [0x2C]='z',[0x2D]='x',[0x2E]='c',[0x2F]='v',[0x30]='b',[0x31]='n',[0x32]='m',
    [0x02]='1',[0x03]='2',[0x04]='3',[0x05]='4',[0x06]='5',[0x07]='6',[0x08]='7',[0x09]='8',[0x0A]='9',[0x0B]='0',
    [0x0C]='-',[0x0D]='=',[0x1A]='[',[0x1B]=']',[0x2B]='\\',[0x27]=';',[0x28]='\'',[0x29]='`',[0x33]=',',[0x34]='.',[0x35]='/',
    [0x39]=' '
};

static unsigned char map_make_to_doom(uint8_t make, int extended) {
    switch (make) {
        case 0x1C: return KEY_ENTER;
        case 0x0E: return KEY_BACKSPACE;
        case 0x0F: return KEY_TAB;
        case 0x01: return KEY_ESCAPE;
        case 0x39: return KEY_USE;
        case 0x48: return KEY_UPARROW;
        case 0x50: return KEY_DOWNARROW;
        case 0x4B: return KEY_LEFTARROW;
        case 0x4D: return KEY_RIGHTARROW;
        case 0x1D: return KEY_FIRE;
        case 0x2A: case 0x36: return KEY_RSHIFT;
        default: return make_to_ascii[make]; 
    }
}

int DG_GetKey(int* pressed, unsigned char* key) {
    unsigned char b;
    ssize_t n = read(0, &b, 1);
    if (n != 1) {
        return 0;
    }

    int is_down = (b & 0x80) ? 0 : 1;
    uint8_t make = b & 0x7F;

    unsigned char dk = map_make_to_doom(make, 0);
    if (!dk) {
        return 0;
    }

    *pressed = is_down;
    *key = dk;

    return 1;
}

void DG_SetWindowTitle(const char* title) {
    window_set_title(window_buffer, title);
}
