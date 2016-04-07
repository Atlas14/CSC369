#include "ext2.h"
#include "ext2_utils.h"
#include <stdio.h>
#include <string.h>


int main(int argc, char **argv) {
	char * target_file_path;
	char target_file_name[256];
	unsigned int tpath_index, tfile_index;
	struct ext2_inode parent, target;


	if(argc != 3) {
        fprintf(stderr, "Usage: ext2_rm <image file name> <target path>\n");
        exit(1);
    }
    
    if (ext2_init(argv[1]) == -1){
        exit(1);
    }

    // check target path
    if(!check_file_path(argv[2]) ) {
        printf("invaid abs path\n");
        return ENOENT;
    }

    // get target file path and name
    target_file_path = malloc(strlen(argv[2]));
    get_last_entry(argv[2],target_file_name, target_file_path);

    // check if it is vaild target file name
    
    if (! strlen(target_file_name)){
        printf("invaid target file name\n");
        return ENOENT;
    } 
    if (is_dir(argv[2])){
        printf("invaid target file name\n");
        return ENOENT;
    }

    // get inode index for source file and target file
    tfile_index = get_inode_index(argv[2]);
    tpath_index = get_inode_index(target_file_path);
    free(target_file_path);

    // check if source path and target path vaild
    if(!tpath_index){
        printf("No such file or directory\n");
        return ENOENT;
    }    
    // check if the target is the file
    if(tfile_index){
        if(! (get_inode(tfile_index)->i_mode & EXT2_S_IFREG)){
            printf("%s not a file\n", target_file_name);
            return ENOENT;
        }
    }
    
    remove_inode(get_inode(tpath_index), get_inode(tfile_index), tfile_index);

    return 0;
}

