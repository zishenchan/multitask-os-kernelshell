#include "file.h"
#include "config.h"
#include "memory/memory.h"
#include "memory/heap/kheap.h"
#include "status.h"
#include "kernel.h"
#include "fat/fat16.h"
#include "disk/disk.h"
#include "string/string.h"

struct filesystem* filesystems[MULTITASK_OS_KERNELSHELL_MAX_FILESYSTEMS];// filesystem is array
struct file_descriptor* file_descriptors[MULTITASK_OS_KERNELSHELL_FILE_DESCRIPTOR];

static struct filesystem** fs_get_free_filesystem()
{
    int i = 0;
    for (i = 0; i < MULTITASK_OS_KERNELSHELL_MAX_FILESYSTEMS; i++)
    {
        if (filesystems[i] == 0)
        {
            return &filesystems[i];
        }
    }

    return 0;
}

void fs_insert_filesystem(struct filesystem* filesystem)
{
    struct filesystem** fs;
    fs = fs_get_free_filesystem(); 
    if (!fs)
    {
        print("Problem inserting filesystem"); 
        while(1) {}
    }

    *fs = filesystem;
}

static void fs_static_load()
{
    //fs_insert_filesystem(fat16_init());
}

void fs_load() // load the available filesystems 
{
    memset(filesystems, 0, sizeof(filesystems));
    fs_static_load();
}

void fs_init()// initialize the file system
{
    memset(file_descriptors, 0, sizeof(file_descriptors));
    fs_load();
}

static int file_new_descriptor(struct file_descriptor** desc_out)// how descriptor made
{
    int res = -ENOMEM;
    for (int i = 0; i < MULTITASK_OS_KERNELSHELL_FILE_DESCRIPTOR; i++)
    {
        if (file_descriptors[i] == 0)
        {
            struct file_descriptor* desc = kzalloc(sizeof(struct file_descriptor));
            // Descriptors start at 1
            desc->index = i + 1;
            file_descriptors[i] = desc;
            *desc_out = desc;
            res = 0;
            break;
        }
    }

    return res;
}

static struct file_descriptor* file_get_descriptor(int fd)
{
    if (fd <= 0 || fd >= MULTITASK_OS_KERNELSHELL_FILE_DESCRIPTOR)
    {
        return 0;
    }

    // Descriptors start at 1
    int index = fd - 1;
    return file_descriptors[index];
}

struct filesystem* fs_resolve(struct disk* disk)
{
    struct filesystem* fs = 0;
    for (int i = 0; i < MULTITASK_OS_KERNELSHELL_MAX_FILESYSTEMS; i++)
    {
        // filesystems[i] here is filesystem created by FAT16
        if (filesystems[i] != 0 && filesystems[i]->resolve(disk) == 0)
        {
            fs = filesystems[i];
            break;
        }
    }

    return fs;
}


FILE_MODE file_get_mode_by_string(const char* str)
{
    FILE_MODE mode = FILE_MODE_INVALID;
    if (strncmp(str, "r", 1) == 0) // we choose the binary mode by provided in header
    {
        mode = FILE_MODE_READ;
    }
    else if(strncmp(str, "w", 1) == 0)
    {
        mode = FILE_MODE_WRITE;
    }
    else if(strncmp(str, "a", 1) == 0)
    {
        mode = FILE_MODE_APPEND;
    }
    return mode;
}

int fopen(const char* filename, const char* mode_str) // to locate file by correct drive number
{
    int res = 0;
    struct path_root* root_path = pathparser_parse(filename, NULL); // parse the path, like 0:/test.txt
    if (!root_path)
    {
        res = -EINVARG;
        goto out;
    }

    // We cannot have just a root path 0:/ 0:/test.txtMore actions
    if (!root_path->first)
    {
        res = -EINVARG;
        goto out;
    } // ensure we do not waste any CPU time

    // Ensure the disk we are reading from exists
    struct disk* disk = disk_get(root_path->drive_no); // pass the disk humber, like 0:// 0://, the second one
    if (!disk)
    {
        res = -EIO;
        goto out;
    }

    // check if the filesystem is available for the disk
    if (!disk->filesystem)
    {
        res = -EIO;
        goto out;
    }

    // we know it's valid disk, so we can resolve the filesystem
    FILE_MODE mode = file_get_mode_by_string(mode_str);
    if (mode == FILE_MODE_INVALID)
    {
        res = -EINVARG;
        goto out;
    }

    // this is how file is being linked
    void* descriptor_private_data = disk->filesystem->open(disk, root_path->first, mode);
    
    if (ISERR(descriptor_private_data)) // know it's error, and check
    {
        res = ERROR_I(descriptor_private_data);
        goto out;
    }

    struct file_descriptor* desc = 0;
    res = file_new_descriptor(&desc); // pass the descriptor
    if (res < 0)
    {
        goto out;
    }
    desc->filesystem = disk->filesystem;
    desc->private = descriptor_private_data;
    desc->disk = disk; // set the disk 
    res = desc->index;


out: 
    // fopen shouldnt return negative valuesMore actions
    if (res < 0)
    res = 0;
    
    return res;
}
