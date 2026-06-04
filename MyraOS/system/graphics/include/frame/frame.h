#ifndef FRAME_H
#define FRAME_H

#include "keyboard/keyboard.h"

typedef struct {
    void (*init)(void);
    void (*handle_input)(key_event_t key);
} screen_t;

void frame_render(void);

void frame_set_screen(screen_t* new_screen);

#endif // FRAME_H