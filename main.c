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

bool starts_with(char* pre,char* str){
    return strncmp(pre,str,strlen(pre)) == 0;
}

int get_command_exit(char* command){

    if(starts_with("OK",command)){
        return 0;
    }else{
        int exit_code = command[strlen(command) - 1] - '0';
        return exit_code;
    }
}

bool is_logical_operator(char* token){
    return equal_strings(token,"||") || equal_strings(token,"&&");
}

void exit_command_chain(int code){
    printf("EXIT: %d\n",code);
}

void evalute_commands(char line[]){
    
    int last_command_exit = -1;
    char* tokens[128];
    int n = tokenize(line,tokens);
    bool run_command = true;

    printf("RAN:");
    for(int i=0;i<n;i++){
        char* token = tokens[i];

        if(is_logical_operator(token)){
            // || case
            if(equal_strings(token,"||")){
                //if last command exit is 0 dont run the next command
                if(last_command_exit == 0)
                    run_command = false;
                else
                    run_command = true;
            // && case
            }else {
                if (last_command_exit == 0)
                    run_command = true;
                else
                    run_command = false;
            }

        }else if(run_command){
            last_command_exit = get_command_exit(token);
            printf(" %s",token);
        }

    }

    printf("\nEXIT: %d\n",last_command_exit);
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
       evalute_commands(line);
    }

    return 0;
}