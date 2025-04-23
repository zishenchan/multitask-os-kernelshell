section .asm

global insb
global insws
global outb
global outw

insb:
    push ebp
    mov ebp, esp

    xor eax, eax ; clear eax register
    mov edx , [ebp + 8] ; put port into edx register
    in al, dx ; read from port into al register

    pop ebp
    ret

insw:
    push ebp
    mov ebp, esp

    xor eax, eax ; clear eax register
    mov edx , [ebp + 8] ; put port into edx register
    in ax, dx ; read from port into ax register

    pop ebp
    ret

outb:
    push ebp
    mov ebp, esp

    mov eax, [ebp+12] ; Load the value to be written into eax
    mov edx , [ebp + 8] ; put port into edx register
    out dx, al ; write to port from al register

    pop ebp
    ret

outw:
    push ebp
    mov ebp, esp

    mov eax, [ebp+12] ; Load the value to be written into eax
    mov edx , [ebp + 8] ; put port into edx register
    out dx, ax ; write to port from ax register

    pop ebp
    ret