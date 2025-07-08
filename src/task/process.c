#include "process.h"
#include "config.h"
#include "status.h"
#include "task/task.h"
#include "memory/memory.h"
#include "string/string.h"
#include "fs/file.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "kernel.h"

// The current process that is running
struct process* current_process = 0;

static struct process* processes[MULTITASK_OS_KERNELSHELL_MAX_PROCESSES] = {};

static void process_init(struct process* process)
{
    memset(process, 0, sizeof(struct process)); // initialize the process being passed
}

struct process* process_current()
{
    return current_process;
}

struct process* process_get(int process_id) // the index
{
    if (process_id < 0 || process_id >= MULTITASK_OS_KERNELSHELL_MAX_PROCESSES) // id means the index
    {
        return NULL;
    }

    return processes[process_id]; // return the pointer to the process
}

static int process_load_binary(const char* filename, struct process* process)
{
    int res = 0;
    int fd = fopen(filename, "r");
    if (!fd)
    {
        res = -EIO; // if the file does not exist
        goto out;
    }

    struct file_stat stat;
    res = fstat(fd, &stat);
    if (res != MULTITASK_OS_KERNELSHELL_ALL_OK)
    {
        goto out;
    }

    void* program_data_ptr = kzalloc(stat.filesize); // need to know the size of file
    if (!program_data_ptr)
    {
        res = -ENOMEM;
        goto out;
    }

    if (fread(program_data_ptr, stat.filesize, 1, fd) != 1) // read the memory we just created
    {
        res = -EIO;
        goto out;
    }

    process->ptr = program_data_ptr;
    process->size = stat.filesize;

out:
    fclose(fd);
    return res;
}

static int process_load_data(const char* filename, struct process* process)
{
    int res = 0;
    res = process_load_binary(filename, process);
    return res;
}


int process_map_binary(struct process* process)
{
    int res = 0;
    paging_map_to(process->task->page_directory, (void*) MULTITASK_OS_KERNELSHELL_PROGRAM_VIRTUAL_ADDRESS, process->ptr, paging_align_address(process->ptr + process->size), PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE);
    return res;
    /**
     * we are mapping to the directory, 
     * in c, we need to map the memory 
     */
}

// the parameter is the ready-loaded process at this point
int process_map_memory(struct process* process)
{
    int res = 0;
    res = process_map_binary(process);
    return res;
}

int process_get_free_slot()
{
    for (int i = 0; i < MULTITASK_OS_KERNELSHELL_MAX_PROCESSES; i++)
    {
        if (processes[i] == 0)
            return i;
    }

    return -EISTKN;
}

int process_load(const char* filename, struct process** process)
{
    int res = 0;
    int process_slot = process_get_free_slot();// find the process slot to load
    if (process_slot < 0)
    {
        res = -EISTKN;
        goto out;
    }

    res = process_load_for_slot(filename, process, process_slot);
out:
    return res;
}

// the address pointer to process, 
int process_load_for_slot(const char* filename, struct process** process, int process_slot)
{
    int res = 0;
    struct task* task = 0;
    struct process* _process;
    void* program_stack_ptr = 0;

    if (process_get(process_slot) != 0)
    {
        res = -EISTKN; // check, cause can not load a process has already in slot
        goto out;
    }

    _process = kzalloc(sizeof(struct process));
    if (!_process)
    {
        res = -ENOMEM; // done here for running out of memory
        goto out;
    }

    process_init(_process);
    res = process_load_data(filename, _process);
    if (res < 0)
    {
        goto out;
    }

    // create a stack memory
    program_stack_ptr = kzalloc(MULTITASK_OS_KERNELSHELL_USER_PROGRAM_STACK_SIZE); // 1024*16
    if (!program_stack_ptr)
    {
        res = -ENOMEM; // fail to create a stack memory
        goto out;
    }

    strncpy(_process->filename, filename, sizeof(_process->filename));
    _process->stack = program_stack_ptr;
    _process->id = process_slot;

    // Create a task, here is only 1 task
    task = task_new(_process);
    if (ERROR_I(task) == 0)
    {
        res = ERROR_I(task);
        goto out; // if the task is not created, return the error
    }

    // now we have the task ready to be run

    _process->task = task;

    res = process_map_memory(_process); // paging model to map the memory of the process
    if (res < 0)
    {
        goto out;
    }

    // now we have map the memory

    *process = _process;

    // Add the process to the array
    processes[process_slot] = _process;

out:
    if (ISERR(res)) // if there is an error in the result
    {
        if (_process && _process->task)
        {
            task_free(_process->task);
        }

       // Free the process data
    }
    return res;
}