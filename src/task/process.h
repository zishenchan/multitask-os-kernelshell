#ifndef PROCESS_H
#define PROCESS_H
#include <stdint.h>
#include "task.h"
#include "config.h"

/**
 * open a notepad in windows, is the process,next is defining how kernel see the process,
 * the next process can be expended if we need to
 */
struct process
{
    // The process id
    uint16_t id;

    char filename[MULTITASK_OS_KERNELSHELL_MAX_PATH];

    // The main process task
    struct task* task;

    // The memory (malloc) allocations of the process, free memory by tracking memory have been allocated
    void* allocations[MULTITASK_OS_KERNELSHELL_MAX_PROGRAM_ALLOCATIONS];

    // The physical pointer to the process memory.
    void* ptr;

    // The physical pointer to the stack memory
    void* stack;

    // The size of the data pointed to by "ptr"
    uint32_t size;

};

int process_load(const char* filename, struct process** process);

int process_load_for_slot(const char* filename, struct process** process, int process_slot);

#endif