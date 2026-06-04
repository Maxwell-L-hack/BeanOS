#include <stdint.h>
#include <stddef.h>
#include <errno.h>

#define SYS_TIME         13
#define SYS_GETTIMEOFDAY 78
#define SYS_NANOSLEEP   162

typedef unsigned int useconds_t;

struct timeval {
    long tv_sec;
    long tv_usec;
};

static inline int syscall(int n, int a, int b, int c) {
    int r;
    __asm__ volatile("int $0x80" : "=a"(r) : "a"(n), "b"(a), "c"(b), "d"(c));
    return r;
}

int _gettimeofday(struct timeval* tv, void* tz) {
    (void)tz;
    if (!tv) {
        errno = EINVAL;
        return -1;
    }

    return syscall(SYS_GETTIMEOFDAY, (int)(uintptr_t)tv, 0, 0);
}

int _sleep(unsigned int sec) {
    syscall(SYS_NANOSLEEP, (int)(sec * 1000u), 0, 0);
    return 0;
}

int _usleep(useconds_t usec) {
    unsigned ms = (usec + 999u) / 1000u;
    syscall(SYS_NANOSLEEP, (int)ms, 0, 0);
    return 0;
}

uint32_t _time(uint32_t* tloc) {
    uint32_t t = (uint32_t)syscall(SYS_TIME, 0, 0, 0);
    if (tloc) {
        *tloc = t;
    }
    return t;
}

int usleep(useconds_t usec) {
    return _usleep(usec);
}

unsigned int sleep(unsigned int sec) {
    _sleep(sec);
    return 0;
}

int gettimeofday(struct timeval* tv, void* tz) {
    return _gettimeofday(tv, tz);
}

uint32_t time(uint32_t* tloc) {
    return _time(tloc);
}
