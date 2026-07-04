#ifndef FS_H
#define FS_H

#define MAGIC_PON 0x6767

#define SUPERBLOCK_LBA  100
#define INODES_START_LBA 101
#define INODE_COUNT      32
#define DATA_START_LBA   110


struct Superblock {
    unsigned short magic;
    unsigned int total_blocks;
    unsigned int inode_count;
    unsigned int data_start_lba;
} __attribute__((packed));

struct Inode {
    unsigned int size;
    unsigned short flags;
    unsigned int direct_blocks[12];
    char reserved[10];
} __attribute__((packed));

struct DirectoryEntry {
    unsigned int inode_num;
    char name[12];
} __attribute__((packed));

void mkfs(void);
int check_fs(void);
void sys_touch(const char *filename);
void sys_ls(void);
void sys_write_file(const char *filename, const char *text);
void sys_cat(const char *filename);
void sys_rm(const char *filename);

#endif
