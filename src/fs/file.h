#ifndef FILE_H
#define FILE_H
#include "pparser.h"
#include <stdint.h>
typedef unsigned int FILE_SEEK_MODE;
enum
{
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};

typedef unsigned int FILE_MODE;
enum
{
    FILE_MODE_READ,
    FILE_MODE_WRITE,
    FILE_MODE_APPEND,
    FILE_MODE_INVALID
};

enum
{
    FILE_STAT_READ_ONLY = 0b00000001
};

typedef unsigned int FILE_STAT_FLAGS;

struct disk;
typedef void*(*FS_OPEN_FUNCTION)(struct disk* disk, struct path_part*, FILE_MODE mode);
typedef int (*FS_READ_FUNCTION)(struct disk* disk, void* private, uint32_t size, uint32_t nmemb, char* out);
// this function accept 4 parameters, the nmemb is how many of block you want to read. 
typedef int(*FS_RESOLVE_FUNCTION)(struct disk* disk);

typedef int (*FS_CLOSE_FUNCTION)(void* private);

// implement the seek function from file.c
typedef int (*FS_SEEK_FUNCTION)(void* private, uint32_t offset, FILE_SEEK_MODE seek_mode);

//this struct needs to be abrove stat funcion
struct file_stat
{
    FILE_STAT_FLAGS flags;
    uint32_t filesize;
};

typedef int (*FS_STAT_FUNCTION)(struct disk* disk, void* private, struct file_stat* stat);

struct filesystem
{
    // Filesystem should return zero from resolve if the provided disk is using its filesystem
    FS_RESOLVE_FUNCTION resolve;
    FS_OPEN_FUNCTION open;
    FS_READ_FUNCTION read;
    FS_SEEK_FUNCTION seek;
    FS_STAT_FUNCTION stat; // this function should return the file stat
    FS_CLOSE_FUNCTION close; // this function should close the file descriptor

    char name[20];
};

struct file_descriptor
{
    // The descriptor index
    int index;
    struct filesystem* filesystem;

    // Private data for internal file descriptor
    void* private;

    // The disk that the file descriptor should be used on
    struct disk* disk;
};






void fs_init();
int fopen(const char* filename, const char* mode);
int fseek(int fd, int offset, FILE_SEEK_MODE whence);
int fread(void* ptr, uint32_t size, uint32_t nmemb, int fd); // the prototype
int fstat(int fd, struct file_stat* stat);
int fclose(int fd);

void fs_insert_filesystem(struct filesystem* filesystem);
struct filesystem* fs_resolve(struct disk* disk);
#endif