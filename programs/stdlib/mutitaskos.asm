[BITS 32]

section .asm


global print:function
global multitaskos_getkey:function
global multitaskos_malloc:function
global multitaskos_free:function
global multitaskos_putchar:function
global multitaskos_process_load_start:function

; void print(const char* filename)
print:
    push ebp
    mov ebp, esp
    push dword[ebp+8] ; push ebp is 4 bytes, use the stack use another 4 bytes
    mov eax, 1 ; Command print
    int 0x80
    add esp, 4 ; Clean up the stack
    pop ebp
    ret

; int getkey()
multitaskos_getkey:
    push ebp
    mov ebp, esp
    mov eax, 2 ; Command getkey
    int 0x80 ; invoke the kernel
    ;mov esp, ebp ; Clean up the stack
    pop ebp
    ret

; void multitaskos_putchar(char c)
multitaskos_putchar:
    push ebp
    mov ebp, esp
    mov eax, 3 ; Command putchar
    push dword [ebp+8] ; Variable "c"
    int 0x80
    add esp, 4
    pop ebp
    ret

; void* multitaskos_malloc(size_t size)
multitaskos_malloc:
    push ebp
    mov ebp, esp
    mov eax, 4 ; Command malloc (Allocates memory for the process)
    push dword[ebp+8] ; Variable "size"
    int 0x80
    add esp, 4
    pop ebp
    ret

; void multitaskos_free(void* ptr)
multitaskos_free:
    push ebp
    mov ebp, esp
    mov eax, 5 ; Command 5 free (Frees the allocated memory for this process)
    push dword[ebp+8] ; Variable "ptr"
    int 0x80
    add esp, 4
    pop ebp
    ret

; void multitaskos_process_load_start(const char* filename)
multitaskos_process_load_start:
    push ebp
    mov ebp, esp
    mov eax, 6 ; Command 6 process load start ( stars a process )
    push dword[ebp+8] ; Variable "filename"
    int 0x80
    add esp, 4
    pop ebp
    ret