#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum _TokenizeState{
    DEFAULT,
    SINGLE_QUOTE,
    DOUBLE_QUOTE
} TokenizeState;

void add_char(char* s,char c){
    int l = strlen(s);  
  
    s[l] = c;
  
    s[l + 1] = '\0';
}

int tokenize(char line[],char* tokens[]){
    
    int token_index = 0;
    TokenizeState current_state = DEFAULT;

    char token[128] = {'\0'};

    int line_length = strlen(line);

    int space_encountered = 0;
    int escaped = 0;

    for(int i=0;i<line_length;i++)
    {
        char character = line[i];
        
        if (character == '\'' && current_state != SINGLE_QUOTE){
            current_state = SINGLE_QUOTE;
            continue;
        }

        if (character == '"' && current_state == DEFAULT){
            current_state = DOUBLE_QUOTE;
            continue;
        }

        int token_length = strlen(token);
        switch (current_state){
            case DEFAULT:
                if(character == '\\'){
                   char next_char = line[i+1];
                   add_char(token,next_char);
                   i++;
                   continue; 
                }

                if(character != ' '){
                    add_char(token,character);
                    token_length++;
                }    

                if((character == ' ' || i == line_length - 1) && token_length!= 0){
                    tokens[token_index] = (char*)malloc(token_length);
                    strncpy(tokens[token_index],token,token_length);
                    tokens[token_index++][token_length] = '\0';
                    //reset token
                    token[0] = '\0';
                }

                break;

            case SINGLE_QUOTE:
                if(character == '\''){
                    if(token_length == 0) token_length = 1;

                    tokens[token_index] = (char*)malloc(token_length);
                    strncpy(tokens[token_index++],token,token_length);        

                    //reset token
                    token[0] = '\0';
                    current_state = DEFAULT;
                }else if(i == line_length - 1){
                    return -1;
                }
                else {
                    add_char(token,character);
                }    

                break;
            case DOUBLE_QUOTE:
                if(character != ' ' && space_encountered){
                    space_encountered = 0;
                }

                if(character == '"'){

                    if(token_length == 0) token_length = 1;

                    tokens[token_index] = (char*)malloc(token_length);
                    strncpy(tokens[token_index++],token,token_length);

                    //reset token
                    token[0] = '\0';
                    current_state = DEFAULT;
                }else if(i == line_length - 1){
                    return -1;
                }else if(character == '\\'){
                    char next_char = line[i+1];
                    add_char(token,next_char);
                    i++;
                }
                else if(character != ' '){
                    add_char(token,character);
                }
                else if(character == ' ' && !space_encountered){
                    add_char(token,character);
                    space_encountered = 1;
                }
                break;
        }
    }

    return token_index;
}

void print_tokens(char* tokens[],int n){

    if (n == -1){
        printf("ERR unterminated quote\n");
        return;
    }

    for(int i=0;i<n;i++){
        printf("[%s]",tokens[i]);
        if (i != n-1){
            printf(" ");
        }
    }
    printf("\n");
}

void free_tokens(char* tokens[],int n){
    for(int i=0;i<n;i++){
        free(tokens[i]);
    }
}

void tokenize_line(char line[]){
    char* tokens[100];
    int n = tokenize(line,tokens);
    print_tokens(tokens,n);
    free_tokens(tokens,n);
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof line, stdin)) {
        if (line[0] == '\n' || line[0] == 0) continue;
        tokenize_line(line);

    }
    return 0;
}
