[BITS 32]

section .asm

global _start

_start:

_loop:
    call getkey ; wait for key to be presse
    push eax
    move eax, 3

    ;push message
    ;mov eax, 1
    
    int 0x80 ; invoke kernel 
    add esp, 4 ; only push one item to stack 

    jmp $

getkey:
    mov eax, 2 ; Command getkey, this go over and over again until key is returned. 
    int 0x80
    cmp eax, 0x00
    je getkey
    ret

section .data
message: db 'Hello, World!', 0; Newline and null terminator