#ifndef UI_EVENT_H
#define UI_EVENT_H

#include <stdint.h>
#include <stdbool.h>

typedef enum ui_event_type_t {
    UI_EVENT_CLICK,
    UI_EVENT_HOVER,
    UI_EVENT_KEY,
    UI_EVENT_TICK,
    UI_EVENT_CUSTOM,
} ui_event_type_t;

typedef struct ui_event_t {
    ui_event_type_t type;
    union {
        struct {
            uint32_t rel_x, rel_y;
        } click;

        struct {
            uint32_t rel_x, rel_y;
        } hover;

        struct {
            int key_code;
            bool pressed;
        } key;

        struct {
            uint32_t system_ticks;
        } tick;

        void* custom_event_data;
    };
} ui_event_t;

#endif // UI_EVENT_H
