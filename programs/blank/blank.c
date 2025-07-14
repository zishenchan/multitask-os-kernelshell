#include "MULTITASKOS.H"
#include <stdlib.h>

int main(int argc, char** argv)
{
    printf("printf function testing\n");
    
    

    while(1) 
    {

    }
    return 0;

    /**
     * Test suite 
     * 
     * The following tests demonstrate various functionalities:
     * 
     * print("Hello print in blank.c!\n");
     *
     * Test ITOS function in stdlib.c:
     * print(itoa(8763)); // Converts integer to string
     *
     * Test putchar function:
     * putchar('A'); 
     *
     * Test memory allocation and deallocation:
     * void* ptr = malloc(512);
     * free(ptr); // Free the allocated memory
     *
     * Test multitaskos_getkeyblock function:
     * multitaskos_getkeyblock(); // Wait for a key press
     *
     * Test multitaskos_terminal_readline function:
     * char buf[256];
     * multitaskos_terminal_readline(buf, sizeof(buf), true);
     * print(buf); // Print the buffer content
     */
}