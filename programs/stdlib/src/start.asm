[BITS 32]

global _start
extern c_start
extern multitaskos_exit

section .asm

_start:
    call c_start
    call multitaskos_exit
    ret