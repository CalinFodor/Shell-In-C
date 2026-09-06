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

void print_char_n(char c,int n){
    for(int i=0;i<n;i++)
        printf("%c",c);
    printf("\n");
}

void print_welcome(){
    printf("\n");
    print_char_n('=',WELCOME_LEN);

    printf(".____    .__  __    __  .__           _________.__           .__  .__   \n");
    printf("|    |   |__|/  |__/  |_|  |   ____  /   _____/|  |__   ____ |  | |  |  \n");
    printf("|    |   |  \\   __\\   __\\  | _/ __ \\ \\_____  \\ |  |  \\_/ __ \\|  | |  |  \n");
    printf("|    |___|  ||  |  |  | |  |_\\  ___/ /        \\|   Y  \\  ___/|  |_|  |__\n");
    printf("|_______ \\__||__|  |__| |____/\\___  >_______  /|___|  /\\___  >____/____/\n");
    printf("        \\/                        \\/        \\/      \\/     \\/           \n");

    print_char_n('=',WELCOME_LEN);
    printf("\n");
}

