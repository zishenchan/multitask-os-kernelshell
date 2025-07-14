#include "multitaskos.h"

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