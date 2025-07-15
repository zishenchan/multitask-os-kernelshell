#include "MULTITASKOS.H"
#include <stdlib.h>
#include "string.h"
#include "stdio.h"

int main(int argc, char** argv)
{
    // Test the multitasking functionality
    while(1)
    {
        print(argv[0]); // Print the name of the program
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
     * 
    * // Test strtok function
    * char words[] = "hello how are you";
    * const char* token = strtok(words, " "); // specify pointer to string
    * while(token)
    * {
    *     printf("%s\n", token);
    *     token = strtok(NULL, " ");
    * }
    * 
    * anohter test
    printf("printf function testing\n");

    void* ptr = malloc(512);
    strcpy(ptr, "hello world");
    print(ptr);
    free(ptr);

    char buffer[512];
    buffer[0] = 'B';
    print("abc\n");
    */
}