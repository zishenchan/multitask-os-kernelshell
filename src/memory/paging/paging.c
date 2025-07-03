#include "paging.h"
#include "memory/heap/kheap.h"
#include "status.h"


void paging_load_directory(uint32_t* directory);
static uint32_t* current_directory = 0; // current page directory

struct paging_4gb_chunk* paging_new_4gb(uint8_t flags)
{
    uint32_t* directory = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
    int offset = 0;

    for (int i = 0; i<PAGING_TOTAL_ENTRIES_PER_TABLE; i++)
    {
        uint32_t* entry  = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
        
        // next align different page / segment
        for(int b = 0; b < PAGING_TOTAL_ENTRIES_PER_TABLE; b++)
        {
            entry[b] = (offset + (b * PAGING_PAGE_SIZE)) | flags; // if the offset is 2, means second virtual address 

        }
        offset += (PAGING_PAGE_SIZE * PAGING_TOTAL_ENTRIES_PER_TABLE);
        directory[i] = (uint32_t)entry | flags | PAGING_IS_WRITEABLE; // set the page table entry
    }

    struct paging_4gb_chunk* chunk_4gb = kzalloc(sizeof(struct paging_4gb_chunk));
    chunk_4gb->directory_entry = directory; // set the page directory entry
    return chunk_4gb;
}
void paging_switch(uint32_t* directory)
{
    paging_load_directory(directory);
    current_directory = directory;
}

void paging_free_4gb(struct paging_4gb_chunk* chunk)
{
    for (int i = 0; i < 1024; i++) // loop all page table, free them all
    {
        uint32_t entry = chunk->directory_entry[i];
        uint32_t* table = (uint32_t*)(entry & 0xfffff000);
        kfree(table);
    }

    kfree(chunk->directory_entry); // delete the directory entry
    kfree(chunk);
}

uint32_t* paging_4gb_chunk_get_directory(struct paging_4gb_chunk* chunk)
{
    return chunk->directory_entry;
}

bool paging_is_aligned(void* addr)
{
    return ((uint32_t)addr % PAGING_PAGE_SIZE) == 0;
}

int paging_get_indexes(void* virtual_address, uint32_t* directory_index_out, uint32_t* table_index_out)
{   
    int res = 0;
    if(!paging_is_aligned(virtual_address)) // which directory and table for this address
    {
        return -EINVARG; // not aligned
        goto out;
    }

    // calcuate the directory and table indexes
    *directory_index_out = ((uint32_t)virtual_address / (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE));
    *table_index_out = ((uint32_t)virtual_address % (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE) / PAGING_PAGE_SIZE);

out:
    return res;
}

int paging_set(uint32_t* directory, void* virt, uint32_t val)
{
    if(!paging_is_aligned(virt))
    {
        return -EINVARG; // not aligned
    }

    uint32_t directory_index = 0;
    uint32_t table_index = 0;
    int res = paging_get_indexes(virt, &directory_index, &table_index);
    if(res < 0)
    {
        return res;
    }

    uint32_t entry = directory[directory_index];
    uint32_t* table = (uint32_t*)(entry & 0xfffff000); // get the page table address, each f is 4 bytes
    table[table_index] = val; // set the page table entry

    return 0;
    
}