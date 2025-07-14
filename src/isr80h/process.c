#include "process.h"
#include "task/task.h"
#include "task/process.h"
#include "string/string.h"
#include "status.h"
#include "config.h"


void* isr80h_command6_process_load_start(struct interrupt_frame* frame)
{
    void* filename_user_ptr = task_get_stack_item(task_current(), 0);
    char filename[MULTITASK_OS_KERNELSHELL_MAX_PATH];
    int res = copy_string_from_task(task_current(), filename_user_ptr, filename, sizeof(filename));
    if (res < 0)
    {
        goto out;// fail to load
    }

    char path[MULTITASK_OS_KERNELSHELL_MAX_PATH];
    strcpy(path, "0:/");
    strcpy(path+3, filename);// we have 3 bytes here.

    struct process* process = 0;
    res = process_load_switch(path, &process);
    if (res < 0)
    {
        goto out;
    }

    task_switch(process->task);// fucntion in task.c, drop to lower level in after line
    task_return(&process->task->registers);// back to task.asm

out:
    return 0;
}