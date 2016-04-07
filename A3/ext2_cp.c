#include <stdio.h>
#include <string.h>
#include "ext2.h"
#include "ext2_utils.h"

int main(int argc, char const *argv[]) {
    char * target_path, * source_path;
    char target_file[256], source_file[256];
    unsigned int sfile, tpath, tfile;
    struct ext2_dir_entry_2 new_entry;

    if(argc != 4) {
        fprintf(stderr, "Usage: ext2_cp <image file name> <source path> <target path>\n");
        exit(1);
    }
    
    // check if the provied path vaild
    if(!check_file_path(argv[2]) || !check_file_path(argv[3]) ) {
        printf("invaid abs path\n");
        return ENOENT;
    }
    
    if (ext2_init(argv[1]) == -1){
       exit(1);
    }

    target_path = malloc(strlen(argv[3]));
    get_last_entry(argv[3], target_file, target_path);

    source_path = malloc(strlen(argv[2]));
    get_last_entry(argv[2], source_file, source_path);

    // source file
    sfile = get_inode_index(argv[2]);
    // path to target file
    tpath = get_inode_index(target_path);
    tfile = get_inode_index(argv[3]); 
    free(target_path);
    free(source_path);
    // tpath should be a dir and sfile is a file
    if(tpath && (get_inode(tpath)->i_mode & EXT2_S_IFDIR)
     && sfile && (get_inode(sfile)->i_mode & EXT2_S_IFREG)){
        // file does not exist
        if(!tfile){
            tfile = allocate_inode();
            copy_inode(tfile, sfile);

            new_entry.file_type = EXT2_FT_REG_FILE;
            new_entry.inode = tfile;
            new_entry.name_len = strlen(target_file);

            add_entry(get_inode(tpath), new_entry, target_file);
        }
        
        // tfile is dir
        else if(get_inode(tfile)->i_mode & EXT2_S_IFDIR){
            // sfile does not in dir(tfile)
            if(!find_entry_inode(get_inode(tfile), source_file)){
                tpath = tfile;
                tfile = allocate_inode();
                copy_inode(tfile, sfile);

                new_entry.file_type = EXT2_FT_REG_FILE;
                new_entry.inode = tfile;
                new_entry.name_len = strlen(source_file);

                add_entry(get_inode(tpath), new_entry, source_file);
            }
            // sfile already exist
            else{
                printf("file already exist\n");
                return ENOENT;
            }
        }
    }
    else{
        printf("No such file or directory\n");
        return ENOENT;
    }
    
    return 0;
}