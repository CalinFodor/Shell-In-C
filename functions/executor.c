#include "../includes/executor.h"

char OLDPWD[1024] = {'\0'};

static char* BUILTIN_CMDS[] = {"echo","pwd","cd","exit","history"};

bool is_builtin(char* cmd){
    int n = sizeof (BUILTIN_CMDS) / sizeof (char*);
    for(int i=0;i<n;i++){
        if(equal_strings(cmd,BUILTIN_CMDS[i])){
            return true;
        }
    }
    return false;
}

int echo(StringList tokens){
    for(int i=1;i<tokens.idx;i++)
        printf("%s ",tokens.elements[i]);
    printf("\n");
    return 0;
}

int pwd(){
    char cwd[1024];
    getcwd(cwd,sizeof(cwd));

    if(strlen(cwd)){
        printf("%s\n",cwd);
    }
}


int cd(StringList tokens){
    
    char cwd[1024];
    getcwd(cwd,sizeof(cwd));

    char* home = getenv("HOME");

    char* arg = NULL;
    if(tokens.idx == 2){
        arg = tokens.elements[1];
    }

    if(tokens.idx == 1 || strcmp(arg,"~") == 0){
        if(home == NULL){
            printf("cd: HOME not set\n");
            return - 1;
        }
        strcpy(OLDPWD,cwd);
        chdir(home);

    }else if(strcmp(arg,"-") == 0){
        if(!strlen(OLDPWD)){
            printf("cd: OLDPWD not set\n");
            return -1;
        }else{
            chdir(OLDPWD);
            strcpy(OLDPWD,cwd);
        }
    }else {
        chdir(arg);
        strcpy(OLDPWD,cwd);
    }
}

int history(History history){
    print_hist(history);
    return 0;
}

int run_builtin_cmd(StringList tokens,History hist){
    char* cmd = tokens.elements[0];

    if(equal_strings(cmd,"echo")){
        return echo(tokens);
    }else if(equal_strings(cmd,"pwd")){
        return pwd();
    }else if(equal_strings(cmd,"cd")){
        return cd(tokens);
    }else if(equal_strings(cmd,"exit")){
        exit(0);
    }else if(equal_strings(cmd,"history")){
        return history(hist);
    }
}

int run_program(StringList tokens){
    char* cmd = tokens.elements[0];
    
    tokens.elements[tokens.idx++] = NULL;

    pid_t child_pid = fork();

    if(child_pid == -1){
        perror("fork");
        return 1;
    }
    if(child_pid == 0){
        int status_code = execvp(cmd,tokens.elements);

        perror("execvp");
    }else{
        waitpid(child_pid,NULL,0);
    }
    return 0;
}

int run_commands(StringList tokens,History history){

    char* cmd = tokens.elements[0];
    if(is_builtin(cmd)){
        run_builtin_cmd(tokens,history);
    }else{
        run_program(tokens);
    }

}