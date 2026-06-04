#ifndef WINDOW_WIDGET_H
#define WINDOW_WIDGET_H

#include "window/window.h"
#include "ui/ui.h"

widget_t* widget_os_window_create(char* title, win_handle_t handle, uint32_t x, uint32_t y, uint32_t app_w, uint32_t app_h);

#endif // WINDOW_WIDGET_H
