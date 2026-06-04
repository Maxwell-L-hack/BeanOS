#ifndef ASSERT_H
#define ASSERT_H

#include "panic/panic.h"

#define STRINGIFY_STEP_2(x) #x
#define STRINGIFY(x) STRINGIFY_STEP_2(x)

#ifdef NDEBUG
#define kassert(cond) ((void)0)
#else
#define kassert(cond) \
    do { \
        if (!(cond)) { \
            kpanic("Assertion failed: " #cond ", file " __FILE__ ", line " STRINGIFY(__LINE__)); \
        } \
    } while (0)
#endif // NDEBUG

#endif // ASSERT_H