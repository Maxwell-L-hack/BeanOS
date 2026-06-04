#include "libc_kernel/string.h"

#include "heap/heap.h"

void* kmemcpy(void* dest, const void* src, size_t size) {
    char *csrc = (char*)src; 
    char *cdest = (char*)dest; 

    for (size_t i = 0; i < size; i++) {
        cdest[i] = csrc[i];
    } 

    return dest;
}

int kstrcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }

    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

size_t kstrlen(const char* str) {
    const char* end = str;

    while (*end != '\0') {
        ++end;
    }

    return end - str;
}

char* kstrncpy(char* dest, const char* src, size_t n) {
    size_t i = 0;
    while (i < n && src[i]) {
        dest[i] = src[i];
        i++;
    }

    while (i < n) {
        dest[i++] = '\0';
    }
    
    return dest;
}

char* kstrdup(const char* src) {
    if (src == NULL) {
        return NULL;
    }

    size_t len = kstrlen(src);
    char* dup = kmalloc(len + 1);
    if (!dup) {
        return NULL;
    }

    for (size_t i = 0; i < len; i++) {
        dup[i] = src[i];
    }
    dup[len] = '\0';

    return dup;
}

void* kmemset(void* dest, int ch, size_t len) {
    unsigned char* cdest = dest;

    while (len--) {
        *cdest++ = (unsigned char)ch;
    }
    
    return cdest;
}

char *kstrchr(const char* s, int c) {
    while (*s) {
        if (*s == (char) c) {
            return (char*) s;
        }

        s++;
    }

    return 0;
}

char* kstrtok(char* str, const char* delim) {
    static char* next = NULL;
    if (str) {
        next = str;
    } else if (!next || !*next) {
        return NULL;
    }

    // Skip leading delimiters
    while (*next && kstrchr(delim, *next)) {
        next++;
    }

    if (!*next) {
        next = NULL;
        return NULL;
    }

    char* start = next;

    // Find end of token
    while (*next && !kstrchr(delim, *next)) {
        next++;
    }

    if (*next) {
        *next = '\0';
        next++;
    } else {
        next = NULL;
    }

    // Return NULL for empty token
    if (*start == '\0') {
        return NULL;
    }

    return start;
}