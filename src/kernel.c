#include "kernel.h"
#include <stdint.h>
#include <stddef.h>
#include "idt/idt.h"
#include "io/io.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "disk/disk.h"

uint16_t* video_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;

uint16_t terminal_make_char(char c, char color)
{
    return (color << 8) | c;
}

void terminal_putchar(int x, int y, char c, char color)
{
    video_mem[y * VGA_WIDTH + x] = terminal_make_char(c, color);
}

void terminal_writechar(char c, char color)
{
    if (c == '\n')
    {
        terminal_col = 0;
        terminal_row++;
        return;
    }

    terminal_putchar(terminal_col, terminal_row, c, color);
    terminal_col++;
    if (terminal_col >= VGA_WIDTH)
    {
        terminal_col = 0;
        terminal_row++;
    }
}

void terminal_initialize()
{
    video_mem = (uint16_t*)(0xB8000);

    terminal_row = 0;
    terminal_col = 0;  

    for (int y = 0; y < VGA_HEIGHT; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            terminal_putchar(x, y, ' ', 0);
        }
    }
}

size_t strlen(const char* str)
{
    size_t len = 0;
    while (str[len] != '\0')
    {
        len++;
    }
    return len;
}

void print(const char* str)
{
    size_t len = strlen(str);
    for (int i = 0; i < len; i++)
    {
        terminal_writechar(str[i], 15);
    }
}

static struct paging_4gb_chunk* kernel_chunk = 0;

void kernel_main()
{   
    terminal_initialize();
    print("Hello, World!\n");
    print("This is a simple kernel.\n");
    print("Welcome to the kernel development world.\n");
    print("Let's write some text to the screen.\n");

    kheap_init(); // Initialize the kernel heap
    
    // Initialize the disk
    disk_search_and_init();

    idt_init();// Initialize the IDT
    //problem(); // Call the problem function to trigger an interrupt

    // setup paging
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    
    // switch to the new page directory
    paging_switch(paging_4gb_chunk_get_directory(kernel_chunk));

    // enable paging
    enable_paging(); // Enable paging

   
    /*
     char* ptr2 = (char*)0x1000; // Get the virtual address
    ptr2[0] = 'A'; // Write to the allocated memory
    ptr2[1] = 'B'; 
    print(ptr2); // Print the contents of the allocated memory  
    print(ptr); 
    both pointer ptr points to 0x1000000, page talbe level,
    ptr2 points to 0x1000, page table level,0x1000 -> 0x1000000,
    they both point to the same physical address, in the page table
    char buf[512];
    disk_read_sector(0, 1, buf); // Read a sector from the disk
    */
    

    enable_interrupts(); // make use below enable paging
}