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

bool starts_with(char *str, char *pre)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

bool is_hist_command(char* cmd){
    return starts_with(cmd,"!") || starts_with(cmd,":");
}

bool is_number(char* str){
    int s_len = strlen(str);
    for(int i=0;i<s_len;i++){
        if(!isdigit(str[i])){
            return false;
        }
    }
    return true;
}

typedef struct _CommandHist{
    char* commands[128];
    int idx;
}CommandHist;

CommandHist COMMAND_HISTORY;

void add_command_to_hist(char* command){
    add_string(COMMAND_HISTORY.commands,command,COMMAND_HISTORY.idx++);
}

void print_hist(){
    int n = COMMAND_HISTORY.idx;
    for(int i=0;i<n;i++){
        printf("%d: %s\n",i+1,COMMAND_HISTORY.commands[i]);
    }
}

void print_last_command(){
    char* last_command = COMMAND_HISTORY.commands[COMMAND_HISTORY.idx - 1];
    printf("RAN %s\n",last_command);
    add_command_to_hist(last_command);
}

void print_n_command(int n){
    int command_len = COMMAND_HISTORY.idx;
    for(int i=0;i<command_len;i++){
        if(i+1 == n){
            char *cmd = COMMAND_HISTORY.commands[i];
            printf("RAN %s\n",cmd);
            add_command_to_hist(cmd);
            return;
        }
    }
    printf("bash: !%d: event not found\n",n);
}

void print_match_command(char* str){
    int command_len = COMMAND_HISTORY.idx;
    bool found_match = false;
    for(int i=0;i<command_len;i++){
        char* cmd = COMMAND_HISTORY.commands[i];
        if(starts_with(cmd,str)){
            printf("RAN %s\n",cmd);
            found_match = true;
            add_command_to_hist(cmd);
            break;
        }
    }

    if(!found_match)
        printf("bash: !%s: event not found\n",str);
}

void execute_command(char line[]){
    if(!is_hist_command(line)){
        add_command_to_hist(line);
        printf("RAN %s\n",line);
    }else{
        if(line[0] == '!'){
            if(line[1] == '!'){
               print_last_command();
            }else if(is_number(line + 1)){
                int n = atoi(line+1);
                print_n_command(n);
            }else{
                char* str = line + 1;
                print_match_command(str);
            }
        }else{
            print_hist();
        }
    }
}

void init_hist(){
    COMMAND_HISTORY.idx = 0;
}

int main(void)
{
    init_hist();
    char line[1024];
    while (fgets(line, sizeof line, stdin))
    {
       if(line[0] == '\n') continue; 

       int line_length = strlen(line);
       if(line[line_length - 1] == '\n'){
            line[line_length - 1] = '\0';
       }
       if(line[0] == '\n') continue;
       execute_command(line);
    }
    
    return 0;
}