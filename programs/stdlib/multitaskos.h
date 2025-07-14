#ifndef MULTITASKOS_H
#define MULTITASKOS_H
#include <stddef.h>
#include <stdbool.h>

void print(const char* filename);
int multitaskos_getkey();

void* multitaskos_malloc(size_t size);
void multitaskos_free(void* ptr);
void multitaskos_putchar(char c);
void multitaskos_terminal_readline(char* out, int max, bool output_while_typing);
void multitaskos_process_load_start(const char* filename);



#endif