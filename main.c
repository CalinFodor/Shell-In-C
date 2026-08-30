#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


//----TOKENIZER-----
 
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

void copy_string(char **dest, char *src)
{
    int s_len = strlen(src) + 1;
    *dest = (char *)malloc(s_len);
    strncpy(*dest, src, s_len - 1);
    (*dest)[s_len - 1] = '\0';
}

int tokenize_redirects(char line[], char *tokens[])
{
    char token[128] = {'\0'};
    int token_index = 0;

    int line_length = strlen(line);
    int in_token = 0;

    for (int i = 0; i < line_length; i++)
    {
        char character = line[i];

        if (isdigit(character) && !in_token && i != line_length - 1)
        {
            char next_character = line[i + 1];
            if (next_character == '>' || next_character == '<')
            {
                add_char(token, character);
                add_char(token, next_character);
                int i_moves = 1;

                if (i <= line_length - 3)
                {
                    char next_next_character = line[i + 2];
                    if (next_character == next_next_character)
                    {
                        add_char(token, next_character);
                        i_moves++;
                    }
                }

                add_string(tokens, token, token_index++);
                i += i_moves;
                token[0] = '\0';
                continue;
            }
        }

        if (character == '>' || character == '<')
        {
            if (in_token)
            {
                add_string(tokens, token, token_index);
                token_index++;
                token[0] = '\0';
                in_token = 0;
            }

            add_char(token, character);

            if (i != line_length - 1)
            {
                char next_character = line[i + 1];
                if (character == next_character)
                {
                    add_char(token, character);
                    i++;
                }
            }

            add_string(tokens, token, token_index++);
            token[0] = '\0';
        }
        else if (character != ' ')
        {
            add_char(token, character);
            in_token = 1;
        }
        else if (in_token)
        {
            add_string(tokens, token, token_index);
            token_index++;
            token[0] = '\0';
            in_token = 0;
        }

        if (i == line_length - 1 && in_token)
        {
            add_string(tokens, token, token_index++);
        }
    }
    return token_index;
}

//-----PARSER-----

int starts_with_digit(char *token)
{
    if (isdigit(token[0]))
        return 1;
    return 0;
}

int has_arrows(char *token, int idx)
{
    if ((token[idx] == '>' || token[idx] == '<') &&  token[idx + 1] == '\0') return 1;
    if((token[idx] == '>' || token[idx] == '<') && (token[idx + 1] == token[idx] && token[idx + 2] == '\0')) return 1;
    return 0;
}

int is_redirect_operator(char *token)
{
    if (starts_with_digit(token) && has_arrows(token, 1)) return 1;
    if (has_arrows(token,0)) return 1;
    return 0;
}

typedef struct _Redirect
{
    char *stream;
    char *operation;
    char *target;
} Redirect;

Redirect new_redirect(char* redir_operator,char* target)
{
    Redirect redir;

    char stream[2];
    stream[1] = '\0';

    int arrow_start = 0;

    if(starts_with_digit(redir_operator)){
        stream[0] = redir_operator[0];
        arrow_start = 1;
    }else if(redir_operator[0] == '<'){
        stream[0] = '0';
    }else{
        stream[0] = '1';
    }

    copy_string(&redir.stream,stream);
    copy_string(&redir.operation, redir_operator + arrow_start);
    copy_string(&redir.target, target);

    return redir;
}

void print_tokens(char* tokens[],int n){
    for(int i=0;i<n;i++)
        printf("[%s] ",tokens[i]);
    printf("\n");
}

void free_tokens(char* tokens[],int n){
    for(int i=0;i<n;i++)
        free(tokens[i]);
}

void parse_redirects(char *tokens[], int n)
{
    char *argv[128];
    int argv_index = 0;

    Redirect redirects[128];
    int redirects_index = 0;

    int i;
    for (i = 0; i < n; i++)
    {        
        if(is_redirect_operator(tokens[i])){
            if(i == n - 1){
                printf("[ERROR] Expected filename\n");
                return;
            }
            Redirect redir = new_redirect(tokens[i],tokens[i+1]);
            redirects[redirects_index++] = redir;
            i++;
        }else{
            add_string(argv,tokens[i],argv_index++);
        }
    }

    printf("argv=[");

    for(int i=0;i<argv_index;i++){
        if(i != argv_index - 1){
            printf("'%s', ",argv[i]);
        }else{
            printf("'%s']\n",argv[i]);
        }
        free(argv[i]);
    }

    for(int i=0;i<redirects_index;i++){
       printf("redir fd=%s op=%s target=%s\n",redirects[i].stream,redirects[i].operation,redirects[i].target);
       free(redirects[i].operation);
       free(redirects[i].stream);
       free(redirects[i].target);
    }

    free_tokens(tokens,n);
}


void process_redirects(char line[])
{
    char *tokens[100];
    int n = tokenize_redirects(line, tokens);
    parse_redirects(tokens,n);
}

int main(void)
{
    char line[1024];
    while (fgets(line, sizeof line, stdin))
    {
        if (line[0] == '\n' || line[0] == 0)
            continue;
        if(line[strlen(line) - 1] == '\n')
            line[strlen(line) - 1] = '\0';
        process_redirects(line);
    }
    return 0;
}