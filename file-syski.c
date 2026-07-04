#include "fs.h"
#include "drivers.h"

int str_compare(const char *s1, const char *s2);

void byte_set(void *dest, unsigned char val, int count) {
    unsigned char *ptr = (unsigned char *)dest;
    for (int i = 0; i < count; i++) {
        ptr[i] = val;
    }
}

void mkfs(void) {
    kprint("i lov kimono, and ur files are gon (on ponOS btw)\n");

    __attribute__((aligned(16))) char buffer[512];
    byte_set(buffer, 0, 512);

    struct Superblock *sb = (struct Superblock *)buffer;
    sb->magic = MAGIC_PON;
    sb->total_blocks = 20480;
    sb->inode_count = INODE_COUNT;
    sb->data_start_lba = DATA_START_LBA;

    ata_write_sectors(buffer, SUPERBLOCK_LBA, 1);

    byte_set(buffer, 0, 512);
    for (int i = 0; i < 4; i++) {
        ata_write_sectors(buffer, INODES_START_LBA + i, 1);
    }

    byte_set(buffer, 0, 512);
    ata_write_sectors(buffer, DATA_START_LBA, 1);

    kprint("Format complete! ponFS initialized successfully\n");
}

int check_fs(void) {
    char buffer[512];
    ata_read_sectors(buffer, SUPERBLOCK_LBA, 1);
    struct Superblock *sb = (struct Superblock *)buffer;
    return (sb->magic == MAGIC_PON);
}

void sys_touch(const char *filename) {
    if (!check_fs()) {
        kprint("No valid ponFS found\n");
        return;
    }

    __attribute__((aligned(16))) char buf[512];
    ata_read_sectors(buf, INODES_START_LBA, 1);

    int free_inode_idx = -1;
    struct Inode *inodes_array = (struct Inode *)buf;

    for (int i = 0; i < 8; i++) {
        if (inodes_array[i].flags == 0) {
            free_inode_idx = i;
            break;
        }
    }

    if (free_inode_idx == -1) {
        kprint("Error: No free inodes.\n");
        return;
    }

    __attribute__((aligned(16))) char root_buf[512];
    ata_read_sectors(root_buf, DATA_START_LBA, 1);

    int free_dir_idx = -1;
    struct DirectoryEntry *dir_entries = (struct DirectoryEntry *)root_buf;

    for (int i = 0; i < 16; i++) {
        if (dir_entries[i].inode_num == 0) {
            free_dir_idx = i;
            break;
        }
    }

    if (free_dir_idx == -1) {
        kprint("not enough storage\n");
        return;
    }

    unsigned int assigned_lba = DATA_START_LBA + 2 + free_inode_idx;

    inodes_array[free_inode_idx].size = 0;
    inodes_array[free_inode_idx].flags = 1;
    inodes_array[free_inode_idx].direct_blocks[0] = assigned_lba;

    dir_entries[free_dir_idx].inode_num = free_inode_idx + 1;

    byte_set(dir_entries[free_dir_idx].name, 0, 12);
    for (int len = 0; len < 11; len++) {
        if (filename[len] == '\0') break;
        dir_entries[free_dir_idx].name[len] = filename[len];
    }

    ata_write_sectors(buf, INODES_START_LBA, 1);
    ata_write_sectors(root_buf, DATA_START_LBA, 1);

    __attribute__((aligned(16))) char clear_buf[512];
    byte_set(clear_buf, 0, 512);
    ata_write_sectors(clear_buf, assigned_lba, 1);

    kprint("File '");
    kprint(filename);
    kprint("' created successfully!\n");
}

void sys_ls(void) {
    if (!check_fs()) {
        kprint("No valid ponFS found\n");
        return;
    }

    __attribute__((aligned(16))) char root_buf[512];
    ata_read_sectors(root_buf, DATA_START_LBA, 1);

    kprint("Listing directory /\n");
    int files_found = 0;
    struct DirectoryEntry *dir_entries = (struct DirectoryEntry *)root_buf;

    for (int i = 0; i < 16; i++) {
        if (dir_entries[i].inode_num != 0) {
            kprint("  * ");
            kprint(dir_entries[i].name);
            kprint("\n");
            files_found++;
        }
    }

    if (files_found == 0) {
        kprint("  (directory is empty)\n");
    }
}

