#include "idt.h"
#include "config.h"
#include "kernel.h"
#include "memory/memory.h"
#include "io/io.h"

struct idt_desc idt_descriptors[MULTITASK_OS_KERNELSHELL_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

extern void idt_load(struct idtr_desc* ptr);
extern void int21h(); // Example: This should be defined in your assembly code
extern void no_interrupt(); // This should be defined in your assembly code

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

    // load the interrupt descriptor table
    idt_load(&idtr_descriptor);// passing the address of the IDTR
}