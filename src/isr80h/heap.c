#include "heap.h"
#include "task/task.h"
#include "task/process.h"
#include <stddef.h>
void* isr80h_command4_malloc(struct interrupt_frame* frame)
{
    /**
     * This function is called when the user space program calls malloc.
     */
    size_t size = (int)task_get_stack_item(task_current(), 0);
    return process_malloc(task_current()->process, size);
}

void* isr80h_command5_free(struct interrupt_frame* frame)
{
    void* ptr_to_free = task_get_stack_item(task_current(), 0);// accept the pointer to free
    process_free(task_current()->process, ptr_to_free);
    return 0;
}