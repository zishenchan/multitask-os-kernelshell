section .asm
global gdt_load

; just making the C version of boot.asm, gdt structure
gdt_load:
    mov eax, [esp+4]
    mov [gdt_descriptor + 2], eax
    mov ax, [esp+8]
    mov [gdt_descriptor], ax
    lgdt [gdt_descriptor] ; call the load function in processor
    ret


section .data
gdt_descriptor:
    dw 0x00 ; Size
    dd 0x00 ; GDT Start Address