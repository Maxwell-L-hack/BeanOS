#ifndef _BLKSIZE_T
typedef long blksize_t;
#define _BLKSIZE_T 1
#endif
#ifndef _BLKCNT_T
typedef long blkcnt_t;
#define _BLKCNT_T 1
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/reent.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <stddef.h>

#ifndef S_IFREG
#define S_IFREG 0100000
#endif
#ifndef S_IFCHR
#define S_IFCHR 0020000
#endif

#define SYS_EXIT   1
#define SYS_FORK   2
#define SYS_READ   3
#define SYS_WRITE  4
#define SYS_OPEN   5
#define SYS_CLOSE  6
#define SYS_EXECVE 11
#define SYS_LSEEK  19

extern struct _reent* _impure_ptr;

static inline int syscall(int num, int arg1, int arg2, int arg3) {
    int ret;
    __asm__ volatile("int $0x80" : "=a"(ret) : "a"(num), "b"(arg1), "c"(arg2), "d"(arg3) : "memory","cc");

    if (ret < 0) {
        errno = -ret;
        return -1;
    }

    return ret;
}

static inline int rsyscall(int num, int arg1, int arg2, int arg3) {
    int ret;
    __asm__ volatile("int $0x80" : "=a"(ret) : "a"(num), "b"(arg1), "c"(arg2), "d"(arg3) : "memory","cc");
    return ret;
}

void* _sbrk(ptrdiff_t);
int _lseek(int, int, int);
int _write(int, const void*, size_t);
int _read(int, void*, size_t);
int _open(const char*, int, ...);
int _close(int);
int _isatty(int);
int _fstat(int, struct stat*);

void * _sbrk_r(struct _reent *r, ptrdiff_t incr);
_ssize_t _write_r(struct _reent *r, int fd, const void *buf, size_t n);
_ssize_t _read_r (struct _reent *r, int fd, void *buf, size_t n);
int _open_r  (struct _reent *r, const char *path, int flags, int mode);
int _close_r (struct _reent *r, int fd);
_off_t _lseek_r(struct _reent *r, int fd, _off_t off, int whence);
int _fstat_r (struct _reent *r, int fd, struct stat *st);
int _isatty_r(struct _reent *r, int fd);

void* sbrk(ptrdiff_t inc) { return _sbrk_r(_impure_ptr, inc); }
long  lseek(int fd, long off, int wh) { return _lseek(fd, (int)off, wh); }
int   write(int fd, const void* b,size_t n) { return _write(fd, b, n); }
int   read(int fd, void* b, size_t n) { return _read(fd, b, n); }
int   open(const char* p, int f, ...) { return _open(p, f); }
int   close(int fd) { return _close(fd); }
int   isatty(int fd) { return _isatty(fd); }
int   fstat(int fd, struct stat* st) { return _fstat(fd, st); }

int mkdir(const char* path, mode_t mode) { 
    (void) path;
    (void) mode;

    errno = ENOSYS;

    return -1;
}

int unlink(const char* path) { 
    (void) path;

    errno = ENOSYS;

    return -1; 
}

int link(const char* o,const char* n) { 
    (void)o; (void)n; 

    errno = ENOSYS;

    return -1;
}

int kill(int pid,int sig) { 
    (void)pid; (void)sig;

    errno = ENOSYS;

    return -1;
}

int getpid(void) { 
    return 1;
}

int times(void* tms) { 
    (void)tms;

    errno = ENOSYS;

    return -1; 
}

// Be careful! This should match the heap in process.h
#define PROCESS_HEAP_START 0x800000
#define PROCESS_HEAP_SIZE  (1024 * 1024 * 24)

#define MALIGN 16

static inline uintptr_t align_up_u(uintptr_t x, uintptr_t a) {
    return (x + (a - 1)) & ~(a - 1);
}

static inline uintptr_t align_down_u(uintptr_t x, uintptr_t a) {
    return x & ~(a - 1);
}

static uintptr_t heap_end = PROCESS_HEAP_START;

void* _sbrk(ptrdiff_t incr) {
    uintptr_t prev = heap_end;

    if (incr != 0) {
        uintptr_t next_raw = heap_end + (uintptr_t)incr;
        uintptr_t next = (incr > 0) ? align_up_u(next_raw, MALIGN) : align_down_u(next_raw, MALIGN);
        if (next < PROCESS_HEAP_START || next > PROCESS_HEAP_START + PROCESS_HEAP_SIZE) {
            errno = ENOMEM;
            return (void*)-1;
        }

        heap_end = next;
    }

    return (void*)prev;
}

int _write(int fd, const void* buf, size_t count) {
    return syscall(SYS_WRITE, fd, (int)(uintptr_t)buf, (int)count);
}

void _exit(int status) {
    syscall(SYS_EXIT, status, 0, 0);
    for (;;) { }
}

int _read(int fd, void* buf, size_t count) {
    return syscall(SYS_READ, fd, (int)(uintptr_t)buf, (int)count);
}

