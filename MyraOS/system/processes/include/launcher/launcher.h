#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <stdbool.h>

void launcher_request_launch(const char* path);
void launcher_launch_pending(void);
bool launcher_launch(const char* path);

#endif // LAUNCHER_H