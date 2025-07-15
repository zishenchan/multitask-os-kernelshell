#ifndef MULTITASKOS_H
#define MULTITASKOS_H
#include <stddef.h>
#include <stdbool.h>

struct command_argument
{
    char argument[512];
    struct command_argument* next;// linked list, each argument 512 bytes.
};

struct process_arguments
{
    int argc; // argument count
    char** argv; // argument vector, array of pointers to arguments
};


void print(const char* filename);
int multitaskos_getkey();

void* multitaskos_malloc(size_t size);
void multitaskos_free(void* ptr);
void multitaskos_putchar(char c);
void multitaskos_terminal_readline(char* out, int max, bool output_while_typing);
void multitaskos_process_load_start(const char* filename);

struct command_argument* multitaskos_oarse_arguments(const char* command, int max);
void multitaskos_process_get_arguments(struct process_arguments* arguments);
int multitaskos_system(struct command_argument* arguments);

int multitaskos_system_run(const char* command);
void multitaskos_exit();

#endif