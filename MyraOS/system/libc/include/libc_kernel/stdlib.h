#ifndef STDLIB_H
#define STDLIB_H

#include <stdint.h>

int katoi(const char* str);

char* kitoa(int value, char* str, int base);

char* kutoa(uint32_t value, char* str, int base);

#endif // STDLIB_H
