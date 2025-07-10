#include "misc.h"
#include "idt/idt.h"

/**
 * command function as name as example
 */
void* isr80h_command0_sum(struct interrupt_frame* frame)
{
    int v2 = (int) task_get_stack_item(task_current(), 1);// we want integer, not pointer
    int v1 = (int) task_get_stack_item(task_current(), 0);
    return (void*)(v1 + v2);
}