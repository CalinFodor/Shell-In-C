#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

 
void add_char(char *s, char c)
{
    int l = strlen(s);
    s[l] = c;
    s[l + 1] = '\0';
}

void add_string(char *strings[], char *string, int poz)
{
    int s_len = strlen(string) + 1;
    strings[poz] = (char *)malloc(s_len);
    strncpy(strings[poz], string, s_len - 1);
    strings[poz][s_len - 1] = '\0';
}

void copy_string(char *dest, char *src)
{
    int s_len = strlen(src) + 1;
    dest = (char *)malloc(s_len);
    strncpy(dest, src, s_len - 1);
    dest[s_len - 1] = '\0';
}


int tokenize(char line[],char* tokens[],char separator){

    int token_index = 0;

    int token_start = 0;
    int in_token = 0;

    int line_length = strlen(line);

    for(int i=0;i<line_length;i++){
        char character = line[i];

        if(character == separator && in_token){
            int s_len = i - token_start + 1;
            
            tokens[token_index] = (char*)malloc(s_len);
            strncpy(tokens[token_index],line + token_start,s_len - 1);
            tokens[token_index++][s_len - 1] = '\0';

            in_token = 0;
            token_start = i;
            
        }else if(character != separator){
            if(!in_token){
                token_start = i;
                in_token = 1;
            }
        }

        if(in_token && i == line_length - 1){
            int len = line_length - token_start;

            tokens[token_index] = (char*)malloc(len+1);
            strncpy(tokens[token_index],line + token_start,len);
            tokens[token_index++][len] = '\0';
        }

    }
    return token_index;
}

void print_tokens(char* tokens[],int n){
    for(int i=0;i<n;i++){
        printf("[%s] ",tokens[i]);
        free(tokens[i]);
    }
    printf("\n");
}

#define HOME "/home/user"
char CWD[128] = HOME;
char OLDPWD[128] = {'\0'};

void parent_dir(char path[]){
    if(strcmp(path,"/") == 0)
        return;
    
    for(int i=strlen(path) - 1;i>=0;i--){
        if(path[i] == '/'){
            if(i != 0){
                path[i] = '\0';
                return;
            }
        }
    }

    path[0] = '/';
    path[1] = '\0';
}

void add_dir_to_path(char path[],char* dir){
    int path_length = strlen(path);
    int dir_length = strlen(dir);

    int offset = 0;
    if(path_length != 1){
        path[path_length] = '/';
        offset = path_length + 1;
    }else{
        offset = path_length;
    }

    strncpy(path+offset,dir,dir_length);
    path[offset + dir_length] = '\0';
}

void parse_path_tokens(char* tokens[],int n){

    for(int i=0;i<n;i++){
        char* token = tokens[i];

        if(strcmp(token,".") == 0){
            continue;
        }else if(strcmp(token,"..") == 0){
            parent_dir(CWD);           
        }else{
            add_dir_to_path(CWD,token);
        }
    }

}

void pwd(){
    printf("%s\n",CWD);
}

void cd(char* tokens[],int n){
    if(n > 2){
        printf("cd: invalid args");
        return;
    }

    char* path = tokens[1];
    if(n == 1 || strcmp(path,"~") == 0){
        strcpy(OLDPWD,CWD);
        strcpy(CWD,HOME);
    }else if(strcmp(path,"-") == 0){
        if(!strlen(OLDPWD)){
            printf("cd: OLDPWD not set\n");
            return;
        }else{
            char temp[128];

            strcpy(temp,CWD);
            strcpy(CWD,OLDPWD);
            strcpy(OLDPWD,temp);
        }
    }else if(path[0] == '/'){
        //absolute path
        strcpy(OLDPWD,CWD);
        
        strcpy(CWD,"/");

        char* path_tokens[128];
        int np = tokenize(path,path_tokens,'/');
        parse_path_tokens(path_tokens,np);
        
    }else{
        strcpy(OLDPWD,CWD);

        char* path_tokens[128];
        int np = tokenize(path,path_tokens,'/');
        parse_path_tokens(path_tokens,np);
    }
    pwd();
}


void execute_commands(char* tokens[],int n){
    char* cmd = tokens[0];

    if(strcmp(cmd,"pwd") == 0){
        pwd();
    }else if(strcmp(cmd,"cd") == 0){
        cd(tokens,n);
    }else{
        printf("ERROR: Unknown command\n");
    }
}


void parse_and_execute_commands(char line[]){
    char* tokens[128];
    int n = tokenize(line,tokens,' ');
    execute_commands(tokens,n);
}


int main(void)
{
    char line[1024];
    while (fgets(line, sizeof line, stdin))
    {
       int line_length = strlen(line);
       if(line[line_length - 1] == '\n'){
            line[line_length - 1] = '\0';
       }
       if(line[0] == '\n') continue;

       parse_and_execute_commands(line);
    }

    return 0;
}