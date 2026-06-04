#ifndef LOG_H
#define LOG_H

#include <print/print.h>

#define klog_info(msg)  do { kset_color(COLOR_BLUE);   kprintf("[KERNEL INFO]: %s\n", msg);  kset_color(COLOR_WHITE); } while (0)
#define klog_warn(msg)  do { kset_color(COLOR_YELLOW); kprintf("[KERNEL WARN]: %s\n", msg);  kset_color(COLOR_WHITE); } while (0)
#define klog_error(msg) do { kset_color(COLOR_RED);    kprintf("[KERNEL ERROR]: %s\n", msg); kset_color(COLOR_WHITE); } while (0)

#endif // LOG_H