#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum _TokenizeState{
    DEFAULT,
    SINGLE_QUOTE,
    DOUBLE_QUOTE
} TokenizeState;

int tokenize(char line[],char* tokens[]){
    
    int token_index = 0;
    int token_start = 0;
    TokenizeState current_state = DEFAULT;

    int line_length = strlen(line);

    for(int i=0;i<line_length;i++){
        char character = line[i];
        
        int token_size = i - token_start;
        
        if (character == '\'' && current_state == DEFAULT){
            current_state = SINGLE_QUOTE;
            token_start++;
            continue;
        }

        switch (current_state){
            case DEFAULT:
                if ((character == ' ' || i == line_length - 1) && token_size != 0 ){
                    tokens[token_index] = (char*)malloc(token_size);

                    strncpy(tokens[token_index],line + token_start,token_size);
                    tokens[token_index][token_size] = '\0';
                    token_index++;

                    token_start = token_start + token_size + 1;
                    token_size = 0;
                }else if(character == ' ' && token_size == 0){
                    token_start++;
                }
                break;

            case SINGLE_QUOTE:
                if(character == '\''){
                    tokens[token_index] = (char*)malloc(token_size);

                    strncpy(tokens[token_index],line + token_start,token_size);
                    tokens[token_index][token_size] = '\0';
                    token_index++;

                    token_start = token_start + token_size + 1;
                    token_size = 0;

                    current_state = DEFAULT;
                }else if(i == line_length - 1){
                    return -1;
                }
                break;

        }

        
    }
    return token_index;
}

void print_tokens(char* tokens[],int n){

    if (n == -1){
        printf("ERR unterminated quote.\n");
        return;
    }

    for(int i=0;i<n;i++){
        printf("[%s]",tokens[i]);
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
    printf("Put commands for tokenization\n");
    while (fgets(line, sizeof line, stdin)) {
        if (line[0] == '\n' || line[0] == 0) continue;
        tokenize_line(line);

    }
    return 0;
}
