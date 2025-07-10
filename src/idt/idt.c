#include "idt.h"
#include "config.h"
#include "kernel.h"
#include "memory/memory.h"
#include "io/io.h"
#include "task/task.h"

struct idt_desc idt_descriptors[MULTITASK_OS_KERNELSHELL_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

static ISR80H_COMMAND isr80h_commands[MULTITASK_OS_KERNELSHELL_MAX_ISR80H_COMMANDS];

extern void idt_load(struct idtr_desc* ptr);
extern void int21h(); // Example: This should be defined in your assembly code
extern void no_interrupt(); // This should be defined in your assembly code
extern void isr80h_wapper(); // This should be defined in your assembly code

void int21h_handler()
{
    // This is a dummy function to be used as a placeholder for interrupt 21h
    print("keyboardPress\n");
    print("Interrupt 21h triggered!\n");
    outb(0x20, 0x20); // Send EOI (End of Interrupt) to PIC
}

void no_interrupt_handler()
{
    outb(0x20, 0x20); // Send EOI (End of Interrupt) to PIC
}


void idt_zero()
{
    // This is a dummy function to be used as a placeholder for interrupt 0
    print("Divide by zero error!\n");
}

void idt_set(int interrupt_no, void* address)
{
    struct idt_desc* desc = &idt_descriptors[interrupt_no];
    desc->offset_1 = (uint32_t)address & 0x0000ffff;
    desc->selector = KERNEL_CODE_SELECTOR; // Kernel code segment
    desc->zero = 0x00;
    desc->type_attr = 0xEE; // Present, DPL=0, 32-bit interrupt gate
    desc->offset_2 = (uint32_t) address >> 16; // upper 16 bits of offset

}
void idt_init()
{
    memset(idt_descriptors, 0, sizeof(idt_descriptors));
    idtr_descriptor.limit = sizeof(idtr_descriptor) -1;
    idtr_descriptor.base = (uint32_t) idt_descriptors;

    for (int i = 0; i < MULTITASK_OS_KERNELSHELL_TOTAL_INTERRUPTS; i++)
    {
        idt_set(i, no_interrupt); // Set all interrupts to a dummy handler
    }

    idt_set(0, idt_zero); // Example: Set interrupt 0 to a dummy address
    idt_set(0x21, int21h); // Set interrupt 21h to the keyboard handler
    idt_set(0x80, isr80h_wapper);

    // load the interrupt descriptor table
    idt_load(&idtr_descriptor);// passing the address of the IDTR
}

void isr80h_register_command(int command_id, ISR80H_COMMAND command)
{
    if (command_id < 0 || command_id >= MULTITASK_OS_KERNELSHELL_MAX_ISR80H_COMMANDS)
    {
        panic("The command is out of bounds\n");
    }

    if (isr80h_commands[command_id]) // make sure command is not already taken
    {
        panic("Your attempting to overwrite an existing command\n");
    }

    isr80h_commands[command_id] = command;
}

void* isr80h_handle_command(int command, struct interrupt_frame* frame)
{
    void* result = 0;

    if(command < 0 || command >= MULTITASK_OS_KERNELSHELL_MAX_ISR80H_COMMANDS)
    {
        // Invalid command
        return 0;
    }

    ISR80H_COMMAND command_func = isr80h_commands[command];
    if (!command_func)
    {
        return 0; // user land return 0 if invalid, just can not handle 
    }

    result = command_func(frame);
    return result;
}


void* isr80h_handler(int command, struct interrupt_frame* frame)
{
    void* res = 0;
    kernel_page(); // following four lines of functions ready to be implemented
    task_current_save_state(frame); // save the registers
    res = isr80h_handle_command(command, frame); 
    task_page();
    return res;
}