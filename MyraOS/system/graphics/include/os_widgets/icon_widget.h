#ifndef ICON_WIDGET_H
#define ICON_WIDGET_H

#include "ui/ui.h"

typedef struct {
    char* title;
    char* icon_path;
    char* exec_path;
} icon_desc_t;

widget_t* widget_os_icon_create(uint32_t x, uint32_t y, icon_desc_t* desc);

#endif