#include "process/process.h"

#include "gdt/gdt.h"
#include "heap/heap.h"
#include "libc_kernel/string.h"
#include "pmm/pmm.h"
#include "schedule/schedule.h"

#define PROC_PAGE_DIR 0xDE000000 

static size_t current_pid = 0;

static void copy_user_code(uint32_t page_dir_phys, exec_info_t* exec_info) {
    uint32_t saved_cr3;
    __asm__ volatile("mov %%cr3, %0" : "=r"(saved_cr3));
    __asm__ volatile("mov %0, %%cr3" :: "r"(page_dir_phys));

    for (size_t i = 0; i < exec_info->segment_count; i++) {
        exec_segment_t* seg = &exec_info->segments[i];

        uint32_t virt_addr = seg->virt_addr;
        uint32_t start_page = PAGE_ALIGN_DOWN(virt_addr);
        uint32_t end_page = PAGE_ALIGN_UP(virt_addr + seg->mem_size);
        uint32_t num_pages = (end_page - start_page) / PAGE_SIZE;

        for (uint32_t j = 0; j < num_pages; j++) {
            uint32_t page_virt = start_page + j * PAGE_SIZE;
            uint32_t frame = (uint32_t) pmm_alloc_page();

            uint32_t page_flags = PAGE_PRESENT | PAGE_USER;
            if (seg->flags & SEGMENT_WRITE) {
                page_flags |= PAGE_WRITE;
            }

            vmm_map_page(page_virt, frame, page_flags);
        }

        kmemcpy((void*)virt_addr, seg->data, seg->file_size);
        
        if (seg->mem_size > seg->file_size) {
            kmemset((void*)(virt_addr + seg->file_size), 0, seg->mem_size - seg->file_size);
        }
    }

    __asm__ volatile("mov %0, %%cr3" :: "r"(saved_cr3));
}

static void map_proc_segments(uint32_t page_dir_phys, uint32_t kernel_stack_top) {
    uint32_t saved_cr3;
    __asm__ volatile("mov %%cr3, %0" : "=r"(saved_cr3));
    __asm__ volatile("mov %0, %%cr3" :: "r"(page_dir_phys));

    for (size_t i = 0; i < (KERNEL_STACK_SIZE / PAGE_SIZE); i++) {
        uint32_t virt = kernel_stack_top - i * PAGE_SIZE;
        uint32_t phys = (uint32_t) pmm_alloc_page();

        vmm_map_page(virt, phys, PAGE_PRESENT | PAGE_WRITE);
    }

    for (size_t i = 0; i < PROCESS_STACK_SIZE / PAGE_SIZE; i++) {
        uint32_t stack_page_virt = PROCESS_STACK_START - i * PAGE_SIZE;
        uint32_t frame = (uint32_t) pmm_alloc_page();

        vmm_map_page(stack_page_virt, frame, PAGE_PRESENT | PAGE_WRITE | PAGE_USER);
    }

    for (uint32_t addr = PROCESS_HEAP_START; addr < PROCESS_HEAP_START + PROCESS_HEAP_SIZE; addr += PAGE_SIZE) {
        uint32_t frame = (uint32_t) pmm_alloc_page();
        vmm_map_page(addr, frame, PAGE_PRESENT | PAGE_WRITE | PAGE_USER);
    }

    __asm__ volatile("mov %0, %%cr3" :: "r"(saved_cr3));
}

process_t* proc_create(exec_info_t* exec_info) {
    __asm__ volatile("cli");

    process_t* process = (process_t*) kmalloc(sizeof(process_t));
    kmemset(process, 0, sizeof(process_t));
    
    page_directory_t* page_dir_phys = (page_directory_t*) pmm_alloc_page();
    page_directory_t* page_dir_virt = (page_directory_t*) PROC_PAGE_DIR;
    
    vmm_map_page((uint32_t) page_dir_virt, (uint32_t) page_dir_phys, PAGE_PRESENT | PAGE_WRITE);
    kmemset(page_dir_virt, 0, PAGE_SIZE);

    // Copy the kernel to the page dir of the proc
    kmemcpy(&page_dir_virt->entries[KERNEL_ENTRY_START], &current_page_directory_virt->entries[KERNEL_ENTRY_START], (PAGE_ENTRIES - KERNEL_ENTRY_START - 1) * sizeof(uint32_t));
    page_dir_virt->entries[PAGE_ENTRIES - 1] = (uint32_t) page_dir_phys | PAGE_PRESENT | PAGE_WRITE;

    copy_user_code((uint32_t) page_dir_phys, exec_info);

    // Create and map the kernel stack
    uint32_t kernel_stack_top = KERNEL_STACK_BASE - (current_pid % SCHEDULE_MAX_COUNT) * KERNEL_STACK_SIZE;
    map_proc_segments((uint32_t) page_dir_phys, kernel_stack_top);

    vmm_unmap_page((uint32_t) page_dir_virt);

    task_state_t* task_state = (task_state_t*) kmalloc(sizeof(task_state_t));
    kmemset(task_state, 0, sizeof(task_state_t));
    task_state->esp = (PROCESS_STACK_START & ~0xF);
    task_state->eip = (uint32_t) exec_info->entry_point;
    task_state->cs = GDT_USER_CODE;
    task_state->ds =
    task_state->es =
    task_state->fs =
    task_state->gs =
    task_state->ss = GDT_USER_DATA;
    task_state->eflags = 0x202; // IF = 1
        
    process->pid = current_pid++;
    process->state = PROCESS_NEW;
    process->page_directory = (uint32_t) page_dir_phys;
    process->regs = task_state;
    process->kernel_stack = kernel_stack_top;
    
    __asm__ volatile("sti");

    return process;
}

void proc_destroy(process_t* process) {
    if (!process) {
        return;
    }

    uint32_t saved_cr3;
    __asm__ volatile("mov %%cr3, %0" : "=r"(saved_cr3));
    __asm__ volatile("cli");
    __asm__ volatile("mov %0, %%cr3" :: "r"(process->page_directory));

    for (size_t i = 0; i < KERNEL_ENTRY_START; i++) {
        if (current_page_directory_virt->entries[i] & PAGE_PRESENT) {
            page_table_t* pt = (page_table_t*)(RECURSIVE_PAGE_TABLE_BASE + i * PAGE_SIZE);
            for (size_t j = 0; j < PAGE_ENTRIES; j++) {
                if (pt->entries[j] & PAGE_PRESENT) {
                    uint32_t phys = pt->entries[j] & ~0xFFF;
                    pmm_free_page((void*) phys);
                }
            }

            uint32_t pt_phys = current_page_directory_virt->entries[i] & ~0xFFF;
            pmm_free_page((void*) pt_phys);

            current_page_directory_virt->entries[i] = 0;
        }
    }

    __asm__ volatile("mov %0, %%cr3" :: "r"(saved_cr3));
    __asm__ volatile("sti");

    pmm_free_page((void*) process->page_directory);
    
    kfree(process->regs);
    kfree(process);
}