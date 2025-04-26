#include "io/io.h"
#include "disk.h"
#include "memory/memory.h"
#include "config.h"
#include "status.h"
/*
osdev ATA read
*/

struct disk disk;

int disk_read_sector(int lba, int total, void* buf)
{
    outb(0x1F6, (lba >> 24) & 0xE0); // set the high 8 bits of the LBA
    outb(0x1F2, total); // set the number of sectors to read
    outb(0x1F3, (unsigned char)(lba & 0xff)); 
    outb(0x1F4, (unsigned char)((lba >> 8) & 0xff));// shift to right by 8 bits
    outb(0x1F5, (unsigned char)((lba >> 16) & 0xff));

    unsigned short* ptr = (unsigned short*)buf;
    for (int b = 0; b < total; b++)
    {
        // wait for the drive to be ready
        char c  = insb(0x1F7);

        while(!(c & 0x08)) // check if the drive is ready
        {
            c = insb(0x1F7);
        }

        // copy from hard disk to buffer
        for (int i = 0; i < 256; i++)
        {
            *ptr = insw(0x1F0); // read the data from the drive
            ptr++;
        }
    }
    return 0;
}

void disk_search_and_init()
{
    memset(&disk, 0, sizeof(disk));
    disk.type = MULTITASK_OS_KERNELSHELL_DISK_TYPE_REAL;
    disk.sector_size = MULTITASK_OS_KERNELSHELL_SECTOR_SIZE; // 512 bytes per sector

}

struct disk* disk_get(int index)
{
    if (index != 0)
        return 0;

    return &disk;
}

int disk_read(struct disk* idisk, unsigned int lba, int total, void* buf)
{
    if(idisk != &disk)
    {
        return -EIO;
    }

    return disk_read_sector(lba, total, buf);
}
