#ifndef DESKTOP_ICONS_H
#define DESKTOP_ICONS_H

#include <stdint.h>

#define ICON_TITLE_LEN 64
#define ICON_PATH_LEN 256

typedef struct {
    char id[ICON_TITLE_LEN];
    char title[ICON_TITLE_LEN];
    char exec_path[ICON_PATH_LEN];
    char icon_path[ICON_PATH_LEN];
    int x;
    int y;
} desktop_icon_t;

desktop_icon_t* icons_ini_load(const char* path, int* count);

#endif // DESKTOP_ICONS_H