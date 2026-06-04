#ifndef SYSCALL_NUMBERS_H
#define SYSCALL_NUMBERS_H

// ========== Custom / Testing ==========
#define SYS_TEST              0

// ========== Process Control ==========
#define SYS_EXIT              1
#define SYS_FORK              2
#define SYS_EXECVE            11
#define SYS_WAITPID           7
#define SYS_KILL              37
#define SYS_GETPID            20
#define SYS_GETPPID           64
#define SYS_GETUID            24
#define SYS_GETEUID           49
#define SYS_SETUID            23
#define SYS_GETGID            47
#define SYS_SETGID            46
#define SYS_GETEGID           50
#define SYS_SETREUID          70
#define SYS_SETREGID          71
#define SYS_CLONE             120
#define SYS_EXIT_GROUP        252

// ========== File System ==========
#define SYS_OPEN              5
#define SYS_CLOSE             6
#define SYS_READ              3
#define SYS_WRITE             4
#define SYS_LSEEK             19
#define SYS_STAT              106
#define SYS_LSTAT             107
#define SYS_FSTAT             108
#define SYS_ACCESS            33
#define SYS_CHMOD             15
#define SYS_FCHMOD            94
#define SYS_UNLINK            10
#define SYS_LINK              9
#define SYS_SYMLINK           83
#define SYS_READLINK          85
#define SYS_MKDIR             39
#define SYS_RMDIR             40
#define SYS_RENAME            38
#define SYS_TRUNCATE          92
#define SYS_FTRUNCATE         93
#define SYS_GETCWD            183
#define SYS_CHDIR             12
#define SYS_FCHDIR            133

// ========== File Descriptors ==========
#define SYS_DUP               41
#define SYS_DUP2              63
#define SYS_PIPE              42
#define SYS_IOCTL             54
#define SYS_FCNTL             55

// ========== Memory Management ==========
#define SYS_BRK               45
#define SYS_MMAP              90
#define SYS_MUNMAP            91
#define SYS_MPROTECT          125
#define SYS_MADVISE           219
#define SYS_MREMAP            163

// ========== Signals ==========
#define SYS_SIGNAL            48
#define SYS_SIGACTION         67
#define SYS_SIGRETURN         119
#define SYS_KILL              37
#define SYS_SIGPROCMASK       126
#define SYS_SIGSUSPEND        72

// ========== Time ==========
#define SYS_TIME              13
#define SYS_GETTIMEOFDAY      78
#define SYS_SETTIMEOFDAY      79
#define SYS_NANOSLEEP         162
#define SYS_ALARM             27

// ========== Scheduling ==========
#define SYS_NICE              34
#define SYS_GETPRIORITY       96
#define SYS_SETPRIORITY       97
#define SYS_SCHED_YIELD       158

// ========== IPC (SysV) ==========
#define SYS_SHMGET            29
#define SYS_SHMAT             30
#define SYS_SHMDT             67
#define SYS_SHMCTL            31

// ========== Networking ==========
#define SYS_SOCKETCALL        102
#define SYS_SOCKET            281
#define SYS_CONNECT           283
#define SYS_SENDTO            289
#define SYS_RECVFROM          292
#define SYS_BIND              282
#define SYS_LISTEN            284
#define SYS_ACCEPT            285

// ========== Misc ==========
#define SYS_GETDENTS          141
#define SYS_SYSINFO           116
#define SYS_UMASK             60
#define SYS_REBOOT            88
#define SYS_TTYNAME           132
#define SYS_TIMES             43
#define SYS_UNAME             122

// ========== Windows/Graphics ==========
#define SYS_WINDOW_CREATE         350
#define SYS_WINDOW_GET_SURFACE    351
#define SYS_WINDOW_DESTROY        352
#define SYS_WINDOW_PRESENT        353
#define SYS_WINDOW_SET_TITLE      354

#endif // SYSCALL_NUMBERS_H
