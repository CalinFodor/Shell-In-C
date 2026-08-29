#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void add_char(char *s, char c) {

    while (*s++);
  
    *(s - 1) = c;
  
    *s = '\0';
}

void add_string(char* strings[],char* string,int poz){
    int s_len = strlen(string) + 1;
    strings[poz] = (char*)malloc(s_len);
    strncpy(strings[poz],string,s_len-1);
    strings[poz][s_len - 1] = '\0';
}

int tokenize_pipeline(char line[], char *tokens[])
{
    char token[128] = {'\0'};
    int token_index = 0;

    int line_length = strlen(line);
    int in_token = 0;

    for(int i=0;i<line_length;i++){
        char character = line[i];

        if(character == '|'){
            if(in_token){
                add_string(tokens,token,token_index);
                token_index++;
                token[0] = '\0';
            }

            add_string(tokens,"|",token_index);
            token_index++;
            in_token=0;
        }else if(character != ' '){
            add_char(token,character);
            in_token = 1;
        }else if(in_token){
            add_string(tokens,token,token_index);
            token_index++;
            token[0] = '\0';
            in_token = 0;
        }

        if(i == line_length - 1 && in_token){
            add_string(tokens,token,token_index++);
        }
    }
    return token_index;
}

int is_pipe(char* token){
    if (strcmp(token,"|") == 0)
        return 1;
    return 0;
}

int validate_pipeline(char* tokens[],int n){
    for(int i=0;i<n;i++){
        if((i == 0 || i == n-1) && is_pipe(tokens[i])){
            return -1;
        }
        if(is_pipe(tokens[i])){
            if(is_pipe(tokens[i - 1]) || is_pipe(tokens[i+1])){
                return -1;
            }
        }
    }
    return n;
}

void print_tokens(char* tokens[],int n){
    if (n == -1){
        printf("ERR syntax error: empty command in pipeline\n");
        return;
    }

    for(int i=0;i<n;i++){
        printf("%s",tokens[i]);
        if(i != n - 1)
            printf(" ");
    }
    printf("\n");
}

void free_tokens(char* tokens[],int n){
    for(int i=0;i<n;i++)
        free(tokens[i]);
}

void parse_pipeline(char line[]){
    char* tokens[100];
    int n = tokenize_pipeline(line,tokens);
    n = validate_pipeline(tokens,n);
    print_tokens(tokens,n);
    free_tokens(tokens,n);
}

int main(void)
{
    char line[1024];
    while (fgets(line, sizeof line, stdin))
    {
        if (line[0] == '\n' || line[0] == 0)
            continue;
        parse_pipeline(line);
    }
    return 0;
}