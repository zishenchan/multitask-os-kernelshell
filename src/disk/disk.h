#ifndef DISK_H
#define DISK_H

typedef unsigned int MULTITASK_OS_KERNELSHELL_DISK_TYPE;

// represents a real physical disk
#define MULTITASK_OS_KERNELSHELL_DISK_TYPE_REAL 0

struct disk
{
    MULTITASK_OS_KERNELSHELL_DISK_TYPE type; // type of disk
    int sector_size;

    int total; // total number of sectors
    void* buf; // buffer to read/write data
};
  
void disk_search_and_init();
struct disk* disk_get(int index);
int disk_read(struct disk* idisk, unsigned int lba, int total, void* buf);

#endif