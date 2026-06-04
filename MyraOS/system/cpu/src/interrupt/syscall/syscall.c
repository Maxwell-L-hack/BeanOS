#include "interrupt/syscall/syscall.h"

#include <stddef.h>

#include "interrupt/syscall/syscall_numbers.h"

extern void _syscall_stub(void);

typedef uint32_t (*syscall_func_t)(uint32_t, uint32_t, uint32_t, uint32_t);

static syscall_func_t syscall_table[SYSCALL_MAX];

extern uint32_t sys_exit(uint32_t, uint32_t, uint32_t, uint32_t);
extern uint32_t sys_open(uint32_t, uint32_t, uint32_t, uint32_t);
extern uint32_t sys_close(uint32_t, uint32_t, uint32_t, uint32_t);
extern uint32_t sys_write(uint32_t, uint32_t, uint32_t, uint32_t);
extern uint32_t sys_read(uint32_t, uint32_t, uint32_t, uint32_t);
extern uint32_t sys_forkexec(uint32_t, uint32_t, uint32_t, uint32_t);
extern uint32_t sys_time(uint32_t, uint32_t, uint32_t, uint32_t);
extern uint32_t sys_sleep(uint32_t, uint32_t, uint32_t, uint32_t);
extern uint32_t sys_lseek(uint32_t, uint32_t, uint32_t, uint32_t);

extern uint32_t sys_window_create(uint32_t, uint32_t, uint32_t, uint32_t);
extern uint32_t sys_window_get_surface(uint32_t, uint32_t, uint32_t, uint32_t);
extern uint32_t sys_window_destroy(uint32_t, uint32_t, uint32_t, uint32_t);
extern uint32_t sys_window_present(uint32_t, uint32_t, uint32_t, uint32_t);
extern uint32_t sys_window_set_title(uint32_t, uint32_t, uint32_t, uint32_t);

static void register_syscall(uint32_t index, syscall_func_t func) {
    if (index < SYSCALL_MAX) {
        syscall_table[index] = func;
    }
}

void syscall_init(void) {
    idt_set_gate(
        SYSCALL_INTERRUPT,
        (uint32_t) _syscall_stub,
        KERNEL_CODE_SEG,
        IDT_FLAG_PRESENT | IDT_FLAG_RING3 | IDT_FLAG_GATE_TASK_32BIT_INT
    );

    register_syscall(SYS_EXIT,       sys_exit);
    register_syscall(SYS_OPEN,       sys_open);
    register_syscall(SYS_CLOSE,      sys_close);
    register_syscall(SYS_WRITE,      sys_write);
    register_syscall(SYS_READ,       sys_read);
    register_syscall(SYS_EXECVE,     sys_forkexec);
    register_syscall(SYS_TIME,       sys_time);
    register_syscall(SYS_NANOSLEEP,  sys_sleep);
    register_syscall(SYS_LSEEK,      sys_lseek);

    register_syscall(SYS_WINDOW_CREATE,            sys_window_create);
    register_syscall(SYS_WINDOW_GET_SURFACE,       sys_window_get_surface);
    register_syscall(SYS_WINDOW_DESTROY,           sys_window_destroy);
    register_syscall(SYS_WINDOW_PRESENT,           sys_window_present);
    register_syscall(SYS_WINDOW_SET_TITLE,         sys_window_set_title);

}

void syscall_handler(const registers_t* regs) {
    uint32_t func_num = regs->eax;

    if (func_num >= SYSCALL_MAX || syscall_table[func_num] == NULL) {
        return;
    }

    uint32_t result = syscall_table[func_num](regs->ebx, regs->ecx, regs->edx, regs->esi);

    ((registers_t*)regs)->eax = result;
}