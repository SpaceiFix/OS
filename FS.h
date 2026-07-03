#ifndef FS_H
#define FS_H

#define MAGIC_PON 0x6767

#define SUPERBLOCK_LBA 33
#define INODES_START_LBA 34
#define INODE_COUNT 32
#define DATA_START_LBA 40

struct Superblock {
    unsigned short magic;
    unsigned int total_blocks;
    unsigned int inode_blocks;
    unsigned int inode_count;
    unsigned int data_start_lba;
};

struct Inode {
    unsigned int size;
    unsigned short flags;
    unsigned int direct_blocks[12];
    char reserved[10];
};

struct DirectoryEntry {
    unsigned int inode_num;
    char name[12];
};

#endif
