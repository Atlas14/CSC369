#include <stdio.h>
#include <string.h>
#include "ext2_utils.h"
#include "ext2.h"

void print_dir_entries(const struct ext2_inode * inode, int aflag) {
    unsigned int i, j, k;
    struct ext2_dir_entry_2 * dir_entry;
    for (i = 0; i < 12; i++) {
        if (inode->i_block[i]) {
            const unsigned char * list = disk + (inode->i_block[i]) * EXT2_BLOCK_SIZE;
            struct ext2_dir_entry_2 * dir_entry;
            const unsigned char * list_ptr = list;
            const unsigned char * list_end = list + EXT2_BLOCK_SIZE;
            while (list_ptr < list_end){
                dir_entry = (struct ext2_dir_entry_2 *)list_ptr;
                if(aflag){
                    if(dir_entry->name_len != 0){
                        printf("%.*s\n",dir_entry->name_len, dir_entry->name);
                    }
                    
                }
                else{
                    if(strncmp(dir_entry->name, "..", dir_entry->name_len) != 0){
                        if(dir_entry->name_len != 0){
                            printf("%.*s\n",dir_entry->name_len, dir_entry->name);
                        }
                    }
                }
                list_ptr += dir_entry->rec_len;
            }
        }
    }
}

int main(int argc, char **argv) {
    int opt, aflag = 0;
    unsigned int inode_index;
    char * path;
    // store the name of file if the given path is a file
    char name[EXT2_NAME_LEN];
    
    if (argc !=3 && argc != 4){
        fprintf(stderr, "Usage: ext2_ls <image file name> flag <abs path> \n");
        exit(1);
    }

    if(argc == 3 || argc == 4){

        if (ext2_init(argv[1]) == -1){
            return ENOENT;
        }
        
        if(argc == 3){
            path = malloc(strlen(argv[2]));
            inode_index = get_inode_index(argv[2]); 
            get_last_entry(argv[2],name,path);
        }
        else{
            if(strcmp(argv[2],"-a") == 0){
                aflag = 1;
                path = malloc(strlen(argv[3]));
                inode_index = get_inode_index(argv[3]);           
                get_last_entry(argv[3],name,path); 
            }
            else{
                printf("Invaild flag\n");
                return ENOENT;
            }
        }
    }

    if(inode_index){
        struct ext2_inode * inode = get_inode(inode_index);
        if(inode){
            // check it's a file or dir
            if (inode->i_mode & EXT2_S_IFDIR){
                 print_dir_entries(inode, aflag);
            }
            else{
                printf("%s\n", name);
            }
        }
    }
    else{
         printf("no such file or diretory\n");
         return ENOENT;
    }
    return 0;
}
