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

// print dir info
void print_dir(unsigned int curr_block){
  struct ext2_dir_entry_2 * dir_entry = (struct ext2_dir_entry_2 *)(disk + EXT2_BLOCK_SIZE * curr_block);

  int i = 0;
  while (i < EXT2_BLOCK_SIZE) {
    i += dir_entry->rec_len;
    char type = '0';
    if ((unsigned int) dir_entry->file_type == EXT2_FT_REG_FILE) {
      type = 'f';
    } 
    else if((unsigned int) dir_entry->file_type == EXT2_FT_DIR) {
      type = 'd';
    }
    printf("Inode: %d rec_len: %d name_len: %d type= %c name=%.*s\n", 
      dir_entry->inode, dir_entry->rec_len, dir_entry->name_len, type, dir_entry->name_len, dir_entry->name);
    dir_entry = (void *) dir_entry + dir_entry->rec_len;
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

    // TE8

    int * dir = malloc(sizeof(int) * sb->s_inodes_count);

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
        // not dir
        *(dir + i) = 0;
      } else if (inode->i_mode & EXT2_S_IFDIR) {
        type = 'd';
        // is dir
        *(dir + i) = 1;
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
    
    // TE9
    printf("\n");
    printf("Directory Blocks:\n");
    
    for (i = EXT2_ROOT_INO - 1; i < sb->s_inodes_count; i++) {
      // check if is dir or file (1 dir)
      if (*(dir + i) == 1) {
        inode = (struct ext2_inode *)(inode_info + (sizeof(struct ext2_inode) * i));
       
        int block_count = inode->i_blocks/2;

        unsigned int curr_block;

        for (j = 0; j < block_count; j++) {
          curr_block = inode->i_block[j];
          printf("   DIR BLOCK NUM: %d (for inode %d)\n", curr_block, i + 1);
          print_dir(curr_block);
    
        }
      }
    }
    
    return 0;
}
