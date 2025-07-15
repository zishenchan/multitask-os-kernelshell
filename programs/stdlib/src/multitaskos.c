#include "multitaskos.h"
#include "string.h"

struct command_argument* peachos_parse_command(const char* command, int max)
{
    struct command_argument* root_command = 0;
    char scommand[1025];
    if (max >= (int) sizeof(scommand))
    {
        return 0;
    }

    strncpy(scommand, command, sizeof(scommand));//Copy the string to command.
    char* token = strtok(scommand, " ");// find the first token 
    if (!token)
    {
        goto out;
    }

    root_command = multitaskos_malloc(sizeof(struct command_argument));
    if (!root_command)
    {
        goto out;// root command failed
    }

    strncpy(root_command->argument, token, sizeof(root_command->argument));
    root_command->next = 0;// paste the root command to the linked list


    struct command_argument* current = root_command;
    token = strtok(NULL, " ");
    while(token != 0)
    {
        struct command_argument* new_command = multitaskos_malloc(sizeof(struct command_argument));
        if (!new_command)
        {
            break;
        }

        strncpy(new_command->argument, token, sizeof(new_command->argument));// copy the token into new command argument
        
        new_command->next = 0x00;// add to linked list
        current->next = new_command;
        current = new_command;
        token = strtok(NULL, " ");// get the next token to loop back
    }
out:
    return root_command; // return the root command. 
}

int pmultitaskos_getkeyblock()
{
    int val = 0;
    do
    {
        val = multitaskos_getkey();
    }
    while(val == 0);// will loop until key is pressed
    return val;
}

/**
 * Wait until key is pressed and fill the buffer.
 */
void multitaskos_terminal_readline(char* out, int max, bool output_while_typing)
{
    int i = 0;
    for (i = 0; i < max -1; i++)
    {
        char key = multitaskos_getkeyblock();

        // Carriage return means we have read the line
        if (key == 13)
        {
            break;
        }

        if (output_while_typing)
        {
            multitaskos_putchar(key);
        }

        // Backspace
        if (key == 0x08 && i >= 1)
        {
            out[i-1] = 0x00;
            // -2 because we will +1 when we go to the continue
            i -= 2;
            continue;
        }

        out[i] = key;
    }

    // Add the null terminator
    out[i] = 0x00;
}

int multitaskos_system_run(const char* command)
{
    char buf[1024];
    strncpy(buf, command, sizeof(buf));
    struct command_argument* root_command_argument = multitaskos_parse_command(buf, sizeof(buf));
    if (!root_command_argument)
    {
        return -1;
    }

    return multitaskos_system(root_command_argument);
}