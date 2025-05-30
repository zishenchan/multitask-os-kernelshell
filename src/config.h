#ifndef CONFIG_H
#define CONFIG_H

#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10
#define MULTITASK_OS_KERNELSHELL_TOTAL_INTERRUPTS 512

#define MULTITASK_OS_KERNELSHELL_HEAP_SIZE_BYTES 104857600 // 100MB, 1024*1024*100
#define MULTITASK_OS_KERNELSHELL_HEAP_BLOCK_SIZE 4096 // 4KB
#define MULTITASK_OS_KERNELSHELL_HEAP_ADDRESS 0x01000000 // 16MB
#define MULTITASK_OS_KERNELSHELL_HEAP_TABLE_ADDRESS 0x00007E00 // 2KB


#define MULTITASK_OS_KERNELSHELL_SECTOR_SIZE 512 // 512 bytes

#define MULTITASK_OS_KERNELSHELL_MAX_FILESYSTEMS 12
#define MULTITASK_OS_KERNELSHELL_FILE_DESCRIPTOR 512
#endif
