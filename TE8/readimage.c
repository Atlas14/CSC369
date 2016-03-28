#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include "ext2.h"

unsigned char *disk;

void print_bitmap(unsigned char * bitmap, unsigned int num_bytes) {
    int i, j;
    unsigned char * p = bitmap;
    unsigned char buf;
    for (i = 0; i < num_bytes; i++) {
      printf(" ");
      buf = *p;
      for (j = 0; j < 8; j++) {
        printf("%d", (buf >> j) & 0x1);
      }
      p++;
    }      
}

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
    printf("    block bitmap: %d\n", bg->bg_block_bitmap);
    printf("    inode bitmap: %d\n", bg->bg_inode_bitmap);
    printf("    inode table: %d\n", bg->bg_inode_table);
    printf("    free blocks: %d\n", bg->bg_free_blocks_count);
    printf("    free inodes: %d\n", bg->bg_free_inodes_count);
    printf("    used_dirs: %d\n", bg->bg_used_dirs_count);


    // block map
    printf("Block bitmap:");
    unsigned char * bitmap = (unsigned char *) (disk + EXT2_BLOCK_SIZE * bg->bg_block_bitmap);
    print_bitmap(bitmap, sb->s_blocks_count / 8);


    // inode map
    printf("\nInode bitmap:");
    bitmap = (unsigned char *) (disk + EXT2_BLOCK_SIZE * bg->bg_inode_bitmap);
    print_bitmap(bitmap, sb->s_blocks_count / 32);
    printf("\n");
    
    // inode info
    printf("\nInodes:\n");
    char * inode_info = (char *)(disk + EXT2_BLOCK_SIZE * bg->bg_inode_table);
    struct ext2_inode *inode; 
    char type = '0';
    int i, j;
    for (i = EXT2_ROOT_INO - 1; i < sb->s_inodes_count; i++){
      if (i < EXT2_GOOD_OLD_FIRST_INO && i != EXT2_ROOT_INO - 1) {
        continue;
      }

      inode = (struct ext2_inode *) (inode_info + sizeof(struct ext2_inode) * i);

      if (inode->i_size == 0) {
        continue;
      }

      if (inode->i_mode & EXT2_S_IFREG) {
        type = 'f';
      } else if (inode->i_mode & EXT2_S_IFDIR) {
        type = 'd';
      }

      printf("[%d] type: %c size: %d links: %d blocks: %d\n", i + 1, type, inode->i_size, inode->i_links_count, inode->i_blocks);
      
      int block_count = inode->i_blocks / 2;
      int blocks;
      for (j = 0; j < block_count; j++) {
        if (j > 11) {
          break;
        }
        blocks  = inode->i_block[j];
      }

      printf("[%d] Blocks:  %d\n", i + 1, blocks);

    }
    
    return 0;

}
