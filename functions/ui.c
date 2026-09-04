#include "../includes/ui.h"

void print_prompt(){
    char cwd[1024];
    char* user;

    getcwd(cwd,sizeof cwd);

    user = getenv("USER");
    if(user == NULL){
        user = copy_string(DEFAULT_USER,strlen(DEFAULT_USER));
    }

    printf(GRN "%s"RESET"@"YEL"LittleShell" RESET ":" BLU"%s" RESET "$ ",user,cwd);
}
