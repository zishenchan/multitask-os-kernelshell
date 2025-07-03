section .asm

global tss_load

; function to load in the TSS segment
tss_load:
    push ebp
    mov ebp, esp
    mov ax, [ebp+8] ; TSS Segment provided to us
    ltr ax
    pop ebp
    ret