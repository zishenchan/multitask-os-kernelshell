#include "kernel.h"
#include <stddef.h>
#include <stdint.h>
#include "idt/idt.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "string/string.h"
#include "fs/file.h"
#include "disk/disk.h"
#include "fs/pparser.h"
#include "disk/streamer.h"
#include "gdt/gdt.h"
#include "config.h"
#include "memory/memory.h"
#include "task/tss.h"
#include "task/task.h"
#include "task/process.h"
#include "status.h"

uint16_t* video_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;

uint16_t terminal_make_char(char c, char colour)
{
    return (colour << 8) | c;
}

void terminal_putchar(int x, int y, char c, char colour)
{
    video_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c, colour);
}

void terminal_writechar(char c, char colour)
{
    if (c == '\n')
    {
        terminal_row += 1;
        terminal_col = 0;
        return;
    }

    terminal_putchar(terminal_col, terminal_row, c, colour);
    terminal_col += 1;
    if (terminal_col >= VGA_WIDTH)
    {
        terminal_col = 0;
        terminal_row += 1;
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



void print(const char* str)
{
    size_t len = strlen(str);
    for (int i = 0; i < len; i++)
    {
        terminal_writechar(str[i], 15);
    }
}


static struct paging_4gb_chunk* kernel_chunk = 0;


void panic(const char* msg) // the kernel panic function
{
    print(msg);
    while(1) {}
}

struct tss tss;

// more clear version for gdt_data in boot.asm
struct gdt gdt_real[MULTITASK_OS_KERNELSHELL_TOTAL_GDT_SEGMENTS];
struct gdt_structured gdt_structured[MULTITASK_OS_KERNELSHELL_TOTAL_GDT_SEGMENTS] = {
    {.base = 0x00, .limit = 0x00, .type = 0x00},                // NULL Segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0x9a},           // Kernel code segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0x92},            // Kernel data segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0xf8},              // User code segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0xf2},             // User data segment
    {.base = (uint32_t)&tss, .limit=sizeof(tss), .type = 0xE9}      // TSS Segment
};
// the limit 0xffffffff is physical address, since we use paging memory model, the limitation will be doen by paging

void kernel_main()
{
    terminal_initialize();
    print("Hello world!\ntest");

    memset(gdt_real, 0x00, sizeof(gdt_real));
    gdt_structured_to_gdt(gdt_real, gdt_structured, MULTITASK_OS_KERNELSHELL_TOTAL_GDT_SEGMENTS);

    // Load the gdt
    gdt_load(gdt_real, sizeof(gdt_real));

    // Initialize the heap
    kheap_init();

    // Initialize filesystems
    fs_init();

    // Search and initialize the disks
    disk_search_and_init();

    // Initialize the interrupt descriptor table
    idt_init();

    // Setup the TSS
    memset(&tss, 0x00, sizeof(tss));// memset the whole tss to 0
    tss.esp0 = 0x600000; // where the kernel tack is located
    tss.ss0 = KERNEL_DATA_SELECTOR;

    // Load the TSS
    tss_load(0x28);// 28 due to the offset

    // Setup paging
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

    // Switch to kernel paging chunk
    paging_switch(kernel_chunk);

    // Enable paging
    enable_paging();

    struct process* process = 0;
    int res  = process_load("0:/blank.bin", &process);
    if (res != MULTITASK_OS_KERNELSHELL_ALL_OK)
    {
        panic("Failed to load process\n");
    }

    task_run_first_ever_task(); // Run the first task, which is the process we just loaded


    while(1) {}

}
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

/*struct path_root* root_path = pathparser_parse("0:/bin/test.exe", NULL);

if(root_path)
{
    
}*/

