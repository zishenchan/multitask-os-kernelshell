section .asm

extern int21h_handler
extern no_interrupt_handler

global int21h
global idt_load
global no_interrupt


idt_load:
    push ebp
    mov ebp, esp

    mov ebx, [ebp + 8] ; Load the address of the IDT into ebx
    lidt [ebx] ; Load the IDT

    pop ebp
    ret

int21h:
    cli
    pushad
    
    call int21h_handler ; Call the interrupt handler
    
    popad
    sti
    iret

no_interrupt:
    cli
    pushad
    
    call no_interrupt_handler ; Call the interrupt handler
    
    popad
    sti
    iret