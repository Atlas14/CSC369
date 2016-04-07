#include "ext2.h"
#include "ext2_utils.h"
#include <string.h>
#include <stdio.h>

int main(int argc, char **argv) {
	char * dirpath;
	char dirname[256];
	unsigned int parent_index, new_dir_index;
	struct ext2_inode new_dir_inode;
	struct ext2_dir_entry_2 new_dir;
	struct ext2_inode * parent_inode;

    if(argc != 3) {
        fprintf(stderr, "Usage: ext2_mkdir <image file name> <file path>\n");
        exit(1);
    }
    if (ext2_init(argv[1]) == -1){
    	exit(1);
    }
    // check file path
    if(! check_file_path(argv[2])){
    	printf("invaid abs path\n");
        return ENOENT;
    }

    dirpath = malloc(strlen(argv[2]));
    get_last_entry(argv[2],dirname,dirpath);
    // check if it is a vaild dirs
    if (strlen(dirname) == 0){
    	printf("invaid dir name\n");
    	return ENOENT;
    } 
    parent_index = get_inode_index(dirpath);

    //  check if it is a vaild dir path
    if(!parent_index || !(get_inode(parent_index)->i_mode & EXT2_S_IFDIR) ){
    	printf("mkdir: %s: No such file or directory\n", dirpath);
    	return ENOENT;
    }
    // check if the dir is already in parent dir
	parent_inode = get_inode(parent_index);
    if (find_entry_inode(parent_inode, dirname)){
    	printf("dir already exist\n");
    	return EEXIST;
    } 
    // free 
    free(dirpath);

    // creat dir inode
   	new_dir_index = allocate_inode();
   	// set links
   	new_dir_inode.i_links_count = 1;
	// set mode to dir
	new_dir_inode.i_mode |= EXT2_S_IFDIR;
	
	int i;
	
	for (i = 0; i < 15; i++)
		new_dir_inode.i_block[i] = 0;

	// set new dir
	*(get_inode(new_dir_index)) = new_dir_inode;
	new_dir.inode = new_dir_index;
	new_dir.name_len = strlen(dirname);
	new_dir.file_type = EXT2_FT_DIR;



	// make dir
	// add new_dir to parent
	add_entry(parent_inode, new_dir, dirname);
	// // create ".", and ".." dir to new dir 
	struct ext2_dir_entry_2 one_dot_dir;
	struct ext2_dir_entry_2 two_dot_dir;

	one_dot_dir.inode = new_dir_index;
	one_dot_dir.name_len = 1;
	one_dot_dir.file_type = EXT2_FT_DIR;

	two_dot_dir.inode = parent_index;
	two_dot_dir.name_len = 2;
	two_dot_dir.file_type = EXT2_FT_DIR;

	add_entry(get_inode(new_dir_index), one_dot_dir, ".");
	add_entry(get_inode(new_dir_index), two_dot_dir, "..");

}