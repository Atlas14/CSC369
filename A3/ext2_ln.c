#include "ext2.h"
#include "ext2_utils.h"
#include <stdio.h>
#include <string.h>


int main(int argc, char **argv) {
	char * target_file_path;
	char target_file_name[256];
	unsigned int spath, tpath_index, sfile_index;
	struct ext2_inode new_inode;
	struct ext2_dir_entry_2 new_entry;
	struct ext2_inode * source_file_inode;

	if(argc != 4) {
        fprintf(stderr, "Usage: ext2_ln <image file name> <source path> <target path>\n");
        exit(1);
    }
    
    if (ext2_init(argv[1]) == -1){
        exit(1);
    }

    // check source path and target path
    if(!check_file_path(argv[2]) || !check_file_path(argv[3]) ) {
        printf("invaid abs path\n");
        return ENOENT;
    }

    // get target file path and name
    target_file_path = malloc(strlen(argv[3]));
    get_last_entry(argv[3],target_file_name, target_file_path);

    // check if it is vaild target file name
    if (strlen(target_file_name) == 0){
        printf("invaid target file name\n");
        return ENOENT;
    } 
    if(is_dir(argv[3])){
        printf("invaid target file name\n");
        return ENOENT;
    }
    // get inode index for source file and target file
    sfile_index = get_inode_index(argv[2]);
    tpath_index = get_inode_index(target_file_path);
    free(target_file_path);
    // check if source path and target path vaild
    if(!sfile_index || !tpath_index){
        printf("No such file or directory\n");
        return ENOENT;
    }
    
    // check if the target file already exist
    if(get_inode_index(argv[3])){
        if((get_inode(get_inode_index(argv[3]))->i_mode & EXT2_S_IFREG)){
            printf("target file already exist\n");
            return EEXIST;
        }
    }
    
    // check if the source path lead to a file
    source_file_inode = get_inode(sfile_index);
    if(!(source_file_inode->i_mode & EXT2_S_IFREG) ){
        printf("Not a file\n");
        return EISDIR;
    }

    new_entry.file_type = EXT2_FT_REG_FILE;
    // link to source file
    new_entry.inode = sfile_index;
    new_entry.name_len = strlen(target_file_name);

    source_file_inode->i_links_count++;

    add_entry(get_inode(tpath_index), new_entry, target_file_name);

	return 0;
}