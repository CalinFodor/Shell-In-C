#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
 
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

char* copy_string(char *src,int len)
{
    char* dest = NULL;
    dest = (char *)malloc(len+1);
    strncpy(dest, src, len);
    dest[len] = '\0';
    return dest;
}

bool equal_strings(char* one,char* two){
    return (strcmp(one,two) == 0);
}

int tokenize(char line[],char* tokens[]){
    
    int token_idx = 0;

    int line_length = strlen(line);

    bool in_token = false;

    int token_start = 0;

    for(int i=0;i<line_length;i++){
        char character = line[i];
        if(character == ' ' && in_token){
            int token_len = i - token_start;
            tokens[token_idx++] = copy_string(line + token_start,token_len);
            in_token = false;
            token_start = i + 1;
        }else if(character == '>' && line[i+1] != '>'){
            int token_len = i - token_start + 1;
            tokens[token_idx++] = copy_string(line+token_start,token_len);
            in_token = false;
            token_start = i+1;
        }else if(character == ' '){
            token_start++;
        }else if(!in_token){
            in_token = true;
        }
    }

    if(in_token){
        int token_len = line_length - token_start;
        tokens[token_idx++] = copy_string(line + token_start,token_len);
    }
    return token_idx;

}

int FD = 100;

typedef enum _RedirDir{
    Input,
    Output
}RedirDir;

typedef enum _ModifyFlag{
    Trunc,
    Append
}ModifyFlag;



typedef struct  _RedirInfo{
    RedirDir redir_dir;
    ModifyFlag modify_flag;
    int fd;
} RedirInfo;

RedirInfo parse_redir_operator(char* redir_operator){
    RedirInfo redir_info;

    int arrow_start = 0;
    int op_len = strlen(redir_operator);

    bool custom_fd = false;
    if(isdigit(redir_operator[0])){
        redir_info.fd = redir_operator[0] - '0';
        arrow_start = 1;
        custom_fd = true;
    }

    if(redir_operator[arrow_start] == '>'){
        redir_info.redir_dir = Output;
        if(redir_operator[arrow_start + 1] == '>'){
            redir_info.modify_flag = Append;
        }else{
            redir_info.modify_flag = Trunc;
        }

        if(!custom_fd){
            redir_info.fd = 1;
        }
    }else if(redir_operator[arrow_start] == '<'){
        redir_info.redir_dir = Input;

        if(!custom_fd){
            redir_info.fd = 0;
        }
    }

    return redir_info;    

}

void redirect_plan(char line[]){
    char* tokens[128];
    int n = tokenize(line,tokens);

    char* redirect_operator = tokens[0];
    char* file = tokens[1];

    RedirInfo redir_info = parse_redir_operator(redirect_operator);

    if(file[0] == '&'){
        int old_fd = file[1] - '0';
        int new_fd = redir_info.fd;
        printf("DUP2 %d %d\n",old_fd,new_fd);
    }else{
        switch(redir_info.redir_dir){
            case Input:
                printf("OPEN %s RDONLY -> fd %d\n",file,FD);
                printf("DUP2 %d %d\n",FD,redir_info.fd);
                break;
            case Output:
                printf("OPEN %s WRONLY|CREAT|",file);    

                if(redir_info.modify_flag == Trunc){
                   printf("TRUNC"); 
                }else{
                   printf("APPEND"); 
                }   

                printf(" -> fd %d\n",FD);
                printf("DUP2 %d %d\n",FD,redir_info.fd);
                break;
        }

        printf("CLOSE %d\n",FD);
    }
    FD++;
}

int main(void)
{

    char line[1024];
    while (fgets(line, sizeof line, stdin))
    {
       if(line[0] == '\n') continue; 

       int line_length = strlen(line);
       if(line[line_length - 1] == '\n'){
            line[line_length - 1] = '\0';
       }
       if(line[0] == '\n') continue;
       redirect_plan(line);
    }

    return 0;
}