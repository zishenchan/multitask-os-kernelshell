[BITS 32]

section .asm

global paging_load_directory
global enable_paging

paging_load_directory:
    push ebp
    mov ebp, esp
    mov eax, [ebp + 8] ; Load the address of the page directory
    mov cr3, eax       ; Load the page directory into CR3
    pop ebp
    ret

enable_paging:
    push ebp
    mov ebp, esp
    mov eax, cr0       ; Read CR0
    or eax, 0x80000000 ; Set the PG bit (bit 31)
    mov cr0, eax       ; Write back to CR0
    pop ebp
    ret