int _open(const char* path, int flags, ...) {
    return syscall(SYS_OPEN, (int)(uintptr_t)path, flags, 0);
}

int _close(int fd) {
    return syscall(SYS_CLOSE, fd, 0, 0);
}

pid_t _fork(void) {
    return (pid_t)syscall(SYS_FORK, 0, 0, 0);
}

int _execve(const char* path, char* const argv[], char* const envp[]) {
    return syscall(SYS_EXECVE, (int)(uintptr_t)path, (int)(uintptr_t)argv, (int)(uintptr_t)envp);
}

int _fstat(int fd, struct stat* st) {
    if (!st) { 
        errno = EINVAL;
        return -1;
    }

    st->st_mode    = _isatty(fd) ? S_IFCHR : S_IFREG;
    st->st_blksize = 4096;
    st->st_blocks  = 0;
    st->st_size    = 0;

    off_t cur = _lseek(fd, 0, SEEK_CUR);
    if (cur == (off_t)-1) {
        return 0;
    }

    off_t end = _lseek(fd, 0, SEEK_END);
    if (end != (off_t)-1) {
        st->st_size = end;
    }

    _lseek(fd, cur, SEEK_SET);
    return 0;
}

int _isatty(int fd) {
    return (fd == 0 || fd == 1 || fd == 2) ? 1 : 0;
}

int _lseek(int fd, int ptr, int dir) {
    return syscall(SYS_LSEEK, fd, ptr, dir);
}

void* _sbrk_r(struct _reent* r, ptrdiff_t incr) {
    uintptr_t prev = heap_end;

    if (incr != 0) {
        uintptr_t next_raw = heap_end + (uintptr_t)incr;
        uintptr_t next = (incr > 0) ? align_up_u(next_raw, MALIGN) : align_down_u(next_raw, MALIGN);

        if (next < PROCESS_HEAP_START || next > PROCESS_HEAP_START + PROCESS_HEAP_SIZE) {
            errno = ENOMEM;
            if (r) { 
                r->_errno = ENOMEM;
            }
            
            return (void*)-1;
        }

        heap_end = next;
    }

    return (void*)prev;
}

_ssize_t _write_r(struct _reent *r, int fd, const void *buf, size_t n) {
    int ret = rsyscall(SYS_WRITE, fd, (int)(uintptr_t)buf, (int)n);
    if (ret < 0) {
        if (r != NULL) {
            r->_errno = -ret;
        }
        return -1;
    }
    return (_ssize_t)ret;
}

_ssize_t _read_r(struct _reent *r, int fd, void *buf, size_t n) {
    int ret = rsyscall(SYS_READ, fd, (int)(uintptr_t)buf, (int)n);
    if (ret < 0) {
        if (r != NULL) {
            r->_errno = -ret;
        }
        return -1;
    }
    return (_ssize_t)ret;
}

int _open_r(struct _reent *r, const char *path, int flags, int mode) {
    (void)mode;
    int ret = rsyscall(SYS_OPEN, (int)(uintptr_t)path, flags, 0);
    if (ret < 0) {
        if (r != NULL) {
            r->_errno = -ret;
        }
        return -1;
    }
    return ret;
}

int _close_r(struct _reent *r, int fd) {
    int ret = rsyscall(SYS_CLOSE, fd, 0, 0);
    if (ret < 0) {
        if (r != NULL) {
            r->_errno = -ret;
        }
        return -1;
    }
    return ret;
}

_off_t _lseek_r(struct _reent *r, int fd, _off_t off, int whence) {
    int ret = rsyscall(SYS_LSEEK, fd, (int)off, whence);
    if (ret < 0) {
        if (r != NULL) {
            r->_errno = -ret;
        }
        return (_off_t)-1;
    }
    return (_off_t)ret;
}

int _fstat_r(struct _reent *r, int fd, struct stat *st) {
    if (st == NULL) {
        if (r != NULL) {
            r->_errno = EINVAL;
        }
        return -1;
    }

    st->st_mode    = (fd == 0 || fd == 1 || fd == 2) ? S_IFCHR : S_IFREG;
    st->st_blksize = 4096;
    st->st_blocks  = 0;
    st->st_size    = 0;

    _off_t cur = _lseek_r(r, fd, 0, SEEK_CUR);
    if (cur != (_off_t)-1) {
        _off_t end = _lseek_r(r, fd, 0, SEEK_END);
        if (end != (_off_t)-1) {
            st->st_size = end;
        }
        _lseek_r(r, fd, cur, SEEK_SET);
    }

    return 0;
}

int _isatty_r(struct _reent *r, int fd) {
    (void)r;
    return (fd == 0 || fd == 1 || fd == 2) ? 1 : 0;
}

void __malloc_lock(struct _reent *r) {
    (void)r;
}

void __malloc_unlock(struct _reent *r) {
    (void)r;
}