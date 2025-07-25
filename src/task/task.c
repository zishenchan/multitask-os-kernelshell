#include "task.h"
#include "kernel.h"
#include "status.h"
#include "process.h"
#include "memory/heap/kheap.h"
#include "memory/memory.h"
#include "idt/idt.h"
#include "memory/paging/paging.h"
#include "string/string.h"
#include "loader/format/elfloader.h"

// The current task that is running
struct task *current_task = 0;

// Task linked list, means task has pointer to another task
struct task *task_tail = 0;
struct task *task_head = 0;

int task_init(struct task *task, struct process *process);


struct task *task_current()
{
    return current_task;
}

struct task *task_new(struct process *process)
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
        current_task = task; // set the current task to the new task
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

struct task *task_get_next()
{
    if (!current_task->next)
    {
        return task_head;
    }

    return current_task->next;
}

// user only access to this function by task_free
static void task_list_remove(struct task *task)
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

int task_free(struct task *task)
{
    paging_free_4gb(task->page_directory);
    task_list_remove(task);

    // Finally free the task data
    kfree(task);
    return 0;
}

// this funciton change the current task, and switch the paging directory
int task_switch(struct task *task)
{
    current_task = task;
    paging_switch(task->page_directory);
    return 0;
}

// we are stating the state of the task, we can resume the task later / save it and come back
void task_save_state(struct task *task, struct interrupt_frame *frame)
{
    task->registers.ip = frame->ip;
    task->registers.cs = frame->cs;
    task->registers.flags = frame->flags;
    task->registers.esp = frame->esp;
    task->registers.ss = frame->ss;
    task->registers.eax = frame->eax;
    task->registers.ebp = frame->ebp;
    task->registers.ebx = frame->ebx;
    task->registers.ecx = frame->ecx;
    task->registers.edi = frame->edi;
    task->registers.edx = frame->edx;
    task->registers.esi = frame->esi;
}

/**
 * needs to be in kernel page first, and the virtual can not access from kernel land.
 * So in paging_map() we map the virtual adress, tmp, to physical address, tmp (second tmp).
 * 
 */
int copy_string_from_task(struct task* task, void* virtual, void* phys, int max)
{
    if (max >= PAGING_PAGE_SIZE)
    {
        return -EINVARG;
    }

    int res = 0;
    char* tmp = kzalloc(max); // allocate memory to copy string, both map same address tmp
    if (!tmp)
    {
        res = -ENOMEM;
        goto out;
    }

    uint32_t* task_directory = task->page_directory->directory_entry;
    uint32_t old_entry = paging_get(task_directory, tmp);
    paging_map(task->page_directory, tmp, tmp, PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    paging_switch(task->page_directory);
    strncpy(tmp, virtual, max); // safely copy virtual string straight to tep, both points to same place
    kernel_page();

    res = paging_set(task_directory, tmp, old_entry);// back to kernel page
    if (res < 0)
    {
        res = -EIO;
        goto out_free;
    }

    strncpy(phys, tmp, max);

out_free:
    kfree(tmp);
out:
    return res;
}

// this function is needed to be called in kernel land, page need to be set in kernel land
void task_current_save_state(struct interrupt_frame *frame)
{
    if (!task_current())
    {
        panic("No current task to save\n");
    }

    struct task *task = task_current();
    task_save_state(task, frame);
}

// this function takes out of kernel page directory, and loads into the user page directory
int task_page()
{
    user_registers();
    task_switch(current_task);
    return 0;
}

int task_page_task(struct task* task)
{
    user_registers();
    paging_switch(task->page_directory);
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

int task_init(struct task *task, struct process *process)
{
    memset(task, 0, sizeof(struct task));
    // Map the entire 4GB address space to its self
    task->page_directory = paging_new_4gb(PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    if (!task->page_directory)
    {
        return -EIO;
    }
    /**
     * It's cleaner to set the IP at front, and change it later
     */
    task->registers.ip = MULTITASK_OS_KERNELSHELL_PROGRAM_VIRTUAL_ADDRESS; // ip last executed

    if (process->filetype == PROCESS_FILETYPE_ELF)
    {
        task->registers.ip = elf_header(process->elf_file)->e_entry;
    }

    task->registers.ss = USER_DATA_SEGMENT;
    task->registers.cs = USER_CODE_SEGMENT; // code segment
    task->registers.esp = MULTITASK_OS_KERNELSHELL_PROGRAM_VIRTUAL_STACK_ADDRESS_START;

    task->process = process; // set the process that the task belongs to

    return 0;
}

/**
 * pull the stack item from the task stack
 */
void* task_get_stack_item(struct task* task, int index)
{
    void* result = 0;

    uint32_t* sp_ptr = (uint32_t*) task->registers.esp;// get the stack pointer

    // Switch to the given tasks page
    task_page_task(task);

    result = (void*) sp_ptr[index];// you add stack, it's downwards, pull is upwards

    // Switch back to the kernel page
    kernel_page();

    return result;
}