void sys_write_file(const char *filename, const char *text) {
    char root_buf[512];
    ata_read_sectors(root_buf, DATA_START_LBA, 1);
    int target_inode = -1;

    for (int i = 0; i < 16; i++) {
        unsigned int inode_num = *(unsigned int *)(root_buf + (i * 16));
        if (inode_num != 0) {
            char *name_ptr = (char *)(root_buf + (i * 16) + 4);
            if (str_compare(name_ptr, filename) == 0) {
                target_inode = inode_num - 1;
                break;
            }
        }
    }

    if (target_inode == -1) {
        kprint("File not found\n");
        return;
    }

    char buf[512];
    ata_read_sectors(buf, INODES_START_LBA, 1);

    unsigned int file_lba = *(unsigned int *)(buf + (target_inode * 64) + 6);

    char data_buf[512];
    byte_set(data_buf, 0, 512);

    int len = 0;
    while (text[len] != '\0' && len < 511) {
        data_buf[len] = text[len];
        len++;
    }
    data_buf[len] = '\0';

    *(unsigned int *)(buf + (target_inode * 64)) = len;

    ata_write_sectors(data_buf, file_lba, 1);
    ata_write_sectors(buf, INODES_START_LBA, 1);

    kprint("Wrote text to file successfully.\n");
}

void sys_cat(const char *filename) {
    char root_buf[512];
    ata_read_sectors(root_buf, DATA_START_LBA, 1);
    int target_inode = -1;

    for (int i = 0; i < 16; i++) {
        unsigned int inode_num = *(unsigned int *)(root_buf + (i * 16));
        if (inode_num != 0) {
            char *name_ptr = (char *)(root_buf + (i * 16) + 4);
            if (str_compare(name_ptr, filename) == 0) {
                target_inode = inode_num - 1;
                break;
            }
        }
    }

    if (target_inode == -1) {
        kprint("File not found\n");
        return;
    }

    char buf[512];
    ata_read_sectors(buf, INODES_START_LBA, 1);

    unsigned int file_lba = *(unsigned int *)(buf + (target_inode * 64) + 6);

    char data_buf[513];
    byte_set(data_buf, 0, 513);
    ata_read_sectors(data_buf, file_lba, 1);

    kprint(data_buf);
    kprint("\n");
}

void sys_rm(const char *filename) {
    if (!check_fs()) {
        kprint("No valid ponFS found\n");
        return;
    }

    char root_buf[512];
    ata_read_sectors(root_buf, DATA_START_LBA, 1);
    int target_inode = -1;
    int dir_entry_idx = -1;

    for (int i = 0; i < 16; i++) {
        unsigned int inode_num = *(unsigned int *)(root_buf + (i * 16));
        if (inode_num != 0) {
            char *name_ptr = (char *)(root_buf + (i * 16) + 4);
            if (str_compare(name_ptr, filename) == 0) {
                target_inode = inode_num - 1;
                dir_entry_idx = i;
                break;
            }
        }
    }

    if (target_inode == -1) {
        kprint("File not found\n");
        return;
    }

    char buf[512];
    ata_read_sectors(buf, INODES_START_LBA, 1);

    unsigned int file_lba = *(unsigned int *)(buf + (target_inode * 64) + 6);

    *(unsigned int *)(buf + (target_inode * 64)) = 0;
    *(unsigned short *)(buf + (target_inode * 64) + 4) = 0;
    *(unsigned int *)(buf + (target_inode * 64) + 6) = 0;

    *(unsigned int *)(root_buf + (dir_entry_idx * 16)) = 0;
    char *del_name_ptr = (char *)(root_buf + (dir_entry_idx * 16) + 4);
    for (int i = 0; i < 12; i++) {
        del_name_ptr[i] = '\0';
    }

    ata_write_sectors(buf, INODES_START_LBA, 1);
    ata_write_sectors(root_buf, DATA_START_LBA, 1);

    char clear_buf[512];
    byte_set(clear_buf, 0, 512);
    ata_write_sectors(clear_buf, file_lba, 1);

    kprint("File '");
    kprint(filename);
    kprint("' deleted successfully\n");
}
