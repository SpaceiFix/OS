#include "FS.h"
#include "drivers.h"

void byte_set(void *dest, unsigned char val, int count) {
    unsigned char *ptr = (unsigned char *)dest;
    for (int i = 0; i < count; i++) {
        ptr[i] = val;
    }
}

void mkfs(void) {
    kprint("i lov kimono, and ur files are gon (on ponOS btw)\n");

    char buffer[512];
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

    kprint("Format complete! ponFS initialized successfully.\n");
}

int check_fs(void) {
    char buffer[512];
    ata_read_sectors(buffer, SUPERBLOCK_LBA, 1);

    struct Superblock *sb = (struct Superblock *)buffer;

    if (sb->magic == MAGIC_PON) {
        return 1;
    }
    return 0;
}
