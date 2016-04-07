#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "ext2.h"

unsigned char *disk;


int main(int argc, char **argv) {

    if(argc != 2) {
        fprintf(stderr, "Usage: readimg <image file name>\n");
        exit(1);
    }
    int fd = open(argv[1], O_RDWR);

    disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(disk == MAP_FAILED) {
	perror("mmap");
	exit(1);
    }

    struct ext2_super_block *sb = (struct ext2_super_block *)(disk + 1024);
    printf("Inodes: %d\n", sb->s_inodes_count);
    printf("Blocks: %d\n", sb->s_blocks_count);
    struct ext2_group_desc *bg = (struct ext2_group_desc *) (disk + 2048);

    //struct ext2_group_desc *bg = (struct ext2_group_desc *) (((unsigned char *)sb) + 1024);
    printf("Block group:\n");
    printf("\tblock bitmap: %d\n", bg->bg_block_bitmap);
    printf("\tinode bitmap: %d\n", bg->bg_inode_bitmap);
    printf("\tinode table: %d\n", bg->bg_inode_table);
    printf("\tfree blocks: %d\n", bg->bg_free_blocks_count);
    printf("\tfree inodes: %d\n", bg->bg_free_inodes_count);
    printf("\tused_dirs: %d\n", bg->bg_used_dirs_count);

    return 0;
}