#include "../includes/fs.h"

StringList get_files_in_dir(char* dir){

    StringList files = init_strings();

    struct dirent *de;
    DIR *dr = opendir(dir);

    if(dr == NULL){
        return files;
    }

    while((de = readdir(dr)) != NULL){
        add_string_to_list(&files,de->d_name);
    }

    closedir(dr);
    return files;
}
