#ifndef ULOG_H
#define ULOG_H

#include "print/print.h"
#include "vga/vga_color.h"
#include "keyboard/keyboard.h"

static inline void wait_for_key(void) {
    kprintf("Press any key to continue...\n");
    keyboard_read_blocking();
}

#define ulog_info(msg)  do { kset_color(COLOR_BLUE);  kprintf("[INFO]: %s\n", msg);  kset_color(COLOR_WHITE); wait_for_key(); } while (0)
#define ulog_warn(msg)  do { kset_color(COLOR_YELLOW); kprintf("[WARN]: %s\n", msg); kset_color(COLOR_WHITE); wait_for_key(); } while (0)
#define ulog_error(msg) do { kset_color(COLOR_RED);   kprintf("[ERROR]: %s\n", msg); kset_color(COLOR_WHITE); wait_for_key(); } while (0)

#define ulog_infof(fmt, ...)  do { kset_color(COLOR_BLUE);  kprintf("[INFO]: " fmt "\n", ##__VA_ARGS__);  kset_color(COLOR_WHITE); wait_for_key(); } while (0)
#define ulog_warnf(fmt, ...)  do { kset_color(COLOR_YELLOW); kprintf("[WARN]: " fmt "\n", ##__VA_ARGS__); kset_color(COLOR_WHITE); wait_for_key(); } while (0)
#define ulog_errorf(fmt, ...) do { kset_color(COLOR_RED);   kprintf("[ERROR]: " fmt "\n", ##__VA_ARGS__); kset_color(COLOR_WHITE); wait_for_key(); } while (0)

#define ulog_infof_nb(fmt, ...)  do { kset_color(COLOR_BLUE);  kprintf("[INFO]: " fmt "\n", ##__VA_ARGS__);  kset_color(COLOR_WHITE); } while (0)
#define ulog_warnf_nb(fmt, ...)  do { kset_color(COLOR_YELLOW); kprintf("[WARN]: " fmt "\n", ##__VA_ARGS__); kset_color(COLOR_WHITE); } while (0)
#define ulog_errorf_nb(fmt, ...) do { kset_color(COLOR_RED);   kprintf("[ERROR]: " fmt "\n", ##__VA_ARGS__); kset_color(COLOR_WHITE); } while (0)

#endif // ULOG_H
