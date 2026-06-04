#ifndef STRING_H
#define STRING_H

#include <stddef.h>

// Strings
int kstrcmp(const char* s1, const char* s2);
size_t kstrlen(const char* str);
char* kstrncpy(char* dest, const char* src, size_t n);
char* kstrdup(const char* src);

// Memory
void* kmemcpy(void* dest, const void* src, size_t size);
void* kmemset(void* dest, int ch, size_t count);

// Tokenize
char *kstrchr(const char *s, int c);
char *kstrtok(char *str, const char *delim);

#endif // STRING_H
