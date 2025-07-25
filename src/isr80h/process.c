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

void* isr80h_command7_invoke_system_command(struct interrupt_frame* frame)
{
    struct command_argument* arguments = task_virtual_address_to_physical(task_current(), task_get_stack_item(task_current(), 0));
    if (!arguments || strlen(arguments[0].argument) == 0)// does it pass us null, or zero in length?
    {
        return ERROR(-EINVARG);
    }

    struct command_argument* root_command_argument = &arguments[0];// like blank.elf arg1 arg2
    const char* program_name = root_command_argument->argument;

    char path[MULTITASK_OS_KERNELSHELL_MAX_PATH];
    strcpy(path, "0:/");// the drive number 
    strncpy(path+3, program_name, sizeof(path));// 3 bytes already existed in buffer
    
    struct process* process = 0;
    int res = process_load_switch(path, &process);
    if (res < 0)
    {
        return ERROR(res);
    }

    res = process_inject_arguments(process, root_command_argument);
    if (res < 0)
    {
        return ERROR(res);
    }

    task_switch(process->task);
    task_return(&process->task->registers);// start use the user land again
    return 0;
}

void* isr80h_command8_get_program_arguments(struct interrupt_frame* frame)
{
    struct process* process = task_current()->process;
    struct process_arguments* arguments = task_virtual_address_to_physical(task_current(), task_get_stack_item(task_current(), 0));
    // After this function called, it returns physical address, and can access them directly.

    process_get_arguments(process, &arguments->argc, &arguments->argv);
    return 0;
}

void* isr80h_command9_exit(struct interrupt_frame* frame)
{
    struct process* process = task_current()->process;
    process_terminate(process);
    task_next();
    return 0;
}