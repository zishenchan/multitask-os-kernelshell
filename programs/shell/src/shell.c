#include "shell.h"
#include "stdio.h"
#include "stdlib.h"
#include "multitaskos.h"
int main(int argc, char** argv)
{
    print("MultitaskOS v1.0.0\n");
    while(1) 
    {
        print("> ");
        char buf[1024];
        multitaskos_terminal_readline(buf, sizeof(buf), true);

        /**
         * Test
         */
        multitaskos_process_load_start(buf);
        print("\n");
    }
    return 0;
}