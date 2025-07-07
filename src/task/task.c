#include "task.h"
#include "kernel.h"
#include "status.h"
#include "process.h"
#include "memory/heap/kheap.h"
#include "memory/memory.h"

// The current task that is running
struct task* current_task = 0;

// Task linked list, means task has pointer to another task
struct task* task_tail = 0;
struct task* task_head = 0;

int task_init(struct task* task, struct process* process);


struct task* task_current()
{
    return current_task;
}

struct task* task_new(struct process* process)
{
    int res = 0;
    struct task* task = kzalloc(sizeof(struct task));
    if (!task)
    {
        res = -ENOMEM;
        goto out;
    }

    res = task_init(task, process); // passig task just created abrove
    if (res != MULTITASK_OS_KERNELSHELL_ALL_OK)
    {
        goto out;
    }

    if (task_head == 0)
    {
        task_head = task;
        task_tail = task;
        goto out;
    }

    task_tail->next = task; // settting the pervious task's next to the new task
    task->prev = task_tail;
    task_tail = task;

out:    
    if (ISERR(res))
    {
        task_free(task);
        return ERROR(res);
    }

    return task;
}

struct task* task_get_next()
{
    if (!current_task->next)
    {
        return task_head;
    }

    return current_task->next;
}

// user only access to this function by task_free
static void task_list_remove(struct task* task)
{
    if (task->prev) // check if pervious pointer
    {
        task->prev->next = task->next;
    }

    if (task == task_head)
    {
        task_head = task->next; // replace the task head
    }

    if (task == task_tail)
    {
        task_tail = task->prev; // new task tail becomes the previous task
    }

    if (task == current_task)
    {
        current_task = task_get_next();
    }
}

int task_free(struct task* task)
{
    paging_free_4gb(task->page_directory);
    task_list_remove(task);

    // Finally free the task data
    kfree(task);
    return 0;
}

// this funciton change the current task, and switch the paging directory
int task_switch(struct task* task)
{
    current_task = task;
    paging_switch(task->page_directory->directory_entry);
    return 0;
}

// this function takes out of kernel page directory, and loads into the user page directory
int task_page()
{
    user_registers();
    task_switch(current_task);
    return 0;
}

void task_run_first_ever_task()
{
    if (!current_task)
    {
        panic("task_run_first_ever_task(): No current task exists!\n");
    }

    task_switch(task_head);
    task_return(&task_head->registers);
}

int task_init(struct task* task, struct process* process)
{
    memset(task, 0, sizeof(struct task));
    // Map the entire 4GB address space to its self
    task->page_directory = paging_new_4gb(PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    if (!task->page_directory)
    {
        return -EIO;
    }

    task->registers.ip = MULTITASK_OS_KERNELSHELL_PROGRAM_VIRTUAL_ADDRESS; // ip last executed
    task->registers.ss = USER_DATA_SEGMENT;
    task->registers.esp = MULTITASK_OS_KERNELSHELL_PROGRAM_VIRTUAL_STACK_ADDRESS_START;

    task->process = process; // set the process that the task belongs to

    return 0;
}