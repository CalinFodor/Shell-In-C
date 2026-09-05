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

        perror(cmd);
    }else{
        waitpid(child_pid,NULL,0);
    }
    return 0;
}

int apply_redirections(RedirInfo redir_info){
    char* file = redir_info.target_file;

    if(file == NULL)
        return -1;

    //dup2(src,des)
    int saved_fd = dup(redir_info.fd);
    if(file[0] == '&'){
        int old_fd = file[1] - '0';
        int new_fd = redir_info.fd;
        //printf("DUP2 %d %d\n",old_fd,new_fd);
        dup2(old_fd,new_fd);
        close(old_fd);
    }else{
        int fd;
        switch(redir_info.redir_dir){
            case Input:
                fd = open(file,O_RDONLY,0644);
                dup2(fd,redir_info.fd);
                break;
            case Output:

                if(redir_info.modify_flag == Trunc){
                    fd = open(file,O_WRONLY|O_CREAT|O_TRUNC,0644);
                }else{
                    fd = open(file,O_WRONLY|O_CREAT|O_APPEND,0644);
                }   
                dup2(fd,redir_info.fd);
                break;
        }
        if(fd < 0){
            perror("open");
            return -1;
        }
        close(fd);
    }
    return saved_fd;
}

void restore_descriptor(RedirInfo redir_info,int saved_fd){
    dup2(saved_fd,redir_info.fd);
    close(saved_fd);
}

int run_commands(ParsedCmd parsed_cmd,History history){

    char* cmd = parsed_cmd.args.elements[0];
    int saved_fd = apply_redirections(parsed_cmd.redir_info);

    if(is_builtin(cmd)){
        run_builtin_cmd(parsed_cmd.args,history);
    }else{
        run_program(parsed_cmd.args);
    }

    if(saved_fd != -1){
        restore_descriptor(parsed_cmd.redir_info,saved_fd);
    }
}