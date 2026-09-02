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

bool is_pipe(char* s){
    return (strcmp(s,"|") == 0);
}

int count_pipes(char* tokens[],int n){
    int count = 0;
    for(int i=0;i<n;i++){
        if(is_pipe(tokens[i])){
            count++;
        }
    }
    return count;
}


void pipe_plan(char line[]){
    char* tokens[128];
    int n_tokens = tokenize(line,tokens);

    int pipe_count = count_pipes(tokens,n_tokens);

    int n = 2*pipe_count-1;

    int cmd_i = 0;
    bool in_command = false;
    for(int i=0;i<n_tokens;i++){
        char* token = tokens[i];
        if(!in_command && !is_pipe(token)){
            printf("CMD %d %s\n",cmd_i,token);
            in_command = true;
            if(pipe_count == 0){
                break;
            }
        }

        if(is_pipe(token) || i == n_tokens - 1){
            int in_dup2 = 10 + 2*(cmd_i - 1);
            int out_dup2 = 11 + 2*cmd_i;

            //first pipe not dup2 stdin
            if(cmd_i != 0){
                printf("  DUP2 %d 0\n",in_dup2);
            }

            //last pipe not dup2 stdout
            if(cmd_i != pipe_count){
                printf("  DUP2 %d 1\n",out_dup2);
            }
            
          
            for(int i=0;i<=n;i++){
                printf("  CLOSE %d\n",10+i);
            }
            cmd_i++;
            in_command = false;
        }
    }
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
       pipe_plan(line);
    }

    return 0;
}