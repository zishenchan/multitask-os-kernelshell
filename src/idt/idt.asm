section .asm

extern int21h_handler
extern no_interrupt_handler
extern isr80h_handler
extern interrupt_handler

global idt_load
global no_interrupt
global enable_interrupts
global disable_interrupts
global isr80h_wrapper
global interrupt_pointer_table

enable_interrupts:
    sti
    ret

disable_interrupts:
    cli
    ret

idt_load:
    push ebp
    mov ebp, esp

    mov ebx, [ebp + 8] ; Load the address of the IDT into ebx
    lidt [ebx] ; Load the IDT

    pop ebp
    ret

int21h:
    pushad
    
    call int21h_handler ; Call the interrupt handler
    
    popad
    iret

no_interrupt:
    pushad
    
    call no_interrupt_handler ; Call the interrupt handler
    
    popad
    iret

%macro interrupt 1
    global int%1
    int%1:
        ; interrupt frame start 
        ; ALREADY PUSHED TO US BY THE PROCESSOR UPON ENTRY TO THIS INTERRUPT AUTOMATICALLY
        ; uint32_t ip
        ; uint32_t cs;
        ; uint32_t flags
        ; uint32_t sp;
        ; uint32_t ss;
        ; Pushes the general purpose registers to the stack
        pushad
        ; interrupt frame end
        push esp
        push dword %i
        call interrupt_handler
        add esp, 8
        popad
        iret
%endmacro

; loop for 512 times
%assign i 0
%rep 512 
    interupt i
%assign i i + 1
%endrep 

isr80h_wrapper:
    ; interrupt frame start 
    ; ALREADY PUSHED TO US BY THE PROCESSOR UPON ENTRY TO THIS INTERRUPT AUTOMATICALLY
    ; uint32_t ip
    ; uint32_t cs;
    ; uint32_t flags
    ; uint32_t sp;
    ; uint32_t ss;
    ; Pushes the general purpose registers to the stack
    pushad
    
    ; interrupt frame end

    ; Push the stack pointer so that we are pointing to the interrupt frame
    push esp

    ; EAX holds our command lets push it to the stack for isr80h_handler
    push eax
    call isr80h_handler ; only return the void pointer
    mov dword[tmp_res], eax ; eax wil have the return value from isr80h_handler
    add esp, 8 ; 32bit system, one is 4 bits, 2 for 8

    ; Restore general purpose registers for user land
    popad
    mov eax, [tmp_res]
    iretd

section .data
; Inside here is stored the return result from isr80h_handler
tmp_res: dd 0

%macro interrupt_array_entry 1
    dd int%1
%endmacro

interrupt_pointer_table: ; table in C problem is extern
%assign i 0
%rep 512
    interrupt_array_entry i
%assign i i+1
%endrep