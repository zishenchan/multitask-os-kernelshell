#include "process.h"
#include "config.h"
#include "status.h"
#include "task/task.h"
#include "memory/memory.h"
#include "string/string.h"
#include "fs/file.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "kernel.h"
#include "loader/format/elfloader.h"


// The current process that is running
struct process* current_process = 0;

static struct process* processes[MULTITASK_OS_KERNELSHELL_MAX_PROCESSES] = {};

static void process_init(struct process* process)
{
    memset(process, 0, sizeof(struct process)); // initialize the process being passed
}

struct process* process_current()
{
    return current_process;
}

struct process* process_get(int process_id) // the index
{
    if (process_id < 0 || process_id >= MULTITASK_OS_KERNELSHELL_MAX_PROCESSES) // id means the index
    {
        return NULL;
    }

    return processes[process_id]; // return the pointer to the process
}


int process_switch(struct process* process)
{
    current_process = process;
    return 0;
}

static int process_find_free_allocation_index(struct process* process)
{
    int res = -ENOMEM;
    for (int i = 0; i < MULTITASK_OS_KERNELSHELL_MAX_PROGRAM_ALLOCATIONS; i++)
    {
        if (process->allocations[i].ptr == 0)// change to "".ptr", it's array of allocation, not pointer.
        {
            res = i;
            break;
        }
    }

    return res;
}

void* process_malloc(struct process* process, size_t size)
{
    void* ptr = kzalloc(size);
    if (!ptr)
    {
        goto out_err;// fail to allocate memory
    }

    int index = process_find_free_allocation_index(process);
    if (index < 0)
    {
        goto out_err;
    }

    /**
     * Two ptr, map the memory from virtual address to physical address.
     * Process only have one task
     */
    int res = paging_map_to(process->task->page_directory, ptr, ptr, paging_align_address(ptr+size), PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    if (res < 0)
    {
        goto out_err;
    }

    process->allocations[index].ptr = ptr;// there is error
    process->allocations[index].size = size; // the size of the allocation
    return ptr;

out_err: 
    if(ptr)
    {
        kfree(ptr);
    }
    return 0;
}


static bool process_is_process_pointer(struct process* process, void* ptr)
{
    for (int i = 0; i < MULTITASK_OS_KERNELSHELL_MAX_PROGRAM_ALLOCATIONS; i++)
    {
        if (process->allocations[i].ptr == ptr)
            return true;
    }

    return false;
}

static void process_allocation_unjoin(struct process* process, void* ptr)
{
    for (int i = 0; i < MULTITASK_OS_KERNELSHELL_MAX_PROGRAM_ALLOCATIONS; i++)
    {
        if (process->allocations[i].ptr == ptr)
        {
            process->allocations[i].ptr = 0x00;
            process->allocations[i].size = 0;
        }
    }
}

static struct process_allocation* process_get_allocation_by_addr(struct process* process, void* addr)
{
    for (int i = 0; i < MULTITASK_OS_KERNELSHELL_MAX_PROGRAM_ALLOCATIONS; i++)
    {
        if (process->allocations[i].ptr == addr)// if the allocation equals to address
            return &process->allocations[i];
    }

    return 0;
}

int process_terminate_allocations(struct process* process)
{
    for (int i = 0; i < MULTITASK_OS_KERNELSHELL_MAX_PROGRAM_ALLOCATIONS; i++)
    {
        process_free(process, process->allocations[i].ptr);
    }

    return 0;
}

int process_free_binary_data(struct process* process)
{
    kfree(process->ptr);
    return 0;
}

int process_free_elf_data(struct process* process)
{
    elf_close(process->elf_file);
    return 0;
}
int process_free_program_data(struct process* process)
{
    int res = 0;
    switch(process->filetype)
    {
        case PROCESS_FILETYPE_BINARY:
            res = process_free_binary_data(process);
        break;

        case PROCESS_FILETYPE_ELF:
            res = process_free_elf_data(process);
        break;

        default:
            res = -EINVARG;
    }
    return res;
}

void process_switch_to_any()
{
    for (int i = 0; i < MULTITASK_OS_KERNELSHELL_MAX_PROCESSES; i++)
    {
        if (processes[i])
        {
            process_switch(processes[i]);
            return;
        }
    }

    /**
     * Can choose to process another process again, or another shell
     */
    panic("No processes to switch too\n");
}

static void process_unlink(struct process* process)
{
    processes[process->id] = 0x00;// remove the process from the array

    if (current_process == process)
    {
        process_switch_to_any();
    }
}

int process_terminate(struct process* process)
{
    int res = 0;

    res = process_terminate_allocations(process);// kernel is responsible for cleaning up
    if (res < 0)
    {
        goto out;
    }

    res = process_free_program_data(process);
    if (res < 0)
    {
        goto out;
    }

    // Free the process stack memory.
    kfree(process->stack);
    // Free the task
    task_free(process->task);
    // Unlink the process from the process array.
    process_unlink(process);

    

out:
    return res;
}

void process_get_arguments(struct process* process, int* argc, char*** argv)
{
    *argc = process->arguments.argc;
    *argv = process->arguments.argv;
}

int process_count_command_arguments(struct command_argument* root_argument)
{
    struct command_argument* current = root_argument;
    int i = 0;
    while(current)
    {
        i++;
        current = current->next;
    }

    return i;
}

/**
 * This function will take any command argument we pass, store in process arguments for us,
 * readly for new command to use, to extract other process arguments. 
 */
int process_inject_arguments(struct process* process, struct command_argument* root_argument)
{
    int res = 0;
    struct command_argument* current = root_argument;
    int i = 0;
    int argc = process_count_command_arguments(root_argument);
    if (argc == 0)
    {
        res = -EIO;
        goto out;
    }

    char **argv = process_malloc(process, sizeof(const char*) * argc);// genertee to access the memory
    if (!argv)
    {
        res = -ENOMEM;// fail to declear the memory
        goto out;
    }


    while(current)
    {
        char* argument_str = process_malloc(process, sizeof(current->argument));
        if (!argument_str)
        {
            res = -ENOMEM;
            goto out;
        }
        /**
         * Possible store more memory than we expect 
         */
        strncpy(argument_str, current->argument, sizeof(current->argument));
        argv[i] = argument_str;
        current = current->next;
        i++;
    }

    process->arguments.argc = argc;
    process->arguments.argv = argv;
out:
    return res;
}

void process_free(struct process* process, void* ptr)
{
    
    if (!process_is_process_pointer(process, ptr))// Not this processes pointer? Then we cant free it.
    {
        return;
    }
    /**
     * Unlinked the page from the process.
     */
    struct process_allocation* allocation = process_get_allocation_by_addr(process, ptr);
    // Unjoin the allocation
    process_allocation_unjoin(process, ptr);
    if (!allocation)
    {
        // Oops its not our pointer.
        return;
    }

    /**
     * After allocation, upmap it.
     * Set the flag to 0x00.
     */
    int res = paging_map_to(process->task->page_directory, allocation->ptr, allocation->ptr, paging_align_address(allocation->ptr+allocation->size), 0x00);
    if (res < 0)
    {
        return;
    }
    // We can now free the memory.
    kfree(ptr);
}


static int process_load_binary(const char* filename, struct process* process)
{
    int res = 0;
    int fd = fopen(filename, "r");
    if (!fd)
    {
        res = -EIO; // if the file does not exist
        goto out;
    }

    struct file_stat stat;
    res = fstat(fd, &stat);
    if (res != MULTITASK_OS_KERNELSHELL_ALL_OK)
    {
        goto out;
    }

    void* program_data_ptr = kzalloc(stat.filesize); // need to know the size of file
    if (!program_data_ptr)
    {
        res = -ENOMEM;
        goto out;
    }

    if (fread(program_data_ptr, stat.filesize, 1, fd) != 1) // read the memory we just created
    {
        res = -EIO;
        goto out;
    }

    process->ptr = program_data_ptr;
    process->size = stat.filesize;

out:
    fclose(fd);
    return res;
}

static int process_load_elf(const char* filename, struct process* process)
{
    int res = 0;
    struct elf_file* elf_file = 0;
    res = elf_load(filename, &elf_file);// load the file, validate pointer later
    if (ISERR(res))
    {
        goto out;
    }

    process->filetype = PROCESS_FILETYPE_ELF;
    process->elf_file = elf_file;
out:
    return res;
}

static int process_load_data(const char* filename, struct process* process)
{
    int res = 0;
    //res = process_load_binary(filename, process);
    res = process_load_elf(filename, process);
    if (res == -EINFORMAT)// file we can't deal with
    {
        res = process_load_binary(filename, process);
    }
    return res;
}


int process_map_binary(struct process* process)
{
    int res = 0;
    paging_map_to(process->task->page_directory, (void*) MULTITASK_OS_KERNELSHELL_PROGRAM_VIRTUAL_ADDRESS, process->ptr, paging_align_address(process->ptr + process->size), PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE);
    return res;
    /**
     * we are mapping to the directory, 
     * in c, we need to map the memory 
     */
}

static int process_map_elf(struct process* process)
{
    int res = 0;

    struct elf_file* elf_file = process->elf_file;

    struct elf_header* header = elf_header(elf_file);
    struct elf32_phdr* phdrs = elf_pheader(header);
    for (int i = 0; i < header->e_phnum; i++)
    {
        struct elf32_phdr* phdr = &phdrs[i];
        void* phdr_phys_address = elf_phdr_phys_address(elf_file, phdr);// calcuate physical address
        int flags = PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL;
        if (phdr->p_flags & PF_W)
        {
            flags |= PAGING_IS_WRITEABLE;
        }
        /**
         * Passing the page_directory.
         * p_memz should be added.
         */
        res = paging_map_to(process->task->page_directory, paging_align_to_lower_page((void*)phdr->p_vaddr), paging_align_to_lower_page(phdr_phys_address), paging_align_address(phdr_phys_address+phdr->p_memsz), flags);
        if (ISERR(res))
        {
            break;
        }
    }

    return res;
}

// the parameter is the ready-loaded process at this point
int process_map_memory(struct process* process)
{
    int res = 0;
    switch(process->filetype)
    {
        case PROCESS_FILETYPE_ELF:
            res = process_map_elf(process);
        break;

        case PROCESS_FILETYPE_BINARY:
            res = process_map_binary(process);
        break;

        default:
            panic("process_map_memory: Invalid filetype\n");
    }
    if (res < 0)
    {
        goto out;
    }
    
    // the writable allow write own stack, map the stack memory
    paging_map_to(process->task->page_directory, (void*)MULTITASK_OS_KERNELSHELL_PROGRAM_VIRTUAL_STACK_ADDRESS_END, process->stack, paging_align_address(process->stack+MULTITASK_OS_KERNELSHELL_USER_PROGRAM_STACK_SIZE), PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE);
out:
    return res;
}

int process_get_free_slot()
{
    for (int i = 0; i < MULTITASK_OS_KERNELSHELL_MAX_PROCESSES; i++)
    {
        if (processes[i] == 0)
            return i;
    }

    return -EISTKN;
}

int process_load(const char* filename, struct process** process)
{
    int res = 0;
    int process_slot = process_get_free_slot();// find the process slot to load
    if (process_slot < 0)
    {
        res = -EISTKN;
        goto out;
    }

    res = process_load_for_slot(filename, process, process_slot);
out:
    return res;
}

int process_load_switch(const char* filename, struct process** process)
{
    int res = process_load(filename, process);
    if (res == 0)
    {
        process_switch(*process);// access the actual process
    }

    return res;
}

// the address pointer to process, 
int process_load_for_slot(const char* filename, struct process** process, int process_slot)
{
    int res = 0;
    struct task* task = 0;
    struct process* _process;
    void* program_stack_ptr = 0;

    if (process_get(process_slot) != 0)
    {
        res = -EISTKN; // check, cause can not load a process has already in slot
        goto out;
    }

    _process = kzalloc(sizeof(struct process));
    if (!_process)
    {
        res = -ENOMEM; // done here for running out of memory
        goto out;
    }

    process_init(_process);
    res = process_load_data(filename, _process);
    if (res < 0)
    {
        goto out;
    }

    // create a stack memory
    program_stack_ptr = kzalloc(MULTITASK_OS_KERNELSHELL_USER_PROGRAM_STACK_SIZE); // 1024*16
    if (!program_stack_ptr)
    {
        res = -ENOMEM; // fail to create a stack memory
        goto out;
    }

    strncpy(_process->filename, filename, sizeof(_process->filename));
    _process->stack = program_stack_ptr;
    _process->id = process_slot;

    // Create a task, here is only 1 task
    task = task_new(_process);
    if (ERROR_I(task) == 0)
    {
        res = ERROR_I(task);
        goto out; // if the task is not created, return the error
    }

    // now we have the task ready to be run

    _process->task = task;

    res = process_map_memory(_process); // paging model to map the memory of the process
    if (res < 0)
    {
        goto out;
    }

    // now we have map the memory

    *process = _process;

    // Add the process to the array
    processes[process_slot] = _process;

out:
    if (ISERR(res)) // if there is an error in the result
    {
        if (_process && _process->task)
        {
            task_free(_process->task);
        }

       // Free the process data
    }
    return res;
}