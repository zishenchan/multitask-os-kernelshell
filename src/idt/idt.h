#ifndef IDT_H
#define IDT_H
#include <stdint.h>

struct interrupt_frame;
typedef void*(*ISR80H_COMMAND)(struct interrupt_frame* frame); // the ISR80H is a pointer

struct idt_desc
{
    uint16_t offset_1; // lower 16 bits of offset
    uint16_t selector; // a code segment selector in GDT or LDT
    uint8_t zero;     // unused, set to 0
    uint8_t type_attr; // type and attributes
    uint16_t offset_2; // upper 16 bits of offset
}__attribute__((packed));

struct idtr_desc
{
    uint16_t limit; // size of the IDT in bytes
    uint32_t base;  // address of the first entry in the IDT
}__attribute__((packed));

struct interrupt_frame
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t reserved;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t esp;
    uint32_t ss;
} __attribute__((packed)); // The PUSHAD description on google 

void idt_init();
void enable_interrupts();
void disable_interrupts();
void isr80h_register_command(int command_id, ISR80H_COMMAND command);

#endif