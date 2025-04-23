#include "kheap.h"
#include "heap.h"
#include "config.h"
#include "kernel.h"

struct heap kernel_heap;
struct heap_table kernel_heap_table;

void kheap_init()
{
    int total_heap_entries = MULTITASK_OS_KERNELSHELL_HEAP_SIZE_BYTES / MULTITASK_OS_KERNELSHELL_HEAP_BLOCK_SIZE; // 100MB / 4KB
    kernel_heap_table.entries = (HEAP_BLOCK_TABLE_ENTRY*) (MULTITASK_OS_KERNELSHELL_HEAP_TABLE_ADDRESS); // 2KB
    kernel_heap_table.total = total_heap_entries;

    void* end = (void*) (MULTITASK_OS_KERNELSHELL_HEAP_ADDRESS + MULTITASK_OS_KERNELSHELL_HEAP_SIZE_BYTES); // 100MB
    int res = heap_create(&kernel_heap, (void*) (MULTITASK_OS_KERNELSHELL_HEAP_ADDRESS), end, &kernel_heap_table);
    if (res < 0)
    {
        print("Kernel heap creation failed\n");
    }
    
}

void* kmalloc(size_t size)
{
    return heap_malloc(&kernel_heap, size);
}

void kfree(void* ptr)
{
    heap_free(&kernel_heap, ptr);
}

    
