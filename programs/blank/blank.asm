[BITS 32]

section .asm

global _start

_start:

    push 20
    push 30
    mov eax, 0 ; eax is used for command, comand 0 sum
    int 0x80
    add esp, 8 ; clean up the stack (2 arguments), each argument(push) is 4 bytes
    
    jmp $