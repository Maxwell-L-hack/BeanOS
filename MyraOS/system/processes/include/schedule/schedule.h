#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "process/process.h"
#include "interrupt/idt/idt.h"

#define SCHEDULE_MAX_COUNT 128

extern process_t* schedule_current_proc;
extern bool proc_launched;

// Init multiprocessing data structure and memory
void schedule_init(void);

// Switch to the next process in the data structure
void schedule_next(void);

// Save registers of a proc for context switch
void schedule_save_context(registers_t* regs);

// Add a new process to the multiprocess data structure
bool schedule_proc(process_t* proc);

// Stop the running process now
void schedule_close_current_proc(void); 

#endif // SCHEDULE